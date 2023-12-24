#include <items/tree.hpp>
#include <items/item.hpp>
#include <rendering/window.hpp>

#include <SDL_timer.h>

#include <thread>
#include <chrono>

using namespace sdl;

Tree::Tree(): running(false),
  fixed_frame_rate(60.0),
  frame_rate(60.0),
  window(new Window(get_window_rect(), get_window_title(), should_use_vsync())),
  rendering_server(window->rendering_server),
  root(new Item)
{
  root->set_name("Root");
  root->set_tree(this);
  running = false;
}

Tree::~Tree() {
  stop();
  root->free();
  delete window;
}

void Tree::events() {
  if (event->type == SDL_QUIT) {
    stop();
    return;
  }

  root->propagate_event(event);
}

void Tree::render(double delta) {
  if (!running)
    return;

  root->propagate_render(delta);
  rendering_server->render();
}

void Tree::loop(double delta) {
  root->propagate_loop(delta);
}

void Tree::initialize() {
}

void Tree::ended() {
}

void Tree::render_loop() {
  uint64_t now = SDL_GetPerformanceCounter();
  uint64_t last = 0;

  while (running) {
    last = now;
    now = SDL_GetPerformanceCounter();
    double delta = (double(now - last)) / SDL_GetPerformanceFrequency();

    auto wait_time = std::chrono::duration<double>(1 / frame_rate);
    std::this_thread::sleep_for(wait_time);
    
    render(delta);
  }
}

void Tree::main_loop() {
  uint64_t now = SDL_GetPerformanceCounter();
  uint64_t last = 0;

  while (running) {
    last = now;
    now = SDL_GetPerformanceCounter();
    double delta = (double(now - last)) / SDL_GetPerformanceFrequency();

    auto wait_time = std::chrono::duration<double>(1 / fixed_frame_rate);
    std::this_thread::sleep_for(wait_time);

    loop(delta);

    for (callback callable: deferred_callbacks)
      callable();
    for (Item *item: deferred_item_removal)
      item->free();

    deferred_callbacks.clear();
  }
}

void Tree::event_loop() {
  while (running) {
    SDL_WaitEvent(event);
    events();
  }
}

void Tree::start() {
  if (!window->intialized_successfully() || running)
    return;

  running = true;
  event = new SDL_Event;
  initialize();

  std::thread main_loop_thread = std::thread(&Tree::main_loop, this);
  std::thread event_loop_thread = std::thread(&Tree::event_loop, this);
  render_loop();

  main_loop_thread.join();
  event_loop_thread.join();
}

void Tree::stop() {
  if (running) {
    running = false;
    ended();
  }
}

void Tree::defer_callable(const callback &callable) {
  if (!callable)
    return;

  deferred_callbacks.push_back(callable);
}

void Tree::queue_free(Item *item) {
  deferred_item_removal.push_back(item);
}


Window *Tree::get_window() const {
  return window;
}

RenderingServer *Tree::get_rendering_server() const {
  return rendering_server;
}

Item *Tree::get_root() const {
  return root;
}

SDL_Event *Tree::get_event() const {
  return event;
}

void Tree::set_frame_rate(const double new_frame_rate) {
  frame_rate = new_frame_rate;
}

double Tree::get_frame_rate() const {
  return frame_rate;
}

void Tree::set_fixed_frame_rate(const double new_fixed_frame_rate) {
  fixed_frame_rate = new_fixed_frame_rate;
}

double Tree::get_fixed_frame_rate() const {
  return fixed_frame_rate;
}
