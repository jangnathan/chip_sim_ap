#pragma once
#include "constants.h"
#include "unit.h"

#include <SDL3/SDL.h>

typedef struct {
	SDL_Window *sdlWindow;
	SDL_Renderer *renderer;
	Vec2i size;
} tWindow;

void createWindow(tWindow *window, Vec2i size);
void updateWindow(tWindow *window);
void windowSetMinSize(tWindow *window, Vec2i size);

void closeWindow(tWindow *window);