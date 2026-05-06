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
	Vec2 oldPosition;
	Vec2 position;
	float zoom;
} Camera;

typedef struct {
	SDL_Cursor *cursorDefault;
	SDL_Cursor *cursorMove;
	Vec2 oldPosition;
	Vec2 position;
	u8 leftClick;
	u8 rightClick;
	u8 leftKeyHeld;
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

typedef enum {
	EDIT_NONE,
	EDIT_SELECT_OPTION,
	EDIT_MOVE_CHIP
} EditState;

typedef struct {
	Ctx ctx;
	SDL_Window *window;
	SDL_Renderer *renderer;

	Textures textures;
	TTF_Font *font;
	Color bgColor;

	EditState editState;

	i32 winWidth;
	i32 winHeight;

	u16 menubarHeight;
	u16 gridSize;
	UI ui;

	u32 editChipID;

	Camera camera;

	Mouse mouse;
	u8 running;
	u8 simulating;
} App;

void initApp(App *app);
void update(App *app);
void closeApp(App *app);
void handleEvents(App *app);
