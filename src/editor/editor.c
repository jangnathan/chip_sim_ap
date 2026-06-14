#include "editor.h"
#include "simulate.h"

#include <stdio.h>

void editorZoomOut(Editor *editor) {
	editor->camera.zoom += 0.1f;

	if (editor->camera.zoom >= 3.0f) {
		editor->camera.zoom = 3.0f;
	}
}
void editorZoomIn(Editor *editor) {
	editor->camera.zoom -= 0.1f;

	if (editor->camera.zoom <= 0.1f) {
		editor->camera.zoom = 0.1f;
	}
}

void editorHandleKeypress(Editor *editor, SDL_KeyboardEvent event) {
	SDL_Scancode code = event.scancode;
	if (code == editor->zoomInKey) {
			editorZoomIn(editor);
	} else if (code == editor->zoomOutKey) {
			editorZoomOut(editor);
	}
}

void openEditChip(Editor *editor, u32 ID) {
	editor->simulating = 0;
	//updateSimulateButton(&editor->ui, app->simulating);

	if (editor->state == EDIT_SELECT_OPTION) {
		editor->editChipID = 0;
		editor->state = EDIT_NONE;
	} else {
		editor->editChipID = ID;
		editor->state = EDIT_SELECT_OPTION;
	}
}
void chipEditClicked(Editor *editor, Chips *chips, u32 ID) {
	switch (editor->state) {
		case EDIT_FIND_LINK_CHIP: {
			u8 numOutputs = getNumOutputs(chips->array[ID].type);

			// open link edit menu
			if (numOutputs <= 1) {
				editor->state = EDIT_NONE;

				linkChipInsignal(chips, editor->editChipID, editor->editChipInOption, ID, 0);
			} else {
				editor->tempChipID = ID;
				editor->tempChipNumOutputs = numOutputs;
				editor->state = EDIT_SELECT_OUT_LINK_CHIP;
			}
			break;
		}
		default:
			break;
	}
}

void updateSimpleChip(Editor *editor, Input *input, Chips *chips, Vec2i pos, u32 ID) {
	Vec2f hitboxSize = {50, 50}; 

	if (worldCollideABB(vec2ItoF(input->mouse.position), vec2ItoF(pos), scaleVec2f(hitboxSize, editor->camera.zoom)) && editor->selectBoxActive == 0) {
		editor->selectBoxActive = 1;
		editor->selectBoxPos = pos;
		editor->selectBoxSize = scaleVec2i(newVec2i(hitboxSize.x, hitboxSize.y), editor->camera.zoom);

		if (input->mouse.rightClick) {
			openEditChip(editor, ID);
		}

		if (input->mouse.leftClick) {
			if (editor->simulating) {
			} else {
				chipEditClicked(editor, chips, ID);
			}
		}
	}
}
void updateSwitch(Editor *editor, Input *input, Chips *chips, Vec2i pos, u32 ID, InputChip *inputChip) {
	Vec2f hitboxSize = {50, 90}; 

	if (worldCollideABB(vec2ItoF(input->mouse.position), vec2ItoF(pos), scaleVec2f(hitboxSize, editor->camera.zoom)) && editor->selectBoxActive == 0) {
		editor->selectBoxActive = 1;
		editor->selectBoxPos = pos;
		editor->selectBoxSize = scaleVec2i(newVec2i(hitboxSize.x, hitboxSize.y), editor->camera.zoom);

		if (input->mouse.rightClick) {
			openEditChip(editor, ID);
		}
		if (input->mouse.leftClick) {
			if (editor->simulating) {
				inputChip->out = !inputChip->out;
			} else {
				chipEditClicked(editor, chips, ID);
			}
		}
	}
}

void updateInputChip(Editor *editor, Input *input, Chips *chips, Vec2i pos, u32 ID, InputChip *inputChip) {
	switch (inputChip->type) {
		case CLOCK:
			break;
		case SWITCH:
			updateSwitch(editor, input, chips, pos, ID, inputChip);
			break;
	}
}

void updateChip(Editor *editor, Input *input, Chips *chips, u32 ID) {
	ChipEntity *chip = chips->array + ID;

	if (chip->parentID != 0) {
		chip->position = translateVec2f(chips->array[chip->parentID].position, chip->attachPosition);
	}

	Vec2i pos = world2screenVec2i(editor->camera, chip->position);

	switch (chip->type) {
		case CE_NONE:
			break;
		case CE_SIMPLE:
			updateSimpleChip(editor, input, chips, pos, ID);
			break;
		case CE_INPUT:
			updateInputChip(editor, input, chips, pos, ID, chips->inputChipsArray + chips->array[ID].typeID);
			break;
	}
}

void updateEditSelectLinkChip(Editor *editor, Input *input, Chips *chips, u8 isInLink) {
	u8 numOptions = 0;
	if (isInLink) {
		numOptions = editor->editChipNumInputs;
	} else {
		numOptions = editor->tempChipNumOutputs;
	}
	for (u8 i = 0; i < numOptions; i++) {
		/* OLD CODE
		if (input->mouse.leftClick &&
			collideABB(input->mouse.position, ui->selectLinkChipOption[i].position, ui->selectLinkChipOption[i].size)) {
			input->mouse.leftClick = 0;

			if (isInLink) {
				editor->editChipInOption = i;
				editor->state = EDIT_FIND_LINK_CHIP;
			} else {
				linkChipInsignal(chips, editor->editChipID, editor->editChipInOption, editor->tempChipID, i);
				editor->state = EDIT_NONE;
			}
			break;
		}
		*/
	}
}

void updateEditor(Editor *editor, Input *input, Chips *chips) {
	const bool *keystates = SDL_GetKeyboardState(NULL);
	u8 isMovingCamera = 0;

	editor->selectBoxActive = 0;

	if (keystates[SDL_SCANCODE_SPACE]) {
		isMovingCamera = 1;
		input->mouse.cursorIcon =  CURSOR_MOVE;

		if (input->mouse.leftDown) {
			editor->camera.oldPosition = editor->camera.position;
		}

		if (input->mouse.leftHeld) {
			editor->camera.position = translateVec2f(editor->camera.oldPosition,
																							scaleVec2f(vec2ItoF(subtractVec2i(input->mouse.oldCenterPosition, input->mouse.centerPosition)),
																	1.0f / editor->camera.zoom));
		}
	}

	if (isMovingCamera == 0) {
		if (editor->state == EDIT_MOVE_CHIP) {
			// position the position inputs
			/* OLD CODE
			ui->textInputs[textInputPositionX].position = newVec2f(app->winWidth - ui->textInputs[textInputPositionX].size.x - 80.0f,
																													app->winHeight / 2 - 140.0f);
			ui->textInputs[textInputPositionY].position = newVec2f(app->winWidth - ui->textInputs[textInputPositionX].size.x - 80.0f,
																													app->winHeight / 2 - 110.0f);

			char buf[UI_TEXT_INPUT_MAX];
			snprintf(buf, UI_TEXT_INPUT_MAX, "%.2f", chips->array[app->editChipID].position.x);
			setUITextInputText(app->renderer, app->font, ui->textInputs + textInputPositionX, buf);
			snprintf(buf, UI_TEXT_INPUT_MAX, "%.2f", chips->array[app->editChipID].position.y);
			setUITextInputText(app->renderer, app->font, ui->textInputs + textInputPositionY, buf);
			*/

			// positioning logic
			Vec2f pos1 = translateVec2f(editor->camera.position, vec2ItoF(input->mouse.centerPosition));
			Vec2f pos = scaleVec2f(pos1, 1.0f / editor->camera.zoom);

			if (chips->array[editor->editChipID].parentID != 0) {
				u32 parentID = chips->array[editor->editChipID].parentID;
				chips->array[editor->editChipID].attachPosition =
					subtractVec2f(pos, chips->array[parentID].position);
			} else {
				chips->array[editor->editChipID].position = pos;
			}
			if (input->mouse.leftClick) {
				input->mouse.leftClick = 0;
				editor->state = EDIT_NONE;
			}
		}
	}

	for (u32 i = 1; i < chips->len; i++) {
		updateChip(editor, input, chips, i);
	}

	if (editor->simulating) {
		simulate(chips);
	}
}

void initEditor(Editor *editor) {
	editor->state = EDIT_NONE;
	editor->simulating = 0;

	editor->camera.zoom = 1.0f;
	editor->camera.position = newVec2f(0.0f, 0.0f);
	editor->camera.oldPosition = newVec2f(0.0f, 0.0f);
	editor->camera.viewportSize.x = 1;
	editor->camera.viewportSize.y = 1;

	editor->bgColor = newColor(220, 220, 220, 0);
	editor->gridSize = 16;
	editor->selectBoxActive = 0;

	editor->editChipID = 0;
	editor->editChipNumInputs = 0;

	// keys control
	editor->zoomOutKey = SDL_SCANCODE_MINUS;
	editor->zoomInKey = SDL_SCANCODE_EQUALS;
}

void initEditorUI(Editor *editor, UICtx *ctx) {
	editor->menubarHeight = 80;
	setUICachedText(&editor->startSimulationText, ctx->renderer, ctx->font, "Simulate", newColor(0, 0, 0, 255));
	setUICachedText(&editor->stopSimulationText, ctx->renderer, ctx->font, "Stop", newColor(0, 0, 0, 255));
}

void menubarClicked(void *state) {
	Editor *editor = (Editor*)state;

	editor->menubarHeight += 4;
}

void simulateButtonClicked(void *state) {
	Editor *editor = (Editor*)state;

	editor->menubarHeight -= 4;
}

void editorUI(UICtx *uiCtx, Editor *editor) {
	uiBeginLayout(uiCtx, &(UILayoutOptions) {
		.size.y = editor->menubarHeight,
		.padding = newVec4i(10, 10, 10, 10),
		.sizing = UI_FILL_WIDTH,
		.bgColor = newColor(255, 255, 255, 255),

		.eventStateObject = editor,
		.onClick = &menubarClicked
	});

	if (1) {
		uiBeginLayout(uiCtx, &(UILayoutOptions) {
			.size = newVec2i(120, 50),
			.padding = newVec4i(10, 10, 10, 10),
			.bgColor = newColor(50, 200, 50, 255),

			.eventStateObject = editor,
			.onClick = &simulateButtonClicked
		});

		uiLabel(uiCtx, &(UILabelOptions) {
			.cachedText = &editor->startSimulationText,
			.fontSize = 24
		});

		uiEndLayout(uiCtx);
	}

	uiEndLayout(uiCtx);

	/*u32 menubar = newUIElement(ui);
	ui->array[menubar].type = UI_BOX;
	ui->array[menubar].attachPosition = newVec2i(0,0);
	ui->array[menubar].size.y = editor->menubarHeight;
	ui->array[menubar].data.box.sizing.width = BOX_SIZING_WINDOW;
	ui->array[menubar].data.box.bgColor = newColor(225, 225, 225, 255);

	u32 simulateButton = newUIElement(ui);
	ui->array[simulateButton].parentID = menubar;
	ui->array[simulateButton].type = UI_BUTTON;
	ui->array[simulateButton].attachPosition = newVec2i(50, 50);
	ui->array[simulateButton].size = newVec2i(80, 40);
	ui->array[simulateButton].data.button.bgColor = newColor(50, 200, 50, 255);
	ui->array[simulateButton].data.button.onClick = &simulateButtonOnClick;*/
}
