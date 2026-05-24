#include "textures.h"
#include "ui.h"
#include "unit.h"

void loadTextures(SDL_Renderer *renderer, Textures *textures, TTF_Font *font) {
	Color White = {255, 255, 255, 0};

	// AND,OR,NOT,NAND,NOR,XOR,XNOR
	textures->simpleAND = newTextTexture(renderer, "AND", font, White);
	textures->simpleOR = newTextTexture(renderer, "OR", font, White);
	textures->simpleNOT = newTextTexture(renderer, "NOT", font, White);
	textures->simpleNAND = newTextTexture(renderer, "NAND", font, White);
	textures->simpleNOR = newTextTexture(renderer, "NOR", font, White);
	textures->simpleXOR = newTextTexture(renderer, "XOR", font, White);
	textures->simpleXNOR = newTextTexture(renderer, "XNOR", font, White);
}
