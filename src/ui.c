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

void initUICtx(UICtx *ctx, void *eventStateObject) {
  if (ctx->window == NULL) {
    fprintf(stderr, "UICtx.window must be set before calling uiInitCtx()");
  }
  if (ctx->font == NULL) {
    fprintf(stderr, "UICtx.font must be set before calling uiInitCtx()");
  }
  if (ctx->input == NULL) {
    fprintf(stderr, "UICtx.input must be set before calling uiInitCtx()");
  }
  ctx->layoutDepth = 1;
  ctx->eventStateObject = eventStateObject;
}

void uiBeginRoot(UICtx *ctx) {
  UILayout *container = ctx->layoutStack + 0;
  container->size = ctx->window->size;
  container->position = newVec2i(0, 0);
  container->padding = newVec4i(0, 0, 0, 0);
  container->orientation = UI_VERTICAL;
  container->cursorPos = newVec2i(0, 0);
  ctx->layoutDepth = 1;

  ctx->onHover = NULL;
  ctx->onClick = NULL;
}

void uiEndRoot(UICtx *ctx) {
  if (ctx->onHover != NULL) {
    ctx->onHover(ctx->eventStateObject);
  }
  if (ctx->onClick != NULL) {
    printf("Hello2\n");
    ctx->onClick(ctx->eventStateObject);
  }

  if (ctx->layoutDepth != 1) {
    fprintf(stderr, "not enough end layouts");
    exit(1);
  }
}

UILayout *uiRootLayout(UICtx *ctx) {
  return ctx->layoutStack + 0;
}

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
    layout->size.x = prevLayout->size.x - (prevLayout->cursorPos.x - prevLayout->position.x);
  }

  layout->position = prevLayout->cursorPos;
  layout->padding = options->padding;
  layout->orientation = options->orientation;

  // adjust cursor position

  layout->cursorPos = newVec2i(layout->position.x + layout->padding.t,
                               layout->position.y + layout->padding.l);

  switch (prevLayout->orientation) {
  case UI_HORIZONTAL:
    prevLayout->cursorPos.x += layout->size.x;
    break;

  case UI_VERTICAL:
    prevLayout->cursorPos.y += layout->size.y;
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
    ctx->onHover = options->onHover;

    if (options->onClick != NULL) {
      input->mouse.cursorIcon = CURSOR_POINTER;

      if (input->mouse.leftClick) {
        ctx->onClick = options->onClick;
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

  float width = options->cachedText->textLen * options->fontSize * 0.5f;

  SDL_FRect dest = {(float)layout->cursorPos.x, (float)layout->cursorPos.y,
                    width, (float)options->fontSize};
  SDL_RenderTexture(renderer, options->cachedText->texture, NULL, &dest);

  switch (layout->orientation) {
  case UI_HORIZONTAL:
    layout->cursorPos.x += (i32)width;
    break;
  case UI_VERTICAL:
    layout->cursorPos.y += options->fontSize;
    break;
  default:
    break;
  }
}

void setUICachedText(UICachedText *cachedText, SDL_Renderer *renderer,
                     TTF_Font *font, char *text, Color color) {
  u8 textLen = strlen(text);
  if (textLen == 0)
    return;
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
