#include "ui.h"
#include <string.h>
#include <stdlib.h>

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color) {
	SDL_Surface* surface;
	SDL_Color sdl_color = {color.r, color.g, color.b, color.a};
	surface = TTF_RenderText_Blended(font, text, 0, sdl_color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	return texture;
}

void uiInitCtx(UICtx *ctx, SDL_Renderer *renderer) {
	ctx->renderer = renderer;
}

void uiBeginLayout(UICtx *ctx, const UILayoutOptions *options) {
	ctx->layoutDepth++;
	if (ctx->layoutDepth >= MAX_LAYOUT_STACK) {
		// throw an error / exit
		return;
	}
}

void uiEndLayout(UICtx *ctx) {
	ctx->layoutDepth--;
}
