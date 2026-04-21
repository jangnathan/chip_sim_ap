#pragma once
#include <SDL3/SDL.h>
#include "chip.h"
#include "unit.h"

typedef enum {
	MS_CHIP,
	MS_BUTTON
} MouseTarget;

typedef struct {
	Vec2 position;
	u8 isClick;
} Mouse;

typedef struct {
	Chips chips;
} Ctx;

typedef struct {
	Ctx ctx;
	SDL_Window *window;
	SDL_Renderer *renderer;

	Color bgColor;

	Mouse mouse;
	u8 running;
} App;

void initApp(App *app);
void update(App *app);
void closeApp(App *app);
void handleEvents(App *app);
