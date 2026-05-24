#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
	SDL_Texture *simpleAND;
	SDL_Texture *simpleOR;
	SDL_Texture *simpleNOT;
	SDL_Texture *simpleNAND;
	SDL_Texture *simpleNOR;
	SDL_Texture *simpleXOR;
	SDL_Texture *simpleXNOR;
} Textures;

void loadTextures(SDL_Renderer *renderer, Textures *textures, TTF_Font *font);
