#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include "chip.h"
#include "render.h"
#include "app.h"

void createHalfAdder(Chips *chips, Vec2 position, u32 *carry_ptr) {
	// just a blank chip / group
	u32 halfAdder = newChipEntity(chips, 0, CE_NONE);
	chips->array[halfAdder].position = position;
	
	u32 in1 = newInputChip(chips, SWITCH);
	chips->array[in1].position = translateVec2(position, newVec2(0.0f, 0.0f));
	chips->array[in1].parentID = halfAdder;
	u32 in2 = newInputChip(chips, SWITCH);
	chips->array[in2].position = translateVec2(position, newVec2(0.0f, 140.0f));
	chips->array[in2].parentID = halfAdder;
	u32 carry = newSimpleChip(chips, AND);
	chips->array[carry].position = translateVec2(position, newVec2(-55.0f, -20.0f));
	chips->array[carry].parentID = halfAdder;
	u32 sum = newSimpleChip(chips, XOR);
	chips->array[sum].position = translateVec2(position, newVec2(0.0f, 340.0f));
	chips->array[sum].parentID = halfAdder;

	linkChipInsignal(chips, carry, 0, in1, 0);
	linkChipInsignal(chips, carry, 1, in2, 0);
	linkChipInsignal(chips, sum, 0, in1, 0);
	linkChipInsignal(chips, sum, 1, in2, 0);
	if (carry_ptr != 0) {
		*carry_ptr = carry;
	}
}

void createFullAdder(Chips *chips, Vec2 position, u32 in1, u32 in2, u32 in3, u32 *carryOut) {
	u32 halfAdder = newChipEntity(chips, 0, CE_NONE);
	chips->array[halfAdder].position = position;

	u32 sum = newSimpleChip(chips, XOR);
	chips->array[sum].position = translateVec2(position, newVec2(55.0f, 260.0f));
	linkChipInsignal(chips, sum, 0, in1, 0);
	linkChipInsignal(chips, sum, 1, in2, 0);

	u32 carry1 = newSimpleChip(chips, AND);
	chips->array[carry1].position = translateVec2(position, newVec2(0.0f, -100.0f));
	linkChipInsignal(chips, carry1, 0, in1, 0);
	linkChipInsignal(chips, carry1, 1, in2, 0);

	u32 sum2 = newSimpleChip(chips, XOR);
	chips->array[sum2].position = translateVec2(position, newVec2(0.0f, 340.0f));
	linkChipInsignal(chips, sum2, 0, sum, 0);
	linkChipInsignal(chips, sum2, 1, in3, 0);

	u32 carry2 = newSimpleChip(chips, AND);
	chips->array[carry2].position = translateVec2(position, newVec2(-70.0f, -105.0f));
	linkChipInsignal(chips, carry2, 0, sum, 0);
	linkChipInsignal(chips, carry2, 1, in3, 0);

	*carryOut = newSimpleChip(chips, OR);
	chips->array[*carryOut].position = translateVec2(position, newVec2(-55.0f, -20.0f));
	linkChipInsignal(chips, *carryOut, 0, carry1, 0);
	linkChipInsignal(chips, *carryOut, 1, carry2, 0);
}

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

	// half adder
	u32 carry1;
	createHalfAdder(chips, newVec2(1200.0f, 550.0f), &carry1);

	// full adder
	u32 in1 = newInputChip(chips, SWITCH);
	chips->array[in1].position = newVec2(1100.0f, 550.0f);
	u32 in2 = newInputChip(chips, SWITCH);
	chips->array[in2].position = newVec2(1100.0f, 670.0f);
	u32 carry2;
	createFullAdder(chips, newVec2(1100.0f, 550.0f), in1, in2, carry1, &carry2);

	u32 in3 = newInputChip(chips, SWITCH);
	chips->array[in3].position = newVec2(1000.0f, 550.0f);
	u32 in4 = newInputChip(chips, SWITCH);
	chips->array[in4].position = newVec2(1000.0f, 670.0f);
	u32 carry3;
	createFullAdder(chips, newVec2(1000.0f, 550.0f), in3, in4, carry2, &carry3);

	u32 in5 = newInputChip(chips, SWITCH);
	chips->array[in5].position = newVec2(900.0f, 550.0f);
	u32 in6 = newInputChip(chips, SWITCH);
	chips->array[in6].position = newVec2(900.0f, 670.0f);
	u32 carry4;
	createFullAdder(chips, newVec2(900.0f, 550.0f), in5, in6, carry3, &carry4);

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
