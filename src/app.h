#pragma once
#include "simulation/ctx.h"
#include "simulation/chip.h"
#include "ui.h"
#include "unit.h"
#include "core/camera.h"
#include "core/input.h"
#include "textures.h"
#include "editor/editor.h"
#include "core/window.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef enum {
	ST_NONE,
	ST_EDIT
} AppState;

typedef struct {
	tWindow window;
	Textures textures;
	TTF_Font *font;

	Input input;

	UICtx uiCtx;

	Ctx ctx;
	AppState state;

	// state structures
	Editor editor;

	u8 running;
} App;

void initApp(App *app);
void runApp(App *app);
