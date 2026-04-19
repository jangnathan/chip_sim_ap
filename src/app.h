#pragma once
#include <SDL3/SDL.h>
#include "chip.h"
#include "unit.h"

typedef struct {
	Chips chips;
} Ctx;

typedef struct {
	Ctx ctx;
	SDL_Window *window;
	SDL_Renderer *renderer;

	Color bgColor;
} App;

void initApp(App *app);
void closeApp(App *app);
