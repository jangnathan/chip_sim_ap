#pragma once
#include "unit.h"
#include "input.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define MAX_LAYOUT_STACK 32

typedef enum {
	UI_ORIENT_NONE = 0,
	UI_HORIZONTAL = 1 << 1,
	UI_VERTICAL = 1 << 2,
} UIOrientation;

typedef enum {
	UI_SIZING_NONE = 0,
	UI_FILL_WIDTH = 1 << 1,
	UI_FILL_HEIGHT = 1 << 2,
} UISizing;

typedef enum {
	UI_ALIGN_NONE = 0,
	UI_ALIGN_LEFT = 1 << 1,
	UI_ALIGN_RIGHT = 1 << 2,
	UI_ALIGN_TOP = 1 << 3,
	UI_ALIGN_BOTTOM = 1 << 4,
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
	SDL_Renderer *renderer;
	Input *input;

	UILayout layoutStack[MAX_LAYOUT_STACK];
	u8 layoutDepth;

	void *eventStateObject;
	void (*onClick)(void *state);
	void (*onHover)(void *state);
} UICtx;

void uiInitCtx(UICtx *ctx, SDL_Renderer *renderer, Input *input);
void uiBeginRoot(UICtx *ctx, i32 winWidth, i32 winHeight);
void uiEndRoot(UICtx *ctx);

// layout stack push / pop
typedef struct {
	Vec2i position;
	Vec2i size;
	Vec4i padding;
	Color bgColor;
	UIOrientation orientation;
	UISizing sizing;
	UIAlignment alignment;

	void *eventStateObject;
	void (*onClick)(void *state);
	void (*onHover)(void *state);
} UILayoutOptions;

void uiBeginLayout(UICtx *ctx, const UILayoutOptions *options);
void uiEndLayout(UICtx *ctx);

typedef struct {
	SDL_Texture *texture;
	u16 textlen;
	u8 fontSize;
} UILabel;

// needs UILabel pointer so it doesnt need to redraw texture every frame
void uiLabel(UICtx *ctx, UILabel *label);

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color);
