#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include "chip.h"
#include "runtime.h"
#include "render.h"
#include "app.h"

int main() {
	// init
	App app;
	initApp(&app);

	Ctx *ctx = &app.ctx;
	Chips *chips = &ctx->chips;

	u32 a = newInputChip(chips, SWITCH);
	u32 b = newInputChip(chips, SWITCH);
	u32 out = newSimpleChip(chips, XOR);
	linkChipInsignal(chips, out, 0, a, 0);
	linkChipInsignal(chips, out, 1, b, 0);

	u8 running = 1;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					running = 0;
					break;

				default:
					break;
			}
		}

		render(&app);
		SDL_Delay(16);
	}

	closeApp(&app);
	return 0;
}
