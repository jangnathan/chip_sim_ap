#include "render.h"
#include <math.h>

SDL_FRect createRenderRect(App *app, float x, float y, float width, float height) {
	SDL_FRect rect = {(x - app->camera.position.x) * app->camera.zoom,
		(y - app->camera.position.y) * app->camera.zoom + app->menubarHeight, width * app->camera.zoom, height * app->camera.zoom};
	return rect;
}

void drawSwitch(App *app, u8 on, float x, float y) {
	SDL_Renderer *renderer = app->renderer;

	SDL_FRect indicator = createRenderRect(app, x, y, 50.0f, 20.0f);
	SDL_FRect box = createRenderRect(app, x, y + 20.0f, 50.0f, 100.0f);
	SDL_FRect switchPath = createRenderRect(app, x + 5.0f, y + 20.0f + 5.0f, 40.0f, 90.0f);
	SDL_FRect switchBox = createRenderRect(app, x + 5.0f, y + 20.0f + 5.0f + (on * 45.0f), 40.0f, 45.0f);

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

void drawClock(App *app, float x, float y) {
}

// a means input, b means the output which is used for out input
void drawWiring(App *app, float ax, float ay, float bx, float by) {
	SDL_Renderer *renderer = app->renderer;

	SDL_FRect vert = createRenderRect(app, ax, ay, 2.0f, by - ay - 2.0f);
	SDL_FRect hor = createRenderRect(app, bx, by - 4.0f, ax - bx, 2.0f);
	SDL_FRect nub = createRenderRect(app, bx, by - 4.0f, 2.0f, 4.0f);

	SDL_SetRenderDrawColor(renderer, 25, 25, 120, 0);
	SDL_RenderFillRect(renderer, &vert);
	SDL_RenderFillRect(renderer, &hor);
	SDL_RenderFillRect(renderer, &nub);
}

void renderSimpleChip(App *app, Vec2 pos, SimpleChip *simpleChip) {
	ChipEntity* chipsArray = app->ctx.chips.array;
	SDL_Renderer *renderer = app->renderer;
	Textures *textures = &app->textures;

	SDL_FRect box = createRenderRect(app, pos.x, pos.y, 50.0f, 50.0f);
	SDL_SetRenderDrawColor(renderer, 25, 25, 25, 0);
	SDL_RenderFillRect(renderer, &box);

	//Vec2 wireIn = translateVec2(pos, newVec2(10.0f, 0.0f));
	Vec2 inAPos = chipsArray[simpleChip->inSignals[0].ID].position;
	Vec2 inBPos = chipsArray[simpleChip->inSignals[1].ID].position;
	drawWiring(app, inAPos.x, inAPos.y + 30, pos.x, pos.y);
	drawWiring(app, inBPos.x, inBPos.y + 30, pos.x + 48.0f, pos.y);

	SDL_FRect text = createRenderRect(app, pos.x + 5.0f, pos.y + 15.0f, 40.0f, 20.0f);
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

	SDL_FRect indicator = createRenderRect(app, pos.x, pos.y + 50, 50.0f, 10.0f);

	if (simpleChip->out) {
		SDL_SetRenderDrawColor(renderer, 205, 20, 20, 0);
	} else {
		SDL_SetRenderDrawColor(renderer, 80, 30, 30, 0);
	}
	SDL_RenderFillRect(renderer, &indicator);
}

void renderInputChip(App *app, Vec2 pos, InputChip *inputChip) {
	switch (inputChip->type) {
		case CLOCK:
			drawClock(app, pos.x, pos.y);
			break;
		case SWITCH:
			drawSwitch(app, inputChip->out, pos.x, pos.y);
			break;
	}
}

void renderChip(App *app, u32 i) {
	Ctx *ctx = &app->ctx;
	Chips *chips = &ctx->chips;

	u32 typeID = chips->array[i].ID;

	switch (chips->array[i].type) {
		case CE_NONE:
			break;
		case CE_SIMPLE:
			renderSimpleChip(app, chips->array[i].position, chips->simpleChipsArray + typeID);
			break;
		case CE_INPUT:
			renderInputChip(app, chips->array[i].position, chips->inputChipsArray + typeID);
			break;
	}
}

void renderButton(SDL_Renderer *renderer, UIButton button) {
	SDL_FRect background = {button.position.x, button.position.y, button.size.x, button.size.y};
	SDL_SetRenderDrawColor(renderer, button.bgColor.r, button.bgColor.g, button.bgColor.b,  button.bgColor.a);
	SDL_RenderFillRect(renderer, &background);
	SDL_RenderTexture(renderer, button.texture, NULL, &background);
}

void renderGrid(App *app, float x, float y, float w, float h) {
	SDL_Renderer *renderer = app->renderer;

	SDL_SetRenderDrawColor(renderer, 180, 180, 180, app->bgColor.a);
	for (u16 i = 0; i < h / app->gridSize; i++) {
		// line across x axis
		SDL_FRect lineX = {0, i * app->gridSize - fmodf(app->camera.position.y, app->gridSize) * app->camera.zoom + y, w, 1.0f};
		SDL_RenderFillRect(renderer, &lineX);
	}
	for (u16 i = 0; i < w / app->gridSize; i++) {
		SDL_FRect lineY = {i * app->gridSize - fmodf(app->camera.position.x, app->gridSize) * app->camera.zoom + x, y, 1.0f, h};
		SDL_RenderFillRect(renderer, &lineY);
	}
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
	renderGrid(app, 0, app->menubarHeight, winWidth, winHeight - app->menubarHeight);

	// render chips
	for (u32 i = 1; i < chips->len; i++) {
		renderChip(app, i);
	}

	// render UI
	SDL_FRect menubar = {0.0f, 0, winWidth, app->menubarHeight};
	SDL_FRect menubarOutline = {0.0f, app->menubarHeight, winWidth, 1.0f};
	SDL_SetRenderDrawColor(renderer, 245, 245, 245, 0);
	SDL_RenderFillRect(renderer, &menubar);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(renderer, &menubarOutline);

	renderButton(renderer, ui->simulateButton);

	SDL_RenderPresent(renderer);
}
