#include "app.h"

#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

SDL_Window *createWindow() {
	SDL_Window *window = SDL_CreateWindow("ChipSim", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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

void loadTextures(App *app) {
	SDL_Renderer *renderer = app->renderer;
	Textures *textures = &app->textures;
	SDL_Color White = {255, 255, 255};

	// AND,OR,NOT,NAND,NOR,XOR,XNOR
	SDL_Surface* surface;
	surface = TTF_RenderText_Blended(app->font, "AND", 0, White);
	textures->simpleAND = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	surface = TTF_RenderText_Blended(app->font, "OR", 0, White);
	textures->simpleOR = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	surface = TTF_RenderText_Blended(app->font, "NOT", 0, White);
	textures->simpleNOT = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	surface = TTF_RenderText_Blended(app->font, "NAND", 0, White);
	textures->simpleNAND = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	surface = TTF_RenderText_Blended(app->font, "NOR", 0, White);
	textures->simpleNOR = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	surface = TTF_RenderText_Blended(app->font, "XOR", 0, White);
	textures->simpleXOR = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	surface = TTF_RenderText_Blended(app->font, "XNOR", 0, White);
	textures->simpleXNOR = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);
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

	app->mouse.isClick = 0;
	app->running = 1;
	app->bgColor = newColor(220, 220, 220, 0);
	app->window = createWindow();
	app->renderer = createRenderer(app->window);

	loadTextures(app);
}

void updateSimpleChip(App *app, ChipEntity *chip, SimpleChip *simpleChip) {
}

void updateSwitch(App *app, ChipEntity *chip, InputChip *inputChip) {
	Vec2 switchSize = {50.0f, 100.0f};
	Vec2 mouseSize = {0.0f, 0.0f};
	if (app->mouse.isClick) {
		if (collideAABB(chip->position, switchSize, app->mouse.position, mouseSize)) {
			inputChip->out = !inputChip->out;
		}
	}
}

void updateInputChip(App *app, ChipEntity *chip, InputChip *inputChip) {
	switch (inputChip->type) {
		case CLOCK:
			break;
		case SWITCH:
			updateSwitch(app, chip, inputChip);
			break;
	}
}

void updateChip(App *app, ChipEntity *chip) {
	Chips *chips = &app->ctx.chips;
	switch (chip->type) {
		case CE_SIMPLE:
			updateSimpleChip(app, chip, chips->simpleChipsArray + chip->ID);
			break;
		case CE_INPUT:
			updateInputChip(app, chip, chips->inputChipsArray + chip->ID);
			break;
	}
}

void update(App *app) {
	Chips *chips = &app->ctx.chips;

	SDL_GetMouseState(&app->mouse.position.x, &app->mouse.position.y);
	for (u32 i = 0; i < chips->len; i++) {
		updateChip(app, chips->array + i);
	}
}

void handleEvents(App *app) {
	if (app->mouse.isClick) {
		app->mouse.isClick = 0;
	}

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				app->running = 0;
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					app->mouse.isClick = 1;
				}
				break;
			default:
				break;
		}
	}
}

void closeApp(App *app) {
	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
	SDL_Quit();
}
