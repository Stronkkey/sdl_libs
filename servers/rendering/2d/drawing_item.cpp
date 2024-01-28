#include "core/math/transform2d.hpp"
#include <algorithm>
#include <servers/rendering/2d/canvas_item.hpp>
#include <servers/rendering/2d/drawing_item.hpp>
#include <servers/rendering/viewport.hpp>
#include <servers/rendering/texture.hpp>

using namespace sdl;

void DrawingItem::draw(const Viewport *viewport) {
	if (!canvas_item.expired())
		_draw(viewport);
}

Rect2 DrawingItem::get_draw_rect() const {
	if (!canvas_item.expired())
		return _get_draw_rect();

	return Rect2::EMPTY;
}

void DrawingItem::_draw(const Viewport*) {
}

Rect2 DrawingItem::_get_draw_rect() const {
	return Rect2::EMPTY;
}

Rect2 TextureDrawingItem::_get_draw_rect() const {
	const Transform2D &global_transform = canvas_item.lock()->get_global_transform();
	const Vector2 position = global_transform.origin + transform.origin;
	const Vector2 size = global_transform.scale * transform.scale * (use_region ? src_region.get_size() : texture.lock()->size);

	return Rect2(position, size);
}

void TextureDrawingItem::_draw(const Viewport *viewport) {
	if (texture.expired())
		return;

	const std::shared_ptr<Texture> texture = this->texture.lock();
	std::shared_ptr<CanvasItem> canvas_item = this->canvas_item.lock();

	const Color &modulate = texture_modulate * canvas_item->get_global_modulate();

	SDL_SetTextureAlphaMod(texture->texture_reference, modulate.a);
	SDL_SetTextureColorMod(texture->texture_reference, modulate.r, modulate.g, modulate.b);
	SDL_SetTextureBlendMode(texture->texture_reference, canvas_item->blend_mode);
	SDL_SetTextureScaleMode(texture->texture_reference, canvas_item->scale_mode);
	
	const Transform2D &global_transform = canvas_item->get_global_transform() * viewport->get_canvas_transform();
	const Rect2i source_region = use_region ? src_region : Rect2i(Vector2::ZERO, texture->size);
	const double rotation = global_transform.rotation + transform.rotation;
	Rect2 final_draw_rect = _get_draw_rect() * viewport->get_canvas_transform();
	final_draw_rect.floored();

	SDL_Rect final_source_region = source_region.to_sdl_rect();
	SDL_FRect final_destination = final_draw_rect.to_sdl_frect();
	SDL_Renderer *renderer = viewport->get_renderer();
	SDL_Texture *final_texture = texture->texture_reference;

	if (rotation == 0.0)
		SDL_RenderCopyF(renderer, final_texture, &final_source_region, &final_destination);
	else
		SDL_RenderCopyExF(renderer, final_texture, &final_source_region, &final_destination, rotation, NULL, flip);
}

void RectDrawingItem::_draw(const Viewport *viewport) {
	std::shared_ptr<CanvasItem> canvas_item = this->canvas_item.lock();

	const Transform2D &global_transform = canvas_item->get_global_transform() * viewport->get_canvas_transform();
	
	SDL_Renderer *renderer = viewport->get_renderer();
	SDL_FRect rect = rectangle;

	rect.x = std::floor(rect.x + global_transform.origin.x);
	rect.y = std::floor(rect.y + global_transform.origin.y);
	rect.w = std::floor(rect.w * global_transform.scale.x);
	rect.h = std::floor(rect.h * global_transform.scale.y);

	SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
	SDL_SetRenderDrawBlendMode(renderer, canvas_item->blend_mode);
	SDL_RenderFillRectF(renderer, &rect);
}

Rect2 RectDrawingItem::_get_draw_rect() const {
	std::shared_ptr<CanvasItem> canvas_item = this->canvas_item.lock();

	return Rect2(rectangle) * canvas_item->get_global_transform();
}

void RectsDrawingItem::_draw(const Viewport *viewport) {
	if (rectangles.empty())
		return;

	std::shared_ptr<CanvasItem> canvas_item = this->canvas_item.lock();
	
	const Transform2D &global_transform = canvas_item->get_global_transform();
	SDL_Renderer *renderer = viewport->get_renderer();

	SDL_SetRenderDrawColor(viewport->get_renderer(), modulate.r, modulate.g, modulate.b, modulate.a);
	SDL_SetRenderDrawBlendMode(viewport->get_renderer(), canvas_item->blend_mode);

	for (const SDL_FRect &rectangle: rectangles) {
		SDL_FRect frect = rectangle;

		frect.x = std::floor(frect.x + global_transform.origin.x);
		frect.y = std::floor(frect.y + global_transform.origin.y);
		frect.w = std::floor(frect.w * global_transform.scale.x);
		frect.h = std::floor(frect.h * global_transform.scale.y);

		SDL_RenderFillRectF(renderer, &frect);
	}
}

Rect2 RectsDrawingItem::_get_draw_rect() const {
	const Transform2D &global_transform = canvas_item.lock()->get_global_transform();
	Rect2 final_rect = Rect2();

	for (const SDL_FRect &frect: rectangles)
		final_rect = final_rect.merge(Rect2(frect));

	final_rect.x += global_transform.origin.x;
	final_rect.y += global_transform.origin.y;
	final_rect.w += global_transform.scale.x;
	final_rect.h += global_transform.scale.y;
	final_rect.rounded();
	return final_rect;
}

void LineDrawingItem::_draw(const Viewport *viewport) {
	std::shared_ptr<CanvasItem> canvas_item = this->canvas_item.lock();

	const Transform2D global_transform = canvas_item->get_global_transform() * viewport->get_canvas_transform();
	SDL_Renderer *renderer = viewport->get_renderer();

	float x_1 = std::round(start_point.x + global_transform.origin.x);
	float y_1 = std::round(start_point.y + global_transform.origin.y);
	float x_2 = std::round((end_point.x + global_transform.origin.x) * global_transform.scale.x);
	float y_2 = std::round((end_point.y + global_transform.origin.y) * global_transform.scale.y);

	SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
	SDL_SetRenderDrawBlendMode(renderer, canvas_item->blend_mode);
	SDL_RenderDrawLineF(renderer, x_1, y_1, x_2, y_2);
}

Rect2 LineDrawingItem::_get_draw_rect() const {
	std::shared_ptr<CanvasItem> canvas_item = this->canvas_item.lock();

	const Transform2D &global_transform = canvas_item->get_global_transform();
	return Rect2(Vector2(start_point) + global_transform.origin, end_point).remove_negative_size().round();
}

void LinesDrawingItem::_draw(const Viewport *viewport) {
	if (points.size() < 2)
		return;
	
	std::shared_ptr<CanvasItem> canvas_item = this->canvas_item.lock();

	const Transform2D &global_transform = canvas_item->get_global_transform();
	const size_t points_size = points.size();
	SDL_Renderer *renderer = viewport->get_renderer();

	SDL_SetRenderDrawColor(renderer, modulate.r, modulate.g, modulate.b, modulate.a);
	SDL_SetRenderDrawBlendMode(renderer, canvas_item->blend_mode);

	for (size_t i = 0; i < points_size; i++) {
		if (i + 1 >= points_size)
			break;

		float x_1 = std::round(points[i].x + global_transform.origin.x);
		float y_1 = std::round(points[i].y + global_transform.origin.y);
		float x_2 = std::round(points[i + 1].x + global_transform.scale.x);
		float y_2 = std::round(points[i + 1].y + global_transform.scale.y);

		SDL_RenderDrawLineF(renderer, x_1, y_1, x_2, y_2);
	}
}

Rect2 LinesDrawingItem::_get_draw_rect() const {
	Rect2 rect = Rect2();

	const Transform2D &global_transform = canvas_item.lock()->get_global_transform();

	for (const SDL_FPoint &point: points)
		rect.expand_to(point);

	rect.x += global_transform.origin.x;
	rect.y += global_transform.origin.y;
	rect.w *= global_transform.scale.x;
	rect.h *= global_transform.scale.y;
	rect.rounded();
	return rect;
}