#ifndef WINDOW
#define WINDOW

#include <types/rect2.hpp>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_log.h>

#include <string>

namespace sdl {

class RenderingServer;

class Window {
  
private:
  bool success;

  void fail(const std::string &error_message, const SDL_LogCategory category = SDL_LOG_CATEGORY_APPLICATION);

public:
  Window(const Rect2i &size = Rect2i(0, 0, 320, 240), const std::string &title = "Default");
  ~Window();

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Surface *surface;
  SDL_Event event;
  RenderingServer *rendering_server;

  void render();

  bool intialized_successfully() const;
};
};

#endif // !WINDOW
