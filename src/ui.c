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

void initUICtx(UICtx *ctx) {
	if (ctx->renderer == NULL) {
		fprintf(stderr, "UICtx.renderer must be set before calling uiInitCtx()");
	}
	if (ctx->font == NULL) {
		fprintf(stderr, "UICtx.font must be set before calling uiInitCtx()");
	}
	if (ctx->input == NULL) {
		fprintf(stderr, "UICtx.input must be set before calling uiInitCtx()");
	}
	ctx->layoutDepth = 1;
}

void uiBeginRoot(UICtx *ctx, i32 winWidth, i32 winHeight) {
	ctx->layoutStack[0].size = newVec2i(winWidth, winHeight);
	ctx->layoutStack[0].cursorPos = newVec2i(0, 0);
	ctx->layoutDepth = 1;

	ctx->onHover = NULL;
	ctx->onClick = NULL;
}

void uiEndRoot(UICtx *ctx) {
	if (ctx->onHover != NULL) {
		ctx->onHover(ctx->eventStateObject);
	}
	if (ctx->onClick != NULL) {
		ctx->onClick(ctx->eventStateObject);
	}

	if (ctx->layoutDepth != 1) {
		fprintf(stderr, "not enough end layouts");
		exit(1);
	}
}

void uiBeginLayout(UICtx *ctx, const UILayoutOptions *options) {
	ctx->layoutDepth++;
	if (ctx->layoutDepth >= MAX_LAYOUT_STACK) {
		fprintf(stderr, "num layouts exceeded");
		exit(1);
	}

	UILayout *layout = ctx->layoutStack + (ctx->layoutDepth - 1);
	UILayout *prevLayout = ctx->layoutStack + (ctx->layoutDepth - 2);

	layout->size = options->size;

	if (options->sizing & UI_FILL_WIDTH) {
		layout->size.x = prevLayout->size.x;
	}

	layout->position = prevLayout->cursorPos;
	layout->padding = options->padding;

	if (options->bgColor.a > 0) {
		layout->bgColor = options->bgColor;

		SDL_FRect background = {(float)layout->position.x, (float)layout->position.y,
			(float)layout->size.x, (float)layout->size.y};

		SDL_SetRenderDrawColor(ctx->renderer, layout->bgColor.r, layout->bgColor.g, layout->bgColor.b, layout->bgColor.a);
		SDL_RenderFillRect(ctx->renderer, &background);
	}
	layout->cursorPos = newVec2i(layout->position.x + layout->padding.t, layout->position.y + layout->padding.l);

	// handle events
	Input *input = ctx->input;
	ctx->eventStateObject = options->eventStateObject;

	if (collideABB(input->mouse.position, layout->position, layout->size)) {
		ctx->onHover = options->onHover;
		input->mouse.cursorIcon = CURSOR_POINTER;

		if (input->mouse.leftClick) {
			ctx->onClick = options->onClick;
		}
	}
}

void uiEndLayout(UICtx *ctx) {
	if (ctx->layoutDepth == 1) {
		fprintf(stderr, "too many end layouts");
		exit(1);
	}
	ctx->layoutDepth--;
}

// only margin and font size
void uiLabel(UICtx *ctx, const UILabelOptions *options) {
	if (options->cachedText == NULL) {
		return;
	}

	UILayout *layout = ctx->layoutStack + ctx->layoutDepth - 1;

	float width = options->cachedText->textLen * options->fontSize * 0.5f;

	SDL_FRect dest = {(float)layout->cursorPos.x, (float)layout->cursorPos.y, width, (float)options->fontSize};
	SDL_RenderTexture(ctx->renderer, options->cachedText->texture, NULL, &dest);

	layout->cursorPos.x += (i32)width;
	layout->cursorPos.y += options->fontSize;
}

void setUICachedText(UICachedText *cachedText, SDL_Renderer *renderer, TTF_Font *font, char *text, Color color) {
	u8 textLen = strlen(text);
	if (textLen == 0) return;
	if (textLen > MAX_TEXT_LEN) {
		fprintf(stderr, "");
	}

	// if identical, skip
	if (strncmp(text, cachedText->text, MAX_TEXT_LEN) == 0) {
		return;
	}

	cachedText->textLen = textLen;
	cachedText->texture = newTextTexture(renderer, text, font, color);
}
