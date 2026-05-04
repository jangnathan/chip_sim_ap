#include "app.h"
#include "simulate.h"

#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

// to account for camera, zoom, offset etc
Vec2 world2screenVec2(App *app, Vec2 a) {
	Vec2 out = scaleVec2(subtractVec2(a, app->camera.position), app->camera.zoom);
	out.y += app->menubarHeight;
	return out;
}

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

void loadTextures(App *app) {
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
	UI *ui = &app->ui;

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

	// init values
	app->simulating = 0;
	app->mouse.leftClick = 0;
	app->mouse.rightClick = 0;
	app->mouse.leftKeyHeld = 0;
	app->mouse.oldPosition = newVec2(0.0f, 0.0f);
	app->mouse.position = newVec2(0.0f, 0.0f);

	app->camera.zoom = 1.0f;
	app->camera.position = newVec2(0.0f, 0.0f);
	app->camera.oldPosition = newVec2(0.0f, 0.0f);

	app->bgColor = newColor(220, 220, 220, 0);
	app->menubarHeight = 80;

	app->mouse.cursorDefault =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	app->mouse.cursorMove =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
	SDL_SetCursor(app->mouse.cursorDefault);

	loadTextures(app);
	ui->stopSimulateTexture = newTextTexture(app->renderer, "Stop", app->font, newColor(0, 0, 0, 0));
	ui->startSimulateTexture = newTextTexture(app->renderer, "Simulate", app->font, newColor(0, 0, 0, 0));

	// Init ui
	UIButton simulateButton = {
		newVec2(10.0f, 15.0f),
		newVec2(120.0f, 50.0f),
		ui->startSimulateTexture,
		newColor(50, 150, 50, 0)
	};
	ui->simulateButton = simulateButton;
}

void updateSimpleChip(App *app, Vec2 pos, SimpleChip *simpleChip) {
}

void updateSwitch(App *app, Vec2 pos, InputChip *inputChip) {
	Vec2 switchSize = {50.0f, 120.0f};
	Vec2 mouseSize = {0.0f, 0.0f};
	if (app->mouse.leftClick) {
		if (collideAABB(pos, scaleVec2(switchSize, app->camera.zoom),
									app->mouse.position, mouseSize)) {
			inputChip->out = !inputChip->out;
		}
	}
}

void updateInputChip(App *app, Vec2 pos, InputChip *inputChip) {
	switch (inputChip->type) {
		case CLOCK:
			break;
		case SWITCH:
			updateSwitch(app, pos, inputChip);
			break;
	}
}

void updateChip(App *app, ChipEntity *chip) {
	Chips *chips = &app->ctx.chips;

	if (chip->parentID != 0) {
		chip->position = translateVec2(chips->array[chip->parentID].position, chip->attachPosition);
	}

	Vec2 pos = world2screenVec2(app, chip->position);

	switch (chip->type) {
		case CE_NONE:
			break;
		case CE_SIMPLE:
			updateSimpleChip(app, pos, chips->simpleChipsArray + chip->ID);
			break;
		case CE_INPUT:
			updateInputChip(app, pos, chips->inputChipsArray + chip->ID);
			break;
	}
}

void update(App *app) {
	UI *ui = &app->ui;
	Chips *chips = &app->ctx.chips;

	SDL_SetCursor(app->mouse.cursorDefault);
	const bool *keystates = SDL_GetKeyboardState(NULL);
	if (keystates[SDL_SCANCODE_SPACE]) {
		SDL_SetCursor(app->mouse.cursorMove);
		if (app->mouse.leftKeyHeld) {
			app->camera.position = translateVec2(app->camera.oldPosition,
																			 scaleVec2(subtractVec2(app->mouse.oldPosition, app->mouse.position), 1.0f / app->camera.zoom));
		}
	}

	if (app->mouse.leftClick && isHoveringButton(app->mouse.position, ui->simulateButton)) {
		app->simulating = !app->simulating;
		if (app->simulating) {
			ui->simulateButton.texture = ui->stopSimulateTexture;
			ui->simulateButton.bgColor = newColor(150, 50, 50, 0);
		} else {
			ui->simulateButton.texture = ui->startSimulateTexture;
			ui->simulateButton.bgColor = newColor(50, 150, 50, 0);
		}
	}

	for (u32 i = 0; i < chips->len; i++) {
		updateChip(app, chips->array + i);
	}

	if (app->simulating) {
		simulate(chips);
	}
}

void handleKeyPress(App *app, SDL_KeyboardEvent event) {
	switch (event.scancode) {
		case SDL_SCANCODE_MINUS:
			app->camera.zoom -= 0.1f;

			if (app->camera.zoom <= 0.1f) {
				app->camera.zoom = 0.1f;
			}
			break;
		case SDL_SCANCODE_EQUALS:
			app->camera.zoom += 0.1f;

			if (app->camera.zoom >= 3.0f) {
				app->camera.zoom = 3.0f;
			}
			break;
		default:
			break;
	}
}

void handleEvents(App *app) {
	app->mouse.leftClick = 0;
	app->mouse.rightClick = 0;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				app->running = 0;
				break;
			case SDL_EVENT_MOUSE_MOTION:
				app->mouse.position = newVec2(event.motion.x, event.motion.y);
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					if (!app->mouse.leftKeyHeld) {
						app->mouse.oldPosition = app->mouse.position;
						app->camera.oldPosition = app->camera.position;
					}
					app->mouse.leftKeyHeld = 1;
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				if (event.button.button == SDL_BUTTON_LEFT) {
					app->mouse.leftKeyHeld = 1;
					app->mouse.leftClick = 1;
				}
				if (event.button.button == SDL_BUTTON_RIGHT) {
					app->mouse.rightClick = 1;
				}
				app->mouse.leftKeyHeld = 0;
				break;
			case SDL_EVENT_KEY_DOWN:
				if (event.key.repeat) {
				} else {
					handleKeyPress(app, event.key);
				}
				break;
			default:
				break;
		}
	}
}

void closeApp(App *app) {
	SDL_DestroyCursor(app->mouse.cursorDefault);
	SDL_DestroyCursor(app->mouse.cursorMove);

	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
	SDL_Quit();
}
