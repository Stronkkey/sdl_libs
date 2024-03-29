#pragma once

#include <core/math/transform2d.hpp>

#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_events.h>

namespace sdl {

class RenderingServer;
class Window;

class Viewport {

private:
	bool vsync;

	Window *window;
	SDL_Renderer *renderer;

	Transform2D canvas_transform;

public:
	Viewport();
	~Viewport() = default;

	void create(Window *from_window);

	Vector2i get_viewport_size() const;

	constexpr Window *get_window() const {
		return window;
	}

	constexpr SDL_Renderer *get_renderer() const {
		return renderer;
	}

	void set_vsync_enabled(const bool vsync_enabled);
	constexpr bool is_vsync_enabled() const {
		return vsync;
	}

	void set_canvas_transform(const Transform2D &new_canvas_transform);
	constexpr const Transform2D &get_canvas_transform() const {
		return canvas_transform;
	}
};

}
