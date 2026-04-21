#include "render.h"

void drawSwitch(SDL_Renderer *renderer, u8 on, float x, float y) {
	SDL_FRect indicator = {x, y, 50.0f, 20.0f};
	SDL_FRect box = {x, y + 20.0f, 50.0f, 100.0f};
	SDL_FRect switchPath = {x + 5.0f, y + 20.0f + 5.0f, 40.0f, 90.0f};
	SDL_FRect switchBox = {x + 5.0f, y + 20.0f + 5.0f + (on * 45.0f), 40.0f, 45.0f};

	if (on) {
		SDL_SetRenderDrawColor(renderer, 205, 20, 20, 0);
	} else {
		SDL_SetRenderDrawColor(renderer, 80, 30, 30, 0);
	}
	SDL_RenderFillRect(renderer, &indicator);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	SDL_RenderFillRect(renderer, &box);

	SDL_SetRenderDrawColor(renderer, 205, 205, 205, 0);
	SDL_RenderFillRect(renderer, &switchPath);

	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
	SDL_RenderFillRect(renderer, &switchBox);
}

void drawClock(SDL_Renderer *renderer, float x, float y) {
}

void renderSimpleChip(App *app, ChipEntity *chip, SimpleChip *simpleChip) {

}

void renderInputChip(App *app, ChipEntity *chip, InputChip *inputChip) {
	switch (inputChip->type) {
		case CLOCK:
			drawClock(app->renderer, chip->position.x, chip->position.y);
			break;
		case SWITCH:
			drawSwitch(app->renderer, inputChip->out, chip->position.x, chip->position.y);
			break;
	}
}

void renderChip(App *app, u32 i) {
	Ctx *ctx = &app->ctx;
	Chips *chips = &ctx->chips;

	u32 localID = chips->array[i].ID;

	switch (chips->array[i].type) {
		case CE_SIMPLE:
			renderSimpleChip(app, chips->array + i, chips->simpleChipsArray + localID);
			break;
		case CE_INPUT:
			renderInputChip(app, chips->array + i, chips->inputChipsArray + localID);
			break;
	}
}

void render(App *app) {
	Ctx *ctx = &app->ctx;
	Chips *chips = &ctx->chips;
	SDL_Renderer *renderer = app->renderer;

	Color bgColor = app->bgColor;
	SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b,  bgColor.a);
	SDL_RenderClear(renderer);

	for (u32 i = 0; i < chips->len; i++) {
		renderChip(app, i);
	}
	SDL_RenderPresent(renderer);
}
