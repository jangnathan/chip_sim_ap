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
	SDL_Texture *stopSimulateTexture;
	SDL_Texture *startSimulateTexture;

	UIBox simulateButton;
	UIBox editChipBox;
	UIBox editChipMoveButton;
} UI;

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color);
UIBox newBox(Vec2 attachPosition, Vec2 size, SDL_Texture *texture, Color bgColor);
