#include "event.h"
#include "ui/ui.h"
#include "input.h"
#include <stdio.h>

void handleKeyPress(App *app, SDL_KeyboardEvent event) {
	if (app->ui.activeTextInput < textInputNone) {
		UITextInput *textInput = app->ui.textInputs + (u32)app->ui.activeTextInput;

		textInput->text[textInput->textLen] = '\0'; // null terminate

		// convert keycode to add text to the active text input

		setUITextInputText(app->renderer, app->font, textInput, "hello");
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
	Input *input = &app->input;
	resetInput(input);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_EVENT_QUIT:
				app->running = 0;
				break;
			case SDL_EVENT_MOUSE_MOTION:
				input->mouse.position = newVec2(event.motion.x, event.motion.y);
				input->mouse.centerPosition = newVec2(event.motion.x - app->winWidth / 2, app->winHeight / 2 - event.motion.y);
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					// for dragging logic
					input->mouse.oldCenterPosition = input->mouse.centerPosition;
					app->camera.oldPosition = app->camera.position;

					input->mouse.leftKeyHeld = 1;
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				if (event.button.button == SDL_BUTTON_LEFT) {
					input->mouse.leftKeyHeld = 1;
					input->mouse.leftClick = 1;
				}
				if (event.button.button == SDL_BUTTON_RIGHT) {
					input->mouse.rightClick = 1;
				}
				input->mouse.leftKeyHeld = 0;
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
