#pragma once
#include "ctx.h"
#include "chip.h"
#include "ui.h"
#include "unit.h"
#include "camera.h"
#include "input.h"
#include "textures.h"
#include "editor/editor.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

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

	UICtx uiCtx;

	AppState state;

	// state structures
	Editor editor;

	u8 running;
} App;

void initApp(App *app);
void runApp(App *app);
