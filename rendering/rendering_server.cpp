#include <rendering/canvas_item.hpp>
#include <rendering/drawing_item.hpp>
#include <rendering/rendering_server.hpp>
#include <rendering/viewport.hpp>

using namespace sdl;

RenderingServer::RenderingServer(Viewport *viewport): viewport(viewport),
  textures(),
  canvas_items(),
  background_color(Color(77, 77, 77, 255)),
  viewport_offset(sdl::Vector2::ZERO),
  index(1) {
}

RenderingServer::~RenderingServer() {
  for (std::pair<uint_t, std::shared_ptr<Texture>> iterator: textures)
    SDL_DestroyTexture(iterator.second->texture_reference);
}

void RenderingServer::set_default_background_color(const Color &new_background_color) {
  background_color = new_background_color;
}

Color RenderingServer::get_default_background_color() const {
  return background_color;
}

Vector2i RenderingServer::get_screen_size() const {
  return viewport->get_viewport_size();
}

void RenderingServer::render() {
  SDL_Renderer *renderer = viewport->get_renderer();

  SDL_RenderClear(viewport->get_renderer());
  for (auto iterator: canvas_items)
    render_canvas_item(iterator.second);

  SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
  SDL_RenderPresent(renderer);
}

std::shared_ptr<Texture> RenderingServer::get_texture_from_uid(const uid texture_uid) const {
  auto iterator = textures.find(texture_uid);
  return iterator != textures.end() ? iterator->second : std::shared_ptr<Texture>(nullptr);
}

std::shared_ptr<CanvasItem> RenderingServer::get_canvas_item_from_uid(const uid canvas_item_uid) const {
  auto iterator = canvas_items.find(canvas_item_uid);
  return iterator != canvas_items.end() ? iterator->second : std::shared_ptr<CanvasItem>(nullptr);
}

void RenderingServer::remove_uid(const uid destroying_uid) {
  destroy_uid(destroying_uid);
}

uid RenderingServer::create_new_uid() {
  uid new_uid = index;
  index++;
  return new_uid;
}

void RenderingServer::destroy_uid(const uid destroying_uid) {
  auto canvas_item_iterator = canvas_items.find(destroying_uid);
  auto texture_iterator = textures.find(destroying_uid);
  
  if (canvas_item_iterator != canvas_items.end()) {
    canvas_items.erase(canvas_item_iterator);
    canvas_item_iterator->second.reset(); // CanvasItem should be deleted now.
  }

  if (texture_iterator != textures.end()) {
    textures.erase(texture_iterator);
    SDL_DestroyTexture(texture_iterator->second->texture_reference);
    texture_iterator->second.reset();
  }

}

void RenderingServer::render_canvas_item(const std::shared_ptr<CanvasItem> &canvas_item) {
  if (!canvas_item || !canvas_item->is_visible() || canvas_item->drawing_items.empty())
    return;

  const Rect2i screen_rect = Rect2i(Vector2::ZERO, get_screen_size());

  for (std::shared_ptr<DrawingItem> drawing_item: canvas_item->drawing_items) {
    bool inside_viewport = screen_rect.intersects(drawing_item->get_draw_rect(viewport));

    if (inside_viewport)
      drawing_item->draw(viewport);
  }
}

RenderingServer::TextureInfo RenderingServer::get_texture_info_from_uid(const uid texture_uid) const {
  auto iterator = textures.find(texture_uid);
  TextureInfo texture_info;

  if (iterator == textures.end())
    return texture_info;

  texture_info.size = iterator->second->size;
  texture_info.format = iterator->second->format;
  return texture_info;
}

uid RenderingServer::load_texture_from_path(const std::string &path) {
  SDL_Texture *texture = IMG_LoadTexture(viewport->get_renderer(), path.c_str());

  if (texture == NULL)
    return uid();
  
  uid new_uid = create_new_uid();
  auto new_texture = std::make_shared<Texture>();

  new_texture->texture_reference = texture;
  int width = 0;
  int height = 0;
  SDL_QueryTexture(texture, &new_texture->format, NULL, &width, &height);

  new_texture->size = Vector2i(width, height);
  textures.insert({new_uid, new_texture});

  return new_uid;
}

uid RenderingServer::create_canvas_item() {
  uid new_uid = create_new_uid();
  auto canvas_item = std::make_shared<CanvasItem>();

  canvas_items.insert({new_uid, canvas_item});
  return new_uid;
}

void RenderingServer::canvas_item_add_texture(const uid texture_uid,
    const uid canvas_item_uid,
    const SDL_RendererFlip flip,
    const Color &modulate,
    const Transform2D &transform) {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  std::shared_ptr<Texture> texture = get_texture_from_uid(texture_uid);

  if (canvas_item && texture) {
    auto texture_drawing_item = std::make_shared<TextureDrawingItem>();
    texture_drawing_item->canvas_item = canvas_item;

    texture_drawing_item->texture = texture;
    texture_drawing_item->flip = flip;
    texture_drawing_item->transform = transform;
    texture_drawing_item->texture_modulate = modulate;

    canvas_item->drawing_items.push_back(texture_drawing_item);
  }
}

void RenderingServer::canvas_item_add_texture_region(const uid texture_uid,
    const uid canvas_item_uid,
    const Rect2i &src_region,
    const SDL_RendererFlip flip,
    const Color &modulate,
    const Transform2D &transform) {
  std::shared_ptr<Texture> texture = get_texture_from_uid(texture_uid);
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);

  if (canvas_item && texture) {
    auto texture_rect_drawing_item = std::make_shared<TextureRectDrawingItem>();
    texture_rect_drawing_item->canvas_item = canvas_item;

    texture_rect_drawing_item->texture = texture;
    texture_rect_drawing_item->src_region = src_region;
    texture_rect_drawing_item->transform = transform;
    texture_rect_drawing_item->flip = flip;
    texture_rect_drawing_item->texture_modulate = modulate;
    
    canvas_item->drawing_items.push_back(texture_rect_drawing_item);
  }
}

void RenderingServer::canvas_item_set_transform(const uid canvas_item_uid, const Transform2D &new_transform) {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  if (canvas_item)
    canvas_item->transform = new_transform;
}

void RenderingServer::canvas_item_set_parent(const uid canvas_item_uid, const uid parent_item_uid) {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  std::shared_ptr<CanvasItem> parent_canvas_item = get_canvas_item_from_uid(parent_item_uid);

  if (canvas_item)
    canvas_item->parent = parent_canvas_item;
  else
    canvas_item->parent = std::shared_ptr<CanvasItem>(nullptr);
}

void RenderingServer::canvas_item_set_modulate(const uid canvas_item_uid, const Color &new_modulate) {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  if (canvas_item)
    canvas_item->modulate = new_modulate;
}

void RenderingServer::canvas_item_set_blend_mode(const uid canvas_item_uid, const SDL_BlendMode blend_mode) {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  if (canvas_item)
    canvas_item->blend_mode = blend_mode;
}

void RenderingServer::canvas_item_clear(const uid canvas_item_uid) {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);

  if (canvas_item)
    canvas_item->drawing_items.clear();
}

Transform2D RenderingServer::canvas_item_get_transform(const uid canvas_item_uid) const {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  return canvas_item ? canvas_item->transform : Transform2D::IDENTITY;
}

Transform2D RenderingServer::canvas_item_get_global_transform(const uid canvas_item_uid) const {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  return canvas_item ? canvas_item->get_global_transform() : Transform2D::IDENTITY;
}

Color RenderingServer::canvas_item_get_modulate(const uid canvas_item_uid) const {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  return canvas_item ? canvas_item->modulate : Color::WHITE;
}

Color RenderingServer::canvas_item_get_global_modulate(const uid canvas_item_uid) const {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  return canvas_item ? canvas_item->get_absolute_modulate() : Color::WHITE;
}

bool RenderingServer::canvas_item_is_visible(const uid canvas_item_uid) const {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  return canvas_item ? canvas_item->visible : true;
}

void RenderingServer::canvas_item_set_visible(const uid canvas_item_uid, const bool visible) {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  if (canvas_item)
    canvas_item->visible = visible;
}

bool RenderingServer::canvas_item_is_globally_visible(const uid canvas_item_uid) const {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  return canvas_item ? canvas_item->is_visible() : true;
}

bool RenderingServer::canvas_item_is_visible_inside_viewport(const uid canvas_item_uid) const {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  if (!canvas_item || !canvas_item->is_visible() || canvas_item->drawing_items.empty())
    return false;
  
  const Rect2i screen_rect = Rect2i(Vector2::ZERO, get_screen_size());
  bool is_visible = true;

  for (std::shared_ptr<DrawingItem> drawing_item: canvas_item->drawing_items) {
    bool inside_viewport = screen_rect.intersects(drawing_item->get_draw_rect(viewport));

    if (!inside_viewport) {
      is_visible = false;
      break;
    }
  }

  return is_visible;
}