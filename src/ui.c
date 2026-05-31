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
	ctx->layoutDepth = 1;
}

void uiRoot(UICtx *ctx, i32 winWidth, i32 winHeight) {
	ctx->layoutStack[0].size = newVec2i(winWidth, winHeight);
	ctx->layoutDepth = 1;
}

void uiBeginLayout(UICtx *ctx, const UILayoutOptions *options) {
	ctx->layoutDepth++;
	if (ctx->layoutDepth >= MAX_LAYOUT_STACK) {
		fprintf(stderr, "num layouts exceeded");
		exit(1);
	}

	UILayout *layout = ctx->layoutStack + (ctx->layoutDepth - 1);
	UILayout *prevLayout = ctx->layoutStack + (ctx->layoutDepth - 2);

	if (isNullVec2i(options->size)) {
		layout->size = newVec2i(0, 0);
	} else {
		layout->size = options->size;
	}
	if (isNullVec2i(options->position)) {
		layout->position = newVec2i(0, 0);
	} else {
		layout->position = options->position;
	}
	if (isNullVec4i(options->padding)) {
		layout->container = newVec4i(0, 0, 0, 0);
	} else {
		// calculate container bounds
		// layout->container = ...
	}
	if (options->sizing & UI_FILL_WIDTH) {
		layout->size.x = prevLayout->size.x;
	}

	if (options->bgColor.a > 0) {
		layout->bgColor = options->bgColor;

		SDL_FRect background = {layout->position.x, layout->position.y,
			layout->size.x, layout->size.y};

		SDL_SetRenderDrawColor(ctx->renderer, layout->bgColor.r, layout->bgColor.g, layout->bgColor.b, layout->bgColor.a);
		SDL_RenderFillRect(ctx->renderer, &background);
	}
}

void uiEndLayout(UICtx *ctx) {
	if (ctx->layoutDepth == 1) {
		fprintf(stderr, "too many end layouts");
		exit(1);
	}
	ctx->layoutDepth--;
}
