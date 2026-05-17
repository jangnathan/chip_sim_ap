#include "input.h"

void resetInput(Input *input) {
	input->mouse.leftClick = 0;
	input->mouse.rightClick = 0;
}

void closeInput(Input *input) {
	SDL_DestroyCursor(input->mouse.cursorDefault);
	SDL_DestroyCursor(input->mouse.cursorMove);
}
