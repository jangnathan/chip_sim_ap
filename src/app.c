#include "app.h"
#include "simulate.h"
#include "input.h"

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

void loadWorldTextures(App *app) {
	SDL_Renderer *renderer = app->renderer;
	Textures *textures = &app->textures;
	Color White = {255, 255, 255, 0};

	// AND,OR,NOT,NAND,NOR,XOR,XNOR
	textures->simpleAND = newTextTexture(renderer, "AND", app->font, White);
	textures->simpleOR = newTextTexture(renderer, "OR", app->font, White);
	textures->simpleNOT = newTextTexture(renderer, "NOT", app->font, White);
	textures->simpleNAND = newTextTexture(renderer, "NAND", app->font, White);
	textures->simpleNOR = newTextTexture(renderer, "NOR", app->font, White);
	textures->simpleXOR = newTextTexture(renderer, "XOR", app->font, White);
	textures->simpleXNOR = newTextTexture(renderer, "XNOR", app->font, White);
};

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

	app->state = ST_EDIT;

	app->editor.ctx = &app->ctx;
	initEditor(&app->editor);
	createEditorUI(&app->editorUI);

	input->mouse.cursorDefault =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	input->mouse.cursorMove =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
	SDL_SetCursor(input->mouse.cursorDefault);

	loadWorldTextures(app);
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

void runApp(App *app) {
}

void closeApp(App *app) {
	closeInput(&app->input);

	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
	SDL_Quit();
}
