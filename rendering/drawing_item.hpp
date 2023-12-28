#pragma once

#include <types/rect2.hpp>
#include <types/color.hpp>
#include <types/transform2d.hpp>

#include <SDL_render.h>

#include <memory>

namespace sdl {

struct CanvasItem;
struct Texture;
class Viewport;

struct DrawingItem {
  std::shared_ptr<CanvasItem> canvas_item;

  virtual void draw(const Viewport *viewport);
  virtual Rect2 get_draw_rect(const Viewport *viewport) const;
};

struct TextureRectDrawingItem: public DrawingItem {
  std::shared_ptr<Texture> texture;

  Color texture_modulate;
  Rect2i src_region;
  Transform2D transform;
  SDL_RendererFlip flip;
 
  void draw(const Viewport *viewport) override;
  void draw_texture(const Viewport *viewport,
    const SDL_Rect &src_region,
    const SDL_FRect &destination,
    const double rotation,
    const SDL_FPoint &center = SDL_FPoint());

  Rect2 get_draw_rect(const Viewport *viewport) const override;
};

struct TextureDrawingItem: public DrawingItem {
  std::shared_ptr<Texture> texture;

  Color texture_modulate;
  Transform2D transform;
  SDL_RendererFlip flip;

  void draw(const Viewport *viewport) override;
  void draw_texture(const Viewport *viewport,
    const SDL_Rect &src_region,
    const SDL_FRect &destination,
    const double rotation,
    const SDL_FPoint &center = SDL_FPoint());

  Rect2 get_draw_rect(const Viewport *viewport) const override;
};

}