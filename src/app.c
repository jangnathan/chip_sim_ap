#include "app.h"
#include "simulate.h"

#include <stdio.h>
#include <string.h>
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
	app->editState = EDIT_NONE;
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
	app->gridSize = 16;
	app->selectBoxActive = 0;

	app->mouse.cursorDefault =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	app->mouse.cursorMove =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
	SDL_SetCursor(app->mouse.cursorDefault);

	loadWorldTextures(app);

	// ui textures
	ui->stopSimulateTexture = newTextTexture(app->renderer, "Stop", app->font, newColor(0, 0, 0, 0));
	ui->startSimulateTexture = newTextTexture(app->renderer, "Simulate", app->font, newColor(0, 0, 0, 0));

	// Init ui
	UIBox simulateButton = newBox(
		newVec2(10.0f, 15.0f),
		newVec2(120.0f, 50.0f),
		ui->startSimulateTexture,
		newColor(50, 150, 50, 0));

	ui->simulateButton = simulateButton;

	app->editChipID = 0;
	app->editState = EDIT_NONE;
	ui->editChipBox = newBox(newVec2(0.0f, 0.0f), newVec2(120.0f, 350.0f), 0, newColor(225, 225, 225, 0));

	ui->editChipMoveButton = newBox(newVec2(20.0f, 20.0f), newVec2(80.0f, 30.0f),
																 newTextTexture(app->renderer, "Move", app->font, newColor(0, 0, 0, 0)), newColor(255, 255, 255, 0));
	ui->editChipLinkButton = newBox(newVec2(0.0f, 0.0f), newVec2(80.0f, 30.0f),
																 newTextTexture(app->renderer, "Link", app->font, newColor(0, 0, 0, 0)), newColor(255, 255, 255, 0));

	ui->textInputs[textInputPositionX] = newUITextInput();
	ui->textInputs[textInputPositionX].size = newVec2(70.0f, 25.0f);
	ui->textInputs[textInputPositionX].fontSize = 24;
	ui->textInputs[textInputPositionX].color = newColor(0,0,0,0);
	ui->textInputs[textInputPositionX].bgColor = newColor(255,255,255,0);

	ui->textInputs[textInputPositionY] = newUITextInput();
	ui->textInputs[textInputPositionY].size = newVec2(70.0f, 25.0f);
	ui->textInputs[textInputPositionY].fontSize = 24;
	ui->textInputs[textInputPositionY].color = newColor(0,0,0,0);
	ui->textInputs[textInputPositionY].bgColor = newColor(255,255,255,0);

	app->editChipNumInputs = 0;
	ui->selectLinkChipBox = newBox(newVec2(0.0f, 0.0f), newVec2(120.0f, 350.0f), 0, newColor(225, 225, 225, 0));
	for (u8 i = 0; i < 4; i++) {
		// 2 is num digits, incase you want to scale >10
		char buf[2];
		snprintf(buf, 2, "%d", i);

		ui->selectLinkChipOption[i] = newBox(newVec2(0.0f, 0.0f), newVec2(50.0f, 50.0f),
																			 newTextTexture(app->renderer, buf, app->font, newColor(0,0,0,0)), newColor(255, 225, 225, 0));
	}
}

void openEditChip(App *app, u32 ID) {
	if (app->editState == EDIT_SELECT_OPTION) {
		app->editChipID = 0;
		app->editState = EDIT_NONE;
	} else {
		app->editChipID = ID;
		app->editState = EDIT_SELECT_OPTION;
	}
}

void chipEditClicked(App *app, u32 ID) {
	if (app->editState == EDIT_FIND_LINK_CHIP) {
		// open link edit menu
		app->editState = EDIT_SELECT_OUT_LINK_CHIP;
	}
}

void updateSimpleChip(App *app, Vec2 pos, u32 ID, SimpleChip *simpleChip) {
	Vec2 size = {50.0f, 50.0f}; 
	if (collideABB(app->mouse.position, pos, scaleVec2(size, app->camera.zoom)) && app->selectBoxActive == 0) {
		app->selectBoxActive = 1;
		app->selectBoxPos = pos;
		app->selectBoxSize = scaleVec2(size, app->camera.zoom);

		if (app->mouse.rightClick) {
			openEditChip(app, ID);
		}

		if (app->mouse.leftClick) {
			if (app->simulating) {
			} else {
				chipEditClicked(app, ID);
			}
		}
	}
}

void updateSwitch(App *app, Vec2 pos, u32 ID, InputChip *inputChip) {
	Vec2 switchSize = {50.0f, 120.0f};
	Vec2 mouseSize = {0.0f, 0.0f};
	if (collideAABB(pos, scaleVec2(switchSize, app->camera.zoom),
								 app->mouse.position, mouseSize) && app->selectBoxActive == 0) {

		app->selectBoxActive = 1;
		app->selectBoxPos = pos;
		app->selectBoxSize = scaleVec2(switchSize, app->camera.zoom);

		if (app->mouse.rightClick) {
			openEditChip(app, ID);
		}
		if (app->mouse.leftClick) {
			if (app->simulating) {
				inputChip->out = !inputChip->out;
			} else {
				chipEditClicked(app, ID);
			}
		}
	}
}

void updateInputChip(App *app, Vec2 pos, u32 ID, InputChip *inputChip) {
	switch (inputChip->type) {
		case CLOCK:
			break;
		case SWITCH:
			updateSwitch(app, pos, ID, inputChip);
			break;
	}
}

void updateChip(App *app, u32 ID) {
	Chips *chips = &app->ctx.chips;
	ChipEntity *chip = chips->array + ID;

	if (chip->parentID != 0) {
		chip->position = translateVec2(chips->array[chip->parentID].position, chip->attachPosition);
	}

	Vec2 pos = world2screenVec2(app, chip->position);

	switch (chip->type) {
		case CE_NONE:
			break;
		case CE_SIMPLE:
			updateSimpleChip(app, pos, ID, chips->simpleChipsArray + chip->typeID);
			break;
		case CE_INPUT:
			updateInputChip(app, pos, ID, chips->inputChipsArray + chip->typeID);
			break;
	}
}


void setUITextInputText(App *app, UITextInput *textInput, char *str) {
	if (strcmp(textInput->text, str) == 0) {
		return;
	}
	strcpy(textInput->text, str);
	textInput->textLen = strlen(str);
	textInput->texture = newTextTexture(app->renderer, textInput->text, app->font, textInput->color);
}

void updateEditSelectInLinkChip(App *app) {
	UI *ui = &app->ui;
	for (u8 i = 0; i < app->editChipNumInputs; i++) {
		if (app->mouse.leftClick &&
			collideABB(app->mouse.position, ui->selectLinkChipOption[i].position, ui->selectLinkChipOption[i].size)) {
			app->mouse.leftClick = 0;
			app->editChipSelectedIn = i;

			app->editState = EDIT_FIND_LINK_CHIP;
			break;
		}
	}
}

void updateUI(App *app) {
	UI *ui = &app->ui;
		Vec2 selectLinkChipBoxPos = newVec2((app->winWidth - ui->selectLinkChipBox.size.x) / 2,
															 (app->winHeight + app->menubarHeight - ui->editChipBox.size.y) / 2);
	ui->selectLinkChipBox.position = selectLinkChipBoxPos;
	if (app->editState == EDIT_SELECT_IN_LINK_CHIP) {
		for (u8 i = 0; i < app->editChipNumInputs; i++) {
			ui->selectLinkChipOption[i].position = translateVec2(selectLinkChipBoxPos, newVec2(20.0f, i * 70.0f));
		}
	}

	// select modes
	switch (app->editState) {
		case EDIT_NONE:
			break;
		case EDIT_SELECT_OPTION: {
			// keep position according to window props
			Vec2 editChipBoxPos = newVec2(app->winWidth - ui->editChipBox.size.x - 20.0f,
																 (app->winHeight + app->menubarHeight - ui->editChipBox.size.y) / 2);
			ui->editChipBox.position = editChipBoxPos;
			ui->editChipMoveButton.position = translateVec2(editChipBoxPos, newVec2(20.0f, 20.0f));
			ui->editChipLinkButton.position = translateVec2(editChipBoxPos, newVec2(20.0f, 50.0f));

			if (app->mouse.leftClick
				&& collideABB(app->mouse.position, ui->editChipMoveButton.position, ui->editChipMoveButton.size)) {
				app->mouse.leftClick = 0;
				app->editState = EDIT_MOVE_CHIP;
			}
			if (app->mouse.leftClick
				&& collideABB(app->mouse.position, ui->editChipLinkButton.position, ui->editChipLinkButton.size)) {
				switch (app->ctx.chips.array[app->editChipID].type) {
					case CE_NONE:
						app->editChipNumInputs = 0;
						return;
					case CE_SIMPLE:
						// has only two inputs
						app->editChipNumInputs = 2;
						break;
					case CE_INPUT:
						app->editChipNumInputs = 0;
						return;
				}
				app->mouse.leftClick = 0;
				app->editState = EDIT_SELECT_IN_LINK_CHIP;
			}
			break;
		}
		case EDIT_MOVE_CHIP: {
			break;
		}
		case EDIT_SELECT_IN_LINK_CHIP: {
			updateEditSelectInLinkChip(app);
			break;
		}
		case EDIT_FIND_LINK_CHIP: {
			break;
		}
		case EDIT_SELECT_OUT_LINK_CHIP: {
			break;
		}
	}

	if (app->mouse.leftClick && collideABB(app->mouse.position, ui->simulateButton.position, ui->simulateButton.size)) {
		app->simulating = !app->simulating;
		if (app->simulating) {
			ui->simulateButton.texture = ui->stopSimulateTexture;
			ui->simulateButton.bgColor = newColor(150, 50, 50, 0);
		} else {
			ui->simulateButton.texture = ui->startSimulateTexture;
			ui->simulateButton.bgColor = newColor(50, 150, 50, 0);
		}
	}

	// update text inputs
	for (u8 i = 0; i < textInputNone; i++) {
		if (app->mouse.leftClick && collideABB(app->mouse.position, ui->textInputs[i].position, ui->textInputs[i].size)) {
			ui->activeTextInput = i;
		}
	}
}

void update(App *app) {
	UI *ui = &app->ui;
	Chips *chips = &app->ctx.chips;

	SDL_SetCursor(app->mouse.cursorDefault);
	const bool *keystates = SDL_GetKeyboardState(NULL);
	u8 isMovingCamera = 0;

	app->selectBoxActive = 0;

	if (keystates[SDL_SCANCODE_SPACE]) {
		isMovingCamera = 1;
		SDL_SetCursor(app->mouse.cursorMove);
		if (app->mouse.leftKeyHeld) {
			app->camera.position = translateVec2(app->camera.oldPosition,
																				scaleVec2(subtractVec2(app->mouse.oldPosition, app->mouse.position), 1.0f / app->camera.zoom));
		}
	}

	updateUI(app);

	if (isMovingCamera == 0) {
		if (app->editState == EDIT_MOVE_CHIP) {
			// position the position inputs
			ui->textInputs[textInputPositionX].position = newVec2(app->winWidth - ui->textInputs[textInputPositionX].size.x - 80.0f,
																												 app->winHeight / 2 - 140.0f);
			ui->textInputs[textInputPositionY].position = newVec2(app->winWidth - ui->textInputs[textInputPositionX].size.x - 80.0f,
																												 app->winHeight / 2 - 110.0f);

			char buf[UI_TEXT_INPUT_MAX];
			snprintf(buf, UI_TEXT_INPUT_MAX, "%.2f", chips->array[app->editChipID].position.x);
			setUITextInputText(app, ui->textInputs + textInputPositionX, buf);
			snprintf(buf, UI_TEXT_INPUT_MAX, "%.2f", chips->array[app->editChipID].position.y);
			setUITextInputText(app, ui->textInputs + textInputPositionY, buf);

			// positioning logic
			Vec2 pos1 = translateVec2(app->camera.position, app->mouse.position);
			pos1.y -= app->menubarHeight;
			Vec2 pos = scaleVec2(pos1, 1.0f / app->camera.zoom);

			if (chips->array[app->editChipID].parentID != 0) {
				u32 parentID = chips->array[app->editChipID].parentID;
				chips->array[app->editChipID].attachPosition =
					subtractVec2(pos, chips->array[parentID].position);
			} else {
				chips->array[app->editChipID].position = pos;
			}
			if (app->mouse.leftClick) {
				app->mouse.leftClick = 0;
				app->editState = EDIT_NONE;
			}
		}
	}

	for (u32 i = 0; i < chips->len; i++) {
		updateChip(app, i);
	}

	if (app->simulating) {
		simulate(chips);
	}
}

void handleKeyPress(App *app, SDL_KeyboardEvent event) {
	if (app->ui.activeTextInput < textInputNone) {
		UITextInput *textInput = app->ui.textInputs + (u32)app->ui.activeTextInput;

		textInput->text[textInput->textLen] = '\0'; // null terminate

		// convert keycode to add text to the active text input

		setUITextInputText(app, textInput, "hello");
		return;
	}

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
