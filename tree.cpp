#include "tree.hpp"

using namespace sdl;

Tree::Tree() {
  root = new Item;
  window = new sdl::Window(get_window_rect(), get_window_title());
  rendering_server = window->rendering_server;
  running = false;
}

Tree::~Tree() {
  root->free();
  delete window;
}

void Tree::events() {
  SDL_PollEvent(event);
  handle_events();
  root->propagate_event(event);
}

void Tree::render() {
  handle_render();
  root->propagate_render();
  rendering_server->render();
}

void Tree::loop() {
  handle_loop();
  root->propagate_loop();
}

void Tree::initialize() {
}

void Tree::ended() {
  on_ended();
}

void Tree::start() {
  if (!window->intialized_successfully() || running)
    return;

  running = true;
  event = new SDL_Event;
  initialize();

  while (running) {
    events();
    loop();
    render();
  }

  ended();
}

void Tree::stop() {
  running = false;
}

Window *Tree::get_window() const {
  return window;
}

Item *Tree::get_root() const {
  return root;
}

void Tree::set_root(Item *new_root) {
  root = new_root;
}
