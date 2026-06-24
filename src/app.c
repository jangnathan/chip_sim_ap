#include "app.h"
#include "simulation/simulate.h"
#include "simulation/circuit.h"
#include "core/input.h"
#include "core/event.h"
#include "editor/render.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

void initCtx(Ctx *ctx) {
	circuitInit(&ctx->circuit);
}

void initApp(App *app) {
	u8 sucess = SDL_Init(SDL_INIT_VIDEO);
	if (!sucess) {
		fprintf(stderr, "error initializing SDL3");
		exit(1);
	}

	if (!TTF_Init()) {
		fprintf(stderr, "error initializing SDL3_ttf");
		exit(1);
	}

	app->font = TTF_OpenFont("asset/ARIAL.ttf", 16);
	if (!app->font) {
		fprintf(stderr, "error loading font");
		exit(1);
	}

	app->running = 1;
	createWindow(&app->window, newVec2i(WINDOW_WIDTH, WINDOW_HEIGHT));
	windowSetMinSize(&app->window, newVec2i(500, 300));

	Input *input = &app->input;
	initInput(input);

	app->uiCtx.window = &app->window;
	app->uiCtx.input = input;
	app->uiCtx.font = app->font;
	initUICtx(&app->uiCtx, app);

	app->state = ST_EDIT;
	app->editor.ctx = &app->ctx;
	app->editor.uiCtx = &app->uiCtx;
	initEditor(&app->editor);

	loadTextures(app->window.renderer, &app->textures, app->font);

	// init logic Ctx
	initCtx(&app->ctx);
	//app->ctx.circuit.len = 0;
}

void update(App *app) {
	updateWindow(&app->window);

	switch (app->state) {
		case ST_NONE:
			break;
		case ST_EDIT:
			Vec2i viewportSize = app->window.size;
			viewportSize.y -= app->editor.menubarHeight;
			app->editor.camera.viewportSize = viewportSize;
			app->editor.camera.viewportPos = newVec2i(0, app->editor.menubarHeight);

			updateEditor(&app->editor, &app->input);
			break;
	}
}

void render(App *app) {
	Ctx *ctx = &app->ctx;
	SDL_Renderer *renderer = app->window.renderer;
	UICtx *uiCtx = &app->uiCtx;

	SDL_SetRenderDrawColor(renderer, app->editor.bgColor.r,
												app->editor.bgColor.g,
												app->editor.bgColor.b, app->editor.bgColor.a);
	SDL_RenderClear(renderer);

	uiBeginRoot(uiCtx);

	switch (app->state) {
		case ST_NONE:
			break;
		case ST_EDIT:
			renderEditor(renderer, &app->textures, &app->editor);
			editorUI(uiCtx, &app->editor);
			break;
	}

	uiEndRoot(uiCtx);

	SDL_RenderPresent(renderer);
}

void closeApp(App *app) {
	closeInput(&app->input);
	closeWindow(&app->window);
	circuitFree(&app->ctx.circuit);
	SDL_Quit();
}

void runApp(App *app) {
	app->running = 1;

	while (app->running) {
		handleEvents(app);
		update(app);
		render(app);
		SDL_Delay(33);
	}

	closeApp(app);
}
