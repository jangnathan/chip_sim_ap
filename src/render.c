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

// a means input, b means the output which is used for out input
void drawWiring(SDL_Renderer *renderer, float ax, float ay, float bx, float by) {
	SDL_FRect vert = {ax, ay, 2.0f, by - ay - 2.0f};
	SDL_FRect hor = {bx, by - 4.0f, ax - bx, 2.0f};
	SDL_FRect nub = {bx, by - 4.0f, 2.0f, 4.0f};

	SDL_SetRenderDrawColor(renderer, 25, 25, 120, 0);
	SDL_RenderFillRect(renderer, &vert);
	SDL_RenderFillRect(renderer, &hor);
	SDL_RenderFillRect(renderer, &nub);
}

void renderSimpleChip(App *app, ChipEntity *chip, SimpleChip *simpleChip) {
	ChipEntity* chipsArray = app->ctx.chips.array;
	SDL_Renderer *renderer = app->renderer;
	Textures *textures = &app->textures;

	// pos used to render
	Vec2 pos = chip->position;
	pos.y += app->menubarHeight;

	SDL_FRect box = {pos.x, pos.y, 50.0f, 50.0f};
	SDL_SetRenderDrawColor(renderer, 25, 25, 25, 0);
	SDL_RenderFillRect(renderer, &box);

	//Vec2 wireIn = translateVec2(pos, newVec2(10.0f, 0.0f));
	Vec2 inAPos = chipsArray[simpleChip->inSignals[0].ID].position;
	Vec2 inBPos = chipsArray[simpleChip->inSignals[1].ID].position;
	drawWiring(renderer, inAPos.x, inAPos.y + app->menubarHeight, pos.x, pos.y);
	drawWiring(renderer, inBPos.x, inBPos.y + app->menubarHeight, pos.x + 48.0f, pos.y);

	SDL_FRect text = {pos.x + 5.0f, pos.y + 15.0f, 40.0f, 20.0f};
	switch (simpleChip->type) {
		case AND:
			SDL_RenderTexture(renderer, textures->simpleAND, NULL, &text);
			break;
		case OR:
			SDL_RenderTexture(renderer, textures->simpleOR, NULL, &text);
			break;
		case NOT:
			SDL_RenderTexture(renderer, textures->simpleNOT, NULL, &text);
			break;
		case NAND:
			SDL_RenderTexture(renderer, textures->simpleNAND, NULL, &text);
			break;
		case NOR:
			SDL_RenderTexture(renderer, textures->simpleNOR, NULL, &text);
			break;
		case XOR:
			SDL_RenderTexture(renderer, textures->simpleXOR, NULL, &text);
			break;
		case XNOR:
			SDL_RenderTexture(renderer, textures->simpleXNOR, NULL, &text);
			break;
	}

	SDL_FRect indicator = {pos.x, pos.y + 50, 50.0f, 10.0f};

	if (simpleChip->out) {
		SDL_SetRenderDrawColor(renderer, 205, 20, 20, 0);
	} else {
		SDL_SetRenderDrawColor(renderer, 80, 30, 30, 0);
	}
	SDL_RenderFillRect(renderer, &indicator);
}

void renderInputChip(App *app, ChipEntity *chip, InputChip *inputChip) {
	switch (inputChip->type) {
		case CLOCK:
			drawClock(app->renderer, chip->position.x, chip->position.y + app->menubarHeight);
			break;
		case SWITCH:
			drawSwitch(app->renderer, inputChip->out, chip->position.x, chip->position.y + app->menubarHeight);
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

void renderButton(SDL_Renderer *renderer, UIButton button) {
	SDL_FRect background = {button.position.x, button.position.y, button.size.x, button.size.y};
	SDL_SetRenderDrawColor(renderer, button.bgColor.r, button.bgColor.g, button.bgColor.b,  button.bgColor.a);
	SDL_RenderFillRect(renderer, &background);
	SDL_RenderTexture(renderer, button.texture, NULL, &background);
}

void render(App *app) {
	Ctx *ctx = &app->ctx;
	UI *ui = &app->ui;
	Chips *chips = &ctx->chips;
	SDL_Renderer *renderer = app->renderer;
	SDL_Window *window = app->window;

	Color bgColor = app->bgColor;
	SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b,  bgColor.a);
	SDL_RenderClear(renderer);

	int winWidth, winHeight;
	SDL_GetWindowSize(window, &winWidth, &winHeight);
	SDL_FRect menubar = {0.0f, app->menubarHeight, winWidth, 1.0f};
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, bgColor.a);
	SDL_RenderFillRect(renderer, &menubar);

	renderButton(renderer, ui->simulateButton);

	// render chips
	for (u32 i = 1; i < chips->len; i++) {
		renderChip(app, i);
	}
	SDL_RenderPresent(renderer);
}
