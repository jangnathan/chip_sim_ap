#include "app.h"
#include "simulate.h"
#include "input.h"
#include "event.h"
#include "editor/render.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

SDL_Window *createWindow() {
	SDL_Window *window = SDL_CreateWindow("ChipSim", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (!window) {
		fprintf(stderr, "failed to open window: %s", SDL_GetError());
		exit(1);
	}
	return window;
}

SDL_Renderer *createRenderer(SDL_Window *window) {
	SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		fprintf(stderr, "failed to create renderer: %s", SDL_GetError());
		exit(1);
	}
	return renderer;
}

void initCtx(Ctx *ctx) {
	chipsInit(&ctx->chips);
}

void initApp(App *app) {
	initCtx(&app->ctx);

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
	app->window = createWindow();
	app->renderer = createRenderer(app->window);

	Input *input = &app->input;
	initInput(input);

	app->uiCtx.renderer = app->renderer;
	app->uiCtx.input = input;
	app->uiCtx.font = app->font;
	initUICtx(&app->uiCtx);

	app->state = ST_EDIT;
	app->editor.ctx = &app->ctx;
	initEditor(&app->editor);
	initEditorUI(&app->editor, &app->uiCtx);

	loadTextures(app->renderer, &app->textures, app->font);
}

void update(App *app) {
	SDL_GetWindowSize(app->window, &app->winWidth, &app->winHeight);

	switch (app->state) {
		case ST_NONE:
			break;
		case ST_EDIT:
			app->editor.camera.viewportSize = newVec2i(app->winWidth, app->winHeight - app->editor.menubarHeight);
			app->editor.camera.viewportPos = newVec2i(0, app->editor.menubarHeight);

			updateEditor(&app->editor, &app->input, &app->ctx.chips);
			break;
	}
}

void render(App *app) {
	Ctx *ctx = &app->ctx;
	Chips *chips = &ctx->chips;
	SDL_Renderer *renderer = app->renderer;
	SDL_Window *window = app->window;
	UICtx *uiCtx = &app->uiCtx;

	SDL_SetRenderDrawColor(renderer, app->editor.bgColor.r,
												app->editor.bgColor.g,
												app->editor.bgColor.b, app->editor.bgColor.a);
	SDL_RenderClear(renderer);

	uiBeginRoot(uiCtx, app->winWidth, app->winHeight);

	switch (app->state) {
		case ST_NONE:
			break;
		case ST_EDIT:
			renderEditor(renderer, &app->textures, app->winWidth, &app->editor, chips);
			editorUI(uiCtx, &app->editor);
			break;
	}

	uiEndRoot(uiCtx);

	SDL_RenderPresent(renderer);
}

void closeApp(App *app) {
	closeInput(&app->input);
	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
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
