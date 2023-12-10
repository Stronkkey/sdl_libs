#ifndef CANVAS_ITEM
#define CANVAS_ITEM

#include <types/transform2d.hpp>
#include <types/rect2.hpp>

#include <SDL_render.h>

#include <memory>
#include <vector>

namespace sdl {

struct DrawingItem;

struct Texture {
  SDL_Texture *texture_reference;
  Vector2i size;
  uint32_t format;
};

struct CanvasItem {
  Transform2D transform = Transform2D::IDENTITY;
  std::shared_ptr<CanvasItem> parent;
  std::vector<std::shared_ptr<DrawingItem>> drawing_items;

  Transform2D get_global_transform() const;
};

// DrawingItem

struct DrawingItem {
  std::shared_ptr<CanvasItem> canvas_item;

  virtual void draw(SDL_Renderer*) {}
};

struct TextureRectDrawingItem: public DrawingItem {
  std::shared_ptr<Texture> texture;
  Rect2i src_region;
  Transform2D transform;
  SDL_RendererFlip flip;
  
  void draw(SDL_Renderer *renderer) override;
};

struct TextureDrawingItem: public DrawingItem {
  std::shared_ptr<Texture> texture;
  Transform2D transform;
  SDL_RendererFlip flip;

  void draw(SDL_Renderer *renderer) override;
};
/*
struct LineDrawingItem: public DrawingItem {
  Rect2 rect;
  void draw(SDL_Renderer *renderer) override;
};

struct RectDrawingItem: public DrawingItem {
  Rect2 rect;
  void draw(SDL_Renderer *renderer) override;
};*/

}

#endif // !CANVAS_ITEM
