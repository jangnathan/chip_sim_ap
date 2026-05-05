#include "ui.h"

SDL_Texture *newTextTexture(SDL_Renderer *renderer, char *text, TTF_Font *font, Color color) {
	SDL_Surface* surface;
	SDL_Color sdl_color = {color.r, color.g, color.b, color.a};
	surface = TTF_RenderText_Blended(font, text, 0, sdl_color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	return texture;
}

UIBox newBox(Vec2 pos, Vec2 size, SDL_Texture *texture, Color bgColor) {
	UIBox x;
	x.position = pos;
	x.size = size;
	x.texture = texture;
	x.bgColor = bgColor;
	return x;
}
