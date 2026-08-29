#include "ui.h"
#include <stdlib.h>
#include <string.h>

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font,
			    Color color) {
  SDL_Surface *surface;
  SDL_Color sdl_color = {color.r, color.g, color.b, color.a};
  surface = TTF_RenderText_Blended(font, text, 0, sdl_color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_DestroySurface(surface);

  return texture;
}

void generateIcons(UICtx *ctx) {
  SDL_Renderer *renderer = ctx->window->renderer;
  UIDefaultIcons *icons = &ctx->defaultIcons;

  // X Icon
  icons->x = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			       SDL_TEXTUREACCESS_TARGET, 32, 32);

  SDL_SetRenderTarget(renderer, icons->x);
  // clear texture to transparent
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  // draw cross
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderLine(renderer, 0, 1, 31, 32);
  for (i8 i = -3; i < 3; i++) {
    // y = mx + c
    // x = (y - c) / m
    SDL_RenderLine(renderer, 0, i, (32 - i), 32);
    SDL_RenderLine(renderer, 32, i, i, 32);
  }

  SDL_SetRenderTarget(renderer, NULL);
}

void initUICtx(UICtx *ctx) {
  if (ctx->window == NULL) {
    fprintf(stderr, "UICtx.window must be set before calling uiInitCtx()");
  }
  if (ctx->font == NULL) {
    fprintf(stderr, "UICtx.font must be set before calling uiInitCtx()");
  }
  if (ctx->input == NULL) {
    fprintf(stderr, "UICtx.input must be set before calling uiInitCtx()");
  }
  if (ctx->eventStateObject == NULL) {
    fprintf(stderr,
	    "UICtx.eventStateObject must be set before calling uiInitCtx()");
  }

  ctx->layoutDepth = 1;
  generateIcons(ctx);
}

void destroyUICtx(UICtx *ctx) {
  if (ctx->defaultIcons.x) {
    SDL_DestroyTexture(ctx->defaultIcons.x);
    ctx->defaultIcons.x = NULL;
  }
}

void uiBeginRoot(UICtx *ctx) {
  UILayout *container = ctx->layoutStack + 0;
  container->size = ctx->window->size;
  container->position = newVec2i(0, 0);
  container->padding = newVec4i(0, 0, 0, 0);
  container->orientation = UI_VERTICAL;
  container->cursorPos = newVec2i(0, 0);
  container->spacing = 0;
  ctx->layoutDepth = 1;

  ctx->onClick = NULL;
  ctx->onHover = NULL;
  ctx->onClickParams[0] = '\0';
  ctx->hoverParams[0] = '\0';

  ctx->isClickedPtr = NULL;
  ctx->isHoveredPtr = NULL;
}

void uiEndRoot(UICtx *ctx) {
  if (ctx->layoutDepth != 1) {
    fprintf(stderr, "not enough end layouts");
    exit(1);
  }
  if (ctx->onClick != NULL) {
    ctx->onClick(ctx->eventStateObject, ctx->onClickParams);
  }
  if (ctx->onHover != NULL) {
    ctx->onHover(ctx->eventStateObject, ctx->hoverParams);
  }

  if (ctx->isClickedPtr != NULL) {
    *ctx->isClickedPtr = 1;
  }
  if (ctx->isHoveredPtr != NULL) {
    *ctx->isHoveredPtr = 1;
  }
}

UILayout *uiRootLayout(UICtx *ctx) { return ctx->layoutStack + 0; }

void uiBeginLayout(UICtx *ctx, const UILayoutOptions *options) {
  if (ctx->layoutDepth >= MAX_LAYOUT_STACK) {
    fprintf(stderr, "num layouts exceeded");
    exit(1);
  }

  UILayout *layout = ctx->layoutStack + ctx->layoutDepth;
  UILayout *prevLayout = ctx->layoutStack + ctx->layoutDepth - 1;
  ctx->layoutDepth++;

  layout->size = options->size;
  if (options->sizing & UI_FILL_WIDTH) {
    layout->size.x =
	prevLayout->size.x - (prevLayout->cursorPos.x - prevLayout->position.x);
  }

  layout->position = prevLayout->cursorPos;
  layout->padding = options->padding;
  layout->orientation = options->orientation;
  layout->spacing = options->spacing;

  layout->hoverCursorIcon = options->hoverCursorIcon;

  // adjust cursor position

  layout->cursorPos = newVec2i(layout->position.x + layout->padding.l,
			       layout->position.y + layout->padding.t);

  switch (prevLayout->orientation) {
  case UI_HORIZONTAL:
    prevLayout->cursorPos.x += layout->size.x + prevLayout->spacing;
    break;

  case UI_VERTICAL:
    prevLayout->cursorPos.y += layout->size.y + prevLayout->spacing;
    break;
  default:
    break;
  }

  // render
  SDL_Renderer *renderer = ctx->window->renderer;
  if (options->bgColor.a > 0) {
    layout->bgColor = options->bgColor;

    SDL_FRect background = {(float)layout->position.x,
			    (float)layout->position.y, (float)layout->size.x,
			    (float)layout->size.y};

    SDL_SetRenderDrawColor(renderer, layout->bgColor.r, layout->bgColor.g,
			   layout->bgColor.b, layout->bgColor.a);
    SDL_RenderFillRect(renderer, &background);
  }

  // handle events
  Input *input = ctx->input;

  if (collideABB(input->mouse.position, layout->position, layout->size)) {
    input->mouse.cursorIcon = layout->hoverCursorIcon;

    ctx->isHoveredPtr = options->isHoveredPtr;

    if (options->onHover != NULL) {
      ctx->onHover = options->onHover;

      if (options->hoverParams != NULL) {
	memcpy(ctx->hoverParams, options->hoverParams,
	       sizeof(ctx->hoverParams));
      }
    }

    if (options->onClick != NULL) {
      if (input->mouse.leftClick) {
	ctx->onClick = options->onClick;
	ctx->isClickedPtr = options->isClickedPtr;

	if (options->onClickParams != NULL) {
	  memcpy(ctx->onClickParams, options->onClickParams,
		 sizeof(ctx->onClickParams));
	}
      }
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

UILayout *uiThisLayout(UICtx *ctx) {
  return ctx->layoutStack + ctx->layoutDepth - 1;
}

void uiMoveLayoutCursor(UICtx *ctx, i32 x, i32 y) {
  UILayout *layout = ctx->layoutStack + ctx->layoutDepth - 1;

  layout->cursorPos.x += x;
  layout->cursorPos.y += y;
}

void uiSetLayoutCursorPos(UICtx *ctx, i32 x, i32 y) {
  UILayout *layout = ctx->layoutStack + ctx->layoutDepth - 1;

  layout->cursorPos.x = x;
  layout->cursorPos.y = y;
}

void uiSetLayoutCursorPosX(UICtx *ctx, i32 x) {
  UILayout *layout = ctx->layoutStack + ctx->layoutDepth - 1;

  layout->cursorPos.x = x;
}

void uiResetLayoutCursorX(UICtx *ctx) {
  UILayout *layout = ctx->layoutStack + ctx->layoutDepth - 1;

  layout->cursorPos.x = layout->position.x + layout->padding.l;
}

// only margin and font size
void uiLabel(UICtx *ctx, const UILabelOptions *options) {
  if (options->cachedText == NULL) {
    return;
  }

  SDL_Renderer *renderer = ctx->window->renderer;
  UILayout *layout = ctx->layoutStack + ctx->layoutDepth - 1;

  u8 textureNeedsRefresh = 0;
  if (strncmp(options->cachedText->text, options->text, MAX_TEXT_LEN)) {
    textureNeedsRefresh = 1;
    strncpy(options->cachedText->text, options->text, MAX_TEXT_LEN);
  } else if (!equalColor(options->color, options->cachedText->color)) {
    textureNeedsRefresh = 1;
    options->cachedText->color = options->color;
  }

  if (textureNeedsRefresh == 1) {
    if (options->cachedText->texture != NULL) {
      SDL_DestroyTexture(options->cachedText->texture);
    }
    options->cachedText->texture = newTextTexture(
	ctx->window->renderer, options->text, ctx->font, options->color);
  }

  options->cachedText->textLen = strnlen(options->text, MAX_TEXT_LEN);
  float width = options->cachedText->textLen * options->fontSize * 0.5f;

  SDL_FRect dest = {(float)layout->cursorPos.x, (float)layout->cursorPos.y,
		    width, (float)options->fontSize};
  SDL_RenderTexture(renderer, options->cachedText->texture, NULL, &dest);

  switch (layout->orientation) {
  case UI_HORIZONTAL:
    layout->cursorPos.x += (i32)width + layout->spacing;
    break;
  case UI_VERTICAL:
    layout->cursorPos.y += options->fontSize + layout->spacing;
    break;
  default:
    break;
  }
}

void uiDecal(UICtx *ctx, const UIDecalOptions *options) {
  SDL_Renderer *renderer = ctx->window->renderer;
  UILayout *layout = ctx->layoutStack + ctx->layoutDepth - 1;

  SDL_FRect dest = {(float)layout->cursorPos.x, (float)layout->cursorPos.y,
		    (float)options->size.x, (float)options->size.y};
  SDL_RenderTexture(renderer, options->texture, NULL, &dest);

  switch (layout->orientation) {
  case UI_HORIZONTAL:
    layout->cursorPos.x += options->size.x + layout->spacing;
    break;
  case UI_VERTICAL:
    layout->cursorPos.y += options->size.y + layout->spacing;
    break;
  default:
    break;
  }
}

void uiTextShortInput(UICtx *ctx, const UITextShortInputOptions *options) {}