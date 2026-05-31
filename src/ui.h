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
	UI_HORIZONTAL,
	UI_VERTICAL,
} UIOrientation;

typedef enum {
	UI_FILL_WIDTH,
	UI_FILL_HEIGHT,
} UISizing;

typedef enum {
	UI_ALIGN_LEFT,
	UI_ALIGN_RIGHT,
	UI_ALIGN_TOP,
	UI_ALIGN_BOTTOM,
} UIAlignment;

typedef struct {
	Vec2i position;
	Vec2i size;
	Rect2i container;
	Color bgColor;
	UIOrientation orientation;
} UILayout;

typedef struct {
	SDL_Renderer *renderer;
	Vec2i windowSize;

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
	Rect2i padding;
	Color bgColor;
	UIOrientation orientation;
	UISizing sizing;
	UIAlignment alignment;
} UILayoutOptions;

void uiBeginLayout(UICtx *ctx, const UILayoutOptions *options);
void uiEndLayout(UICtx *ctx);

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color);
