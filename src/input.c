#include "input.h"

void initInput(Input *input) {
	input->mouse.leftClick = 0;
	input->mouse.rightClick = 0;
	input->mouse.leftButtonHeld = 0;
	input->mouse.centerPosition = newVec2i(0, 0);
	input->mouse.oldCenterPosition = newVec2i(0, 0);
	input->mouse.position = newVec2i(0, 0);
}

// called at start of event
void updateInput(Input *input) {
	// reset clicks after event
	input->mouse.leftClick = 0;
	input->mouse.rightClick = 0;
}

void closeInput(Input *input) {
	SDL_DestroyCursor(input->mouse.cursorDefault);
	SDL_DestroyCursor(input->mouse.cursorMove);
}
