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
	u32 c = newInputChip(chips, SWITCH);
	chips->array[c].position = newVec2(250.0f, 50.0f);

	u32 out = newSimpleChip(chips, XOR);
	chips->array[out].position = newVec2(75.0f, 250.0f);
	linkChipInsignal(chips, out, 0, a, 0);
	linkChipInsignal(chips, out, 1, b, 0);

	u32 out2 = newSimpleChip(chips, AND);
	chips->array[out2].position = newVec2(150.0f, 320.0f);
	linkChipInsignal(chips, out2, 0, out, 0);
	linkChipInsignal(chips, out2, 1, c, 0);

	// SR LATCH
	u32 s = newInputChip(chips, SWITCH);
	u32 r = newInputChip(chips, SWITCH);
	chips->array[s].position = newVec2(350.0f, 50.0f);
	chips->array[r].position = newVec2(450.0f, 50.0f);

	u32 nor = newSimpleChip(chips, NOR);
	chips->array[nor].position = newVec2(350.0f, 250.0f);
	// in a real world, one gate would be reached faster
	getSimpleChip(chips, nor)->out = 1;

	u32 nor2 = newSimpleChip(chips, NOR);
	chips->array[nor2].position = newVec2(450.0f, 200.0f);

	u32 orgate = newSimpleChip(chips, OR);
	chips->array[orgate].position = newVec2(375.0f, 350.0f);

	linkChipInsignal(chips, nor, 0, s, 0);
	linkChipInsignal(chips, nor, 1, nor2, 0);

	linkChipInsignal(chips, nor2, 1, r, 0);
	linkChipInsignal(chips, nor2, 0, nor, 0);

	linkChipInsignal(chips, orgate, 0, nor, 0);
	linkChipInsignal(chips, orgate, 1, nor, 0);

	app.running = 1;
	while (app.running) {
		handleEvents(&app);
		update(&app);

		render(&app);
		SDL_Delay(33);
	}

	closeApp(&app);
	return 0;
}
