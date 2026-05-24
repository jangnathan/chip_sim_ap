#include "ui.h"
#include <string.h>
#include <stdlib.h>

u32 newUIElement(UI *ui) {
	ui->array[ui->len].parentID = 0;

	if (ui->len >= ui->size) {
		ui->size *= 2;
		ui->array = realloc(ui->array, sizeof(UIElement) * ui->size);
	}

	ui->len++;

	return ui->len - 1;
}

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color) {
	SDL_Surface* surface;
	SDL_Color sdl_color = {color.r, color.g, color.b, color.a};
	surface = TTF_RenderText_Blended(font, text, 0, sdl_color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	return texture;
}

void initUI(UI *ui, u32 size) {
	ui->size = size;
	ui->len = 1; // to make 0 the root
	ui->array = malloc(sizeof(UIElement) * size);
}

void updateButton(UI* ui, UIElement *element) {
}

void updateBox(UI* ui, UIElement *element) {
}

void updateLabel(UI* ui, UIElement *element) {
}

void updateTextInput(UI* ui, UIElement *element) {
}

void updateUI(UI *ui) {
	for (u32 i = 0; i < ui->len; i++) {
		switch (ui->array[i].type) {
			case UI_BUTTON:
				updateButton(ui, ui->array + i);
				break;
			case UI_BOX:
				break;
			case UI_LABEL:
				break;
			case UI_TEXT_INPUT:
				break;
		}
	}
}

void uiHandleInput(Input *input, UI *ui) {
}

void renderButton(SDL_Renderer *renderer, UIElement *element) {
	SDL_FRect border1 = {};
	SDL_FRect border2 = {};
	SDL_FRect border3 = {};
	SDL_FRect border4 = {};
	SDL_FRect background = {};
}

void renderBox(SDL_Renderer *renderer, UIElement *element) {
}

void renderUI(SDL_Renderer *renderer, UI *ui) {
	for (u32 i = 0; i < ui->len; i++) {
		switch (ui->array[i].type) {
			case UI_BUTTON:
				renderButton(renderer, ui->array + i);
				break;
			case UI_BOX:
				renderBox(renderer, ui->array + i);
				break;
			case UI_LABEL:
				break;
			case UI_TEXT_INPUT:
				break;
		}
	}
}
