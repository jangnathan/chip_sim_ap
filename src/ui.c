#include "ui.h"
#include <string.h>
#include <stdlib.h>

void initUI(UI *ui, u16 size) {
	ui->size = size;
	ui->len = 1; // to make 0 the root
	ui->array = malloc(sizeof(UIElement) * size);
}

u16 newUIElement(UI *ui) {
	ui->len++;

	if (ui->len >= ui->size) {
		ui->size *= 2;
		ui->array = realloc(ui->array, sizeof(UIElement) * ui->size);
	}

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
