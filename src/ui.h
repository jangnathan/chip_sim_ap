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

typedef struct {
	char text[32];
	u8 textLen;

	Vec2 position;
	Vec2 size;
	Color color;
	u8 fontSize;

	SDL_Texture *texture;
} UITextInput;

typedef struct {
	SDL_Texture *stopSimulateTexture;
	SDL_Texture *startSimulateTexture;

	UIBox simulateButton;
	UIBox editChipBox;
	UIBox editChipMoveButton;

	UITextInput inputPositionX;
	UITextInput inputPositionY;
} UI;

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color);
UITextInput newUITextInput(Vec2 pos, Vec2 size, u8 fontSize, Color color);

UIBox newBox(Vec2 pos, Vec2 size, SDL_Texture *texture, Color bgColor);
