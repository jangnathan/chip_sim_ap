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

void updateSimulateButton(UI *ui, u8 state) {
	if (state) {
		ui->simulateButton.texture = ui->stopSimulateTexture;
		ui->simulateButton.bgColor = newColor(150, 50, 50, 0);
	} else {
		ui->simulateButton.texture = ui->startSimulateTexture;
		ui->simulateButton.bgColor = newColor(50, 150, 50, 0);
	}
}

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

	Input *input = &app->input;
	input->mouse.leftClick = 0;
	input->mouse.rightClick = 0;
	input->mouse.leftKeyHeld = 0;
	input->mouse.oldCenterPosition = newVec2(0.0f, 0.0f);
	input->mouse.position = newVec2(0.0f, 0.0f);

	app->camera.zoom = 1.0f;
	app->camera.position = newVec2(0.0f, 0.0f);
	app->camera.oldPosition = newVec2(0.0f, 0.0f);

	app->bgColor = newColor(220, 220, 220, 0);
	app->menubarHeight = 80;
	app->gridSize = 16;
	app->selectBoxActive = 0;

	input->mouse.cursorDefault =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	input->mouse.cursorMove =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
	SDL_SetCursor(input->mouse.cursorDefault);

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

	ui->activeTextInput = textInputNone;
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
	app->simulating = 0;
	updateSimulateButton(&app->ui, app->simulating);

	if (app->editState == EDIT_SELECT_OPTION) {
		app->editChipID = 0;
		app->editState = EDIT_NONE;
	} else {
		app->editChipID = ID;
		app->editState = EDIT_SELECT_OPTION;
	}
}
void chipEditClicked(App *app, u32 ID) {
	switch (app->editState) {
		case EDIT_FIND_LINK_CHIP: {
			u8 numOutputs = getNumOutputs(app->ctx.chips.array[ID].type);

			// open link edit menu
			if (numOutputs <= 1) {
				app->editState = EDIT_NONE;

				linkChipInsignal(&app->ctx.chips, app->editChipID, app->editChipInOption, ID, 0);
			} else {
				app->tempChipID = ID;
				app->tempChipNumOutputs = numOutputs;
				app->editState = EDIT_SELECT_OUT_LINK_CHIP;
			}
			break;
		}
		default:
			break;
	}
}

void updateSimpleChip(App *app, Vec2 pos, u32 ID, SimpleChip *simpleChip) {
	Input *input = &app->input;

	Vec2 hitboxSize = {50.0f, 50.0f}; 
	if (worldCollideABB(input->mouse.position, pos, scaleVec2(hitboxSize, app->camera.zoom)) && app->selectBoxActive == 0) {
		app->selectBoxActive = 1;
		app->selectBoxPos = pos;
		app->selectBoxSize = scaleVec2(hitboxSize, app->camera.zoom);

		if (input->mouse.rightClick) {
			openEditChip(app, ID);
		}

		if (input->mouse.leftClick) {
			if (app->simulating) {
			} else {
				chipEditClicked(app, ID);
			}
		}
	}
}

void updateSwitch(App *app, Vec2 pos, u32 ID, InputChip *inputChip) {
	Input *input = &app->input;

	Vec2 hitboxSize = {50.0f, 90.0f}; 
	if (worldCollideABB(input->mouse.position, pos, scaleVec2(hitboxSize, app->camera.zoom)) && app->selectBoxActive == 0) {

		app->selectBoxActive = 1;
		app->selectBoxPos = pos;
		app->selectBoxSize = scaleVec2(hitboxSize, app->camera.zoom);

		if (input->mouse.rightClick) {
			openEditChip(app, ID);
		}
		if (input->mouse.leftClick) {
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

	Vec2 pos = world2screenVec2(app->camera, chip->position);

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

void updateEditSelectLinkChip(App *app, u8 isInLink) {
	UI *ui = &app->ui;
	Input *input = &app->input;

	u8 numOptions = 0;
	if (isInLink) {
		numOptions = app->editChipNumInputs;
	} else {
		numOptions = app->tempChipNumOutputs;
	}
	for (u8 i = 0; i < numOptions; i++) {
		if (input->mouse.leftClick &&
			worldCollideABB(input->mouse.position, ui->selectLinkChipOption[i].position, ui->selectLinkChipOption[i].size)) {
			input->mouse.leftClick = 0;

			if (isInLink) {
				app->editChipInOption = i;
				app->editState = EDIT_FIND_LINK_CHIP;
			} else {
				linkChipInsignal(&app->ctx.chips, app->editChipID, app->editChipInOption, app->tempChipID, i);
				app->editState = EDIT_NONE;
			}
			break;
		}
	}
}

void updateUI(App *app) {
	UI *ui = &app->ui;
	Input *input = &app->input;

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

			if (input->mouse.leftClick
				&& collideABB(input->mouse.position, ui->editChipMoveButton.position, ui->editChipMoveButton.size)) {
				input->mouse.leftClick = 0;
				app->editState = EDIT_MOVE_CHIP;
			}
			if (input->mouse.leftClick
				&& collideABB(input->mouse.position, ui->editChipLinkButton.position, ui->editChipLinkButton.size)) {
				// set number of inputs
				u8 numInputs = getNumInputs(app->ctx.chips.array[app->editChipID].type);
				if (numInputs == 0) return;
				app->editChipNumInputs = numInputs;

				input->mouse.leftClick = 0;
				app->editState = EDIT_SELECT_IN_LINK_CHIP;
			}
			break;
		}
		case EDIT_MOVE_CHIP: {
			break;
		}
		case EDIT_SELECT_IN_LINK_CHIP: {
			updateEditSelectLinkChip(app, 1);
			break;
		}
		case EDIT_FIND_LINK_CHIP: {
			break;
		}
		case EDIT_SELECT_OUT_LINK_CHIP: {
			updateEditSelectLinkChip(app, 0);
			break;
		}
	}

	if (input->mouse.leftClick && collideABB(input->mouse.position, ui->simulateButton.position, ui->simulateButton.size)) {
		app->simulating = !app->simulating;
		if (app->simulating) {
			app->editState = EDIT_NONE;
		}
		updateSimulateButton(&app->ui, app->simulating);
	}

	// update text inputs
	for (u8 i = 0; i < textInputNone; i++) {
		if (input->mouse.leftClick && collideABB(input->mouse.position, ui->textInputs[i].position, ui->textInputs[i].size)) {
			ui->activeTextInput = i;
		}
	}
}

void update(App *app) {
	UI *ui = &app->ui;
	Chips *chips = &app->ctx.chips;
	Input *input = &app->input;

	SDL_SetCursor(input->mouse.cursorDefault);
	const bool *keystates = SDL_GetKeyboardState(NULL);
	u8 isMovingCamera = 0;

	app->selectBoxActive = 0;

	if (keystates[SDL_SCANCODE_SPACE]) {
		isMovingCamera = 1;
		SDL_SetCursor(input->mouse.cursorMove);
		if (input->mouse.leftKeyHeld) {
			app->camera.position = translateVec2(app->camera.oldPosition,
																				scaleVec2(subtractVec2(input->mouse.oldCenterPosition, input->mouse.centerPosition), 1.0f / app->camera.zoom));
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
			setUITextInputText(app->renderer, app->font, ui->textInputs + textInputPositionX, buf);
			snprintf(buf, UI_TEXT_INPUT_MAX, "%.2f", chips->array[app->editChipID].position.y);
			setUITextInputText(app->renderer, app->font, ui->textInputs + textInputPositionY, buf);

			// positioning logic
			Vec2 pos1 = translateVec2(app->camera.position, input->mouse.centerPosition);
			Vec2 pos = scaleVec2(pos1, 1.0f / app->camera.zoom);

			if (chips->array[app->editChipID].parentID != 0) {
				u32 parentID = chips->array[app->editChipID].parentID;
				chips->array[app->editChipID].attachPosition =
					subtractVec2(pos, chips->array[parentID].position);
			} else {
				chips->array[app->editChipID].position = pos;
			}
			if (input->mouse.leftClick) {
				input->mouse.leftClick = 0;
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
void closeApp(App *app) {
	closeInput(&app->input);

	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
	SDL_Quit();
}
