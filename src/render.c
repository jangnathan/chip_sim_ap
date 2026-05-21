#include "render.h"
#include "editor/render.h"
#include "unit.h"
#include <math.h>

// UI rendering
void renderBox(SDL_Renderer *renderer, UIElement *element) {
	SDL_FRect background = {element->position.x, element->position.y, element->size.x, element->size.y};
	UIBox box = element->data.box;

	SDL_SetRenderDrawColor(renderer, box.bgColor.r, box.bgColor.g, box.bgColor.b,  box.bgColor.a);
	SDL_RenderFillRect(renderer, &background);
	if (element->texture != 0) {
		SDL_RenderTexture(renderer, element->texture, NULL, &background);
	}
}

void renderInputText(SDL_Renderer *renderer, UIElement *element) {
	SDL_FRect box = {element->position.x, element->position.y, element->size.x, element->size.y};

	UITextInput input = element->data.textInput;
	SDL_SetRenderDrawColor(renderer, input.bgColor.r, input.bgColor.g, input.bgColor.b,  input.bgColor.a);
	SDL_RenderFillRect(renderer, &box);

	/*SDL_FRect textbox = {element->position.x, element->position.y, input.textLen * input.fontSize * 0.5, input.size.y};

	if (element->texture != 0) {
		SDL_RenderTexture(renderer, element->texture, NULL, &textbox);
	}*/
}

void render(App *app) {
	Ctx *ctx = &app->ctx;
	Chips *chips = &ctx->chips;
	SDL_Renderer *renderer = app->renderer;
	SDL_Window *window = app->window;

	switch (app->state) {
		case ST_NONE:
			break;
		case ST_EDIT:
			renderEditor(renderer, &app->textures, app->winWidth, &app->editor, chips);
			break;
	}

	SDL_RenderPresent(renderer);
}
