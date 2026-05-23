#pragma once
#include "ctx.h"
#include "chip.h"
#include "ui.h"
#include "editor/editor.h"
#include "unit.h"
#include "camera.h"
#include "input.h"

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

typedef enum {
	ST_NONE,
	ST_EDIT
} AppState;

typedef struct {
	Ctx ctx;
	SDL_Window *window;
	SDL_Renderer *renderer;

	Input input;

	Textures textures;
	TTF_Font *font;

	i32 winWidth;
	i32 winHeight;

	AppState state;

	// ui
	UI editorUI;

	// state structures
	Editor editor;

	u8 running;
} App;

void initApp(App *app);
void update(App *app);
void closeApp(App *app);
