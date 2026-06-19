#include "window.h"

SDL_Window *createSDLWindow(int x, int y) {
	SDL_Window *window = SDL_CreateWindow("ChipSim", x, y, SDL_WINDOW_RESIZABLE);
	if (!window) {
		fprintf(stderr, "failed to open window: %s", SDL_GetError());
		exit(1);
	}
	return window;
}

SDL_Renderer *createSDLRenderer(SDL_Window *window) {
	SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		fprintf(stderr, "failed to create renderer: %s", SDL_GetError());
		exit(1);
	}
	return renderer;
}

void createWindow(tWindow *window, Vec2i size) {
    window->sdlWindow = createSDLWindow(size.x, size.y);
    window->renderer = createSDLRenderer(window->sdlWindow);
}
void updateWindow(tWindow *window) {
	SDL_GetWindowSize(window->sdlWindow, &window->size.x, &window->size.y);
}
void windowSetMinSize(tWindow *window, Vec2i size) {
	SDL_SetWindowMinimumSize(window, size.x, size.y);
}
void closeWindow(tWindow *window) {
	SDL_DestroyRenderer(window->renderer);
	SDL_DestroyWindow(window->sdlWindow);
}