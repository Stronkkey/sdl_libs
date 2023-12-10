#include <rendering/rendering_server.hpp>
#include <rendering/canvas_item.hpp>

using namespace sdl;

RenderingServer::RenderingServer(): renderer(nullptr) {
}

RenderingServer::RenderingServer(SDL_Renderer *new_renderer): renderer(new_renderer) {
}

RenderingServer::~RenderingServer() {
  for (std::pair<uid, std::shared_ptr<CanvasItem>> iterator: canvas_items)
    destroy_uid(iterator.first);
}

void RenderingServer::render() {
  if (!renderer)
    return;

  SDL_RenderClear(renderer);
  for (auto iterator: canvas_items) {
    render_canvas_item(iterator.second);
  }
  SDL_RenderPresent(renderer);
}

std::shared_ptr<Texture> RenderingServer::get_texture_from_uid(const uid &grab_uid) const {
  auto iterator = textures.find(grab_uid);
  return iterator != textures.end() ? iterator->second : std::shared_ptr<Texture>(nullptr);
}

std::shared_ptr<CanvasItem> RenderingServer::get_canvas_item_from_uid(const uid &grab_uid) const {
  auto iterator = canvas_items.find(grab_uid);
  return iterator != canvas_items.end() ? iterator->second : std::shared_ptr<CanvasItem>(nullptr);
}

void RenderingServer::remove_uid(uid &destroying_uid) {
  destroy_uid(destroying_uid);
}

uid RenderingServer::create_new_uid() {
  uid new_uid;
  new_uid.id = index;
  index++;
  return new_uid;
}

void RenderingServer::destroy_uid(uid &destroying_uid) {
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
  for (std::shared_ptr<DrawingItem> drawing_item: canvas_item->drawing_items)
    drawing_item->draw(renderer);
}

uid RenderingServer::load_texture_from_path(const std::string &path) {
  SDL_Texture *texture = IMG_LoadTexture(renderer, path.c_str());

  if (texture == NULL)
    return uid();
  
  uid new_uid = create_new_uid();
  auto new_texture = std::make_shared<Texture>();

  new_texture->texture_reference = texture;
  int width = 0;
  int height = 0;
  SDL_QueryTexture(texture, &new_texture->format, NULL, &width, &height);

  new_uid.type = UID_RENDERING_TEXTURE;
  new_texture->size = Vector2i(width, height);
  textures.insert({new_uid, new_texture});

  return new_uid;
}

uid RenderingServer::create_canvas_item() {
  uid new_uid = create_new_uid();
  auto canvas_item = std::make_shared<CanvasItem>();
  new_uid.type = UID_RENDERING;
  canvas_items.insert({new_uid, canvas_item});
  return new_uid;
}

void RenderingServer::canvas_item_add_texture(const uid &texture_uid,
  const uid &canvas_item_uid,
  const Transform2D &transform,
  const SDL_RendererFlip flip)
{
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  std::shared_ptr<Texture> texture = get_texture_from_uid(texture_uid);

  if (canvas_item && texture) {
    auto texture_drawing_item = std::make_shared<TextureDrawingItem>();
    texture_drawing_item->canvas_item = canvas_item;

    texture_drawing_item->texture = texture;
    texture_drawing_item->flip = flip;
    texture_drawing_item->transform = transform;

    canvas_item->drawing_items.push_back(texture_drawing_item);
  }
}

void RenderingServer::canvas_item_add_texture_region(const uid &texture_uid,
  const uid &canvas_item_uid,
  const Rect2i &src_region,
  const Transform2D &transform,
  const SDL_RendererFlip flip)
{
  std::shared_ptr<Texture> texture = get_texture_from_uid(texture_uid);
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);

  if (canvas_item && texture) {
    auto texture_rect_drawing_item = std::make_shared<TextureRectDrawingItem>();
    texture_rect_drawing_item->canvas_item = canvas_item;

    texture_rect_drawing_item->texture = texture;
    texture_rect_drawing_item->src_region = src_region;
    texture_rect_drawing_item->transform = transform;
    texture_rect_drawing_item->flip = flip;
    
    canvas_item->drawing_items.push_back(texture_rect_drawing_item);
  }
}

void RenderingServer::canvas_item_set_transform(const uid &canvas_item_uid, const Transform2D &new_transform) {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  if (canvas_item)
    canvas_item->transform = new_transform;
}

void RenderingServer::canvas_item_set_parent(const uid &canvas_item_uid, const uid &parent_item_uid) {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  std::shared_ptr<CanvasItem> parent_canvas_item = get_canvas_item_from_uid(parent_item_uid);

  if (canvas_item)
    canvas_item->parent = parent_canvas_item;
  else
    canvas_item->parent = std::shared_ptr<CanvasItem>(nullptr);
}

void RenderingServer::canvas_item_clear(const uid &canvas_item_uid) {
  auto canvas_item_iterator = canvas_items.find(canvas_item_uid);

  if (canvas_item_iterator == canvas_items.end())
    return;

  canvas_item_iterator->second->drawing_items.clear();
}

Transform2D RenderingServer::canvas_item_get_transform(const uid &canvas_item_uid) const {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  return canvas_item ? canvas_item->transform : Transform2D::IDENTITY;
}

Transform2D RenderingServer::canvas_item_get_global_transform(const uid &canvas_item_uid) const {
  std::shared_ptr<CanvasItem> canvas_item = get_canvas_item_from_uid(canvas_item_uid);
  return canvas_item ? canvas_item->get_global_transform() : Transform2D::IDENTITY;
}
