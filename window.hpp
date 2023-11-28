#include <SDL.h>
#include <string>

#include "rendering_server.hpp"
#include "rect2.hpp"

namespace sdl {

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

  bool intialized_successfully() const;
};
};
