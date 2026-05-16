#include "input.h"

u8 hitboxClicked(Mouse *mouse, Hitbox hitbox) {
	if (collideABB(mouse->position, hitbox.position, hitbox.size)) {
		// hover effect
		if (mouse->leftClick) {
			mouse->leftClick = 0;
			return 1;
		}
	}
	return 0;
}

void resetInput(Input *input) {
	input->mouse.leftClick = 0;
	input->mouse.rightClick = 0;
}

void closeInput(Input *input) {
	SDL_DestroyCursor(input->mouse.cursorDefault);
	SDL_DestroyCursor(input->mouse.cursorMove);
}
