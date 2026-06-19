#pragma once
#include "unit.h"
#include "core/input.h"
#include "core/window.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define MAX_LAYOUT_STACK 32

typedef enum {
	UI_VERTICAL,
	UI_HORIZONTAL,
} UIOrientation;

typedef enum {
	UI_SIZING_NONE = 0,
	UI_FILL_WIDTH = 1 << 1,
	UI_FILL_HEIGHT = 1 << 2,
} UISizing;

typedef enum { // by default is top left, cannot have both top and bottom or right and left
	UI_ALIGN_TOP = 0,
	UI_ALIGN_RIGHT = 1 << 1,
	UI_ALIGN_BOTTOM = 1 << 2,
	UI_ALIGN_LEFT = 0,
} UIAlignment;

typedef struct {
	Vec2i cursorPos;
	Vec2i position;
	Vec2i size;
	Vec4i padding;
	Color bgColor;

	UIOrientation orientation;
	u8 wrap;
} UILayout;


typedef struct {
	SDL_Texture *x;
} UIDefaultIcons;

typedef struct {
	tWindow *window;
	TTF_Font *font;
	Input *input;

	UIDefaultIcons defaultIcons;

	UILayout layoutStack[MAX_LAYOUT_STACK];
	u8 layoutDepth;

	void *eventStateObject;
	void (*onClick)(void *state);
	void (*onHover)(void *state);
} UICtx;

void initUICtx(UICtx *ctx, void *eventStateObject);
void uiBeginRoot(UICtx *ctx);
void uiEndRoot(UICtx *ctx);
UILayout *uiRootLayout(UICtx *ctx);

// layout stack push / pop
typedef struct {
	Vec2i size;
	Vec4i padding;
	Color bgColor;
	UIOrientation orientation;
	UISizing sizing;

	void (*onClick)(void *state);
	void (*onHover)(void *state);
} UILayoutOptions;

void uiBeginLayout(UICtx *ctx, const UILayoutOptions *options);
void uiEndLayout(UICtx *ctx);

UILayout *uiThisLayout(UICtx *ctx);
void uiMoveLayoutCursor(UICtx *ctx, i32 x, i32 y);
void uiSetLayoutCursorPos(UICtx *ctx, i32 x, i32 y);
void uiResetLayoutCursorX(UICtx *ctx);

// text / texture rendering

#define MAX_TEXT_LEN 128

typedef struct {
	char text[MAX_TEXT_LEN];
	SDL_Texture *texture;
	u8 textLen;
} UICachedText;

typedef struct {
	UICachedText *cachedText;
	u8 fontSize;
} UILabelOptions;

void uiLabel(UICtx *ctx, const UILabelOptions *options);

typedef struct {
	SDL_Texture *texture;
	Vec2i size;
} UIDecalOptions;
void uiDecal(UICtx *ctx, const UIDecalOptions *options);

void setUICachedText(UICachedText *cachedText, SDL_Renderer *renderer, TTF_Font *font, char *text, Color color);
SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color);