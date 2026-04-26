#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include "chip.h"
#include "render.h"
#include "app.h"

int main() {
	// init
	App app;
	initApp(&app);

	Ctx *ctx = &app.ctx;
	Chips *chips = &ctx->chips;

	u32 a = newInputChip(chips, SWITCH);
	chips->array[a].position = newVec2(50.0f, 50.0f);
	u32 b = newInputChip(chips, SWITCH);
	chips->array[b].position = newVec2(150.0f, 50.0f);

	u32 out = newSimpleChip(chips, XOR);
	chips->array[out].position = newVec2(75.0f, 250.0f);
	linkChipInsignal(chips, out, 0, a, 0);
	linkChipInsignal(chips, out, 1, b, 0);

	app.running = 1;
	while (app.running) {
		handleEvents(&app);
		update(&app);

		render(&app);
		SDL_Delay(16);
	}

	closeApp(&app);
	return 0;
}
