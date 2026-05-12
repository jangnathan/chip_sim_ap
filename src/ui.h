#pragma once
#include "unit.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
	Vec2 position;
	Vec2 size;
	SDL_Texture *texture;
	Color bgColor;
} UIBox;

#define UI_TEXT_INPUT_MAX 32
typedef struct {
	char text[UI_TEXT_INPUT_MAX];
	u8 textLen;

	Vec2 position;
	Vec2 size;
	SDL_Texture *texture;
	Color color;
	Color bgColor;
	u8 fontSize;
} UITextInput;


#define NUM_TEXT_INPUTS 2
typedef enum {
	textInputPositionX,
	textInputPositionY,
	textInputNone
} TextInputID;

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color);
UITextInput newUITextInput();

UIBox newBox(Vec2 pos, Vec2 size, SDL_Texture *texture, Color bgColor);
