#include "render.h"

void render(App *app) {
	SDL_Renderer *renderer = app->renderer;

	Color bgColor = app->bgColor;
	SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b,  bgColor.a);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}
