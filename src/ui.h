#pragma once
#include "unit.h"
#include "input.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

//typedef struct {
//	Vec2i position;
//	Vec2i attachPosition;
//	Vec2i size;
//	SDL_Texture *texture;
//	Color bgColor;
//} UIBox;
//
//#define UI_TEXT_INPUT_MAX 32
//typedef struct {
//	char text[UI_TEXT_INPUT_MAX];
//	u8 textLen;
//
//	Vec2i position;
//	Vec2i attachPosition;
//	Vec2i size;
//	SDL_Texture *texture;
//	Color color;
//	Color bgColor;
//	u8 fontSize;
//} UITextInput;

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
	Vec2i position;
	Vec2i size;
	Vec4i container;
	Color bgColor;
	UIOrientation orientation;
} UILayout;

typedef struct {
	SDL_Renderer *renderer;

	UILayout layoutStack[MAX_LAYOUT_STACK];
	u8 layoutDepth;
} UICtx;

/*
#define NUM_TEXT_INPUTS 2
typedef enum {
	textInputPositionX,
	textInputPositionY,
	textInputNone
} TextInputID;
*/

void uiInitCtx(UICtx *ctx, SDL_Renderer *renderer);

typedef struct {
	Vec2i position;
	Vec2i size;
	Vec4i padding;
	Color bgColor;
	UIOrientation orientation;
	UISizing sizing;
	UIAlignment alignment;
} UILayoutOptions;

void uiRoot(UICtx *ctx, i32 winWidth, i32 winHeight);
void uiBeginLayout(UICtx *ctx, const UILayoutOptions *options);
void uiEndLayout(UICtx *ctx);

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color);
