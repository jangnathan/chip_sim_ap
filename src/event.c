#include "event.h"
#include "ui.h"
#include "input.h"
#include <stdio.h>

void handleKeyPress(App *app, SDL_KeyboardEvent event) {
	/* OLD
	if (app->ui.activeTextInput < textInputNone) {
		UITextInput *textInput = app->ui.textInputs + (u32)app->ui.activeTextInput;

		textInput->text[textInput->textLen] = '\0'; // null terminate

		// convert keycode to add text to the active text input

		setUITextInputText(app->renderer, app->font, textInput, "hello");
		return;
	}*/

	switch (app->state) {
		case ST_NONE:
			break;
		case ST_EDIT:
			editorHandleKeypress(&app->editor, event);
			break;
	}
}

void handleEvents(App *app) {
	Input *input = &app->input;
	updateInput(input);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				app->running = 0;
				break;
			case SDL_EVENT_MOUSE_MOTION:
				input->mouse.position = newVec2i(event.motion.x, event.motion.y);
				input->mouse.centerPosition = newVec2i(event.motion.x - app->winWidth / 2, app->winHeight / 2 - event.motion.y);
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					// for dragging logic
					input->mouse.oldCenterPosition = input->mouse.centerPosition;
					input->mouse.leftDown = 1;
					input->mouse.leftHeld = 1;
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				if (event.button.button == SDL_BUTTON_LEFT) {
					input->mouse.leftHeld = 1;
					input->mouse.leftClick = 1;
				}
				if (event.button.button == SDL_BUTTON_RIGHT) {
					input->mouse.rightClick = 1;
				}
				input->mouse.leftHeld = 0;
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
