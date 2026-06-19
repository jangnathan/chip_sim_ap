#include "core/input.h"

void initInput(Input *input) {
	input->mouse.leftClick = 0;
	input->mouse.leftHeld = 0;
	input->mouse.leftDown = 0;
	input->mouse.rightClick = 0;
	input->mouse.centerPosition = newVec2i(0, 0);
	input->mouse.oldCenterPosition = newVec2i(0, 0);
	input->mouse.position = newVec2i(0, 0);

	input->mouse.cursorIcon = CURSOR_DEFAULT;

	input->mouse.cursorDefault =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	input->mouse.cursorMove =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
	input->mouse.cursorPointer =
		SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);

	SDL_SetCursor(input->mouse.cursorDefault);
}

// called at start of event
void updateInput(Input *input) {
	// reset clicks after event
	input->mouse.leftClick = 0;
	input->mouse.rightClick = 0;
	input->mouse.leftDown = 0;

	if (input->mouse.cursorIcon != input->mouse.prevCursorIcon) {
		input->mouse.prevCursorIcon = input->mouse.cursorIcon;

		switch (input->mouse.cursorIcon) {
			case CURSOR_DEFAULT:
				SDL_SetCursor(input->mouse.cursorDefault);
				break;
			case CURSOR_MOVE:
				SDL_SetCursor(input->mouse.cursorMove);
				break;
			case CURSOR_POINTER:
				SDL_SetCursor(input->mouse.cursorPointer);
				break;
		}
	}

	input->mouse.cursorIcon = CURSOR_DEFAULT;
}

void closeInput(Input *input) {
	SDL_DestroyCursor(input->mouse.cursorDefault);
	SDL_DestroyCursor(input->mouse.cursorMove);
	SDL_DestroyCursor(input->mouse.cursorPointer);
}
