#pragma once
#include "chip.h"
#include "ui.h"
#include "unit.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

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
	SDL_Texture *simpleAND;
	SDL_Texture *simpleOR;
	SDL_Texture *simpleNOT;
	SDL_Texture *simpleNAND;
	SDL_Texture *simpleNOR;
	SDL_Texture *simpleXOR;
	SDL_Texture *simpleXNOR;
} Textures;

typedef struct {
	Ctx ctx;
	SDL_Window *window;
	SDL_Renderer *renderer;

	UI ui;

	Textures textures;
	TTF_Font *font;
	Color bgColor;

	u16 menubarHeight;

	Mouse mouse;
	u8 running;
	u8 simulating;
} App;

void initApp(App *app);
void update(App *app);
void closeApp(App *app);
void handleEvents(App *app);
