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
	textures->boneEditorIcon = loadPNGTexture(renderer, "assets/Bone.png");
}

void destroyTextures(Textures *textures) {
	if (textures->simpleAND) SDL_DestroyTexture(textures->simpleAND);
	if (textures->simpleOR) SDL_DestroyTexture(textures->simpleOR);
	if (textures->simpleNOT) SDL_DestroyTexture(textures->simpleNOT);
	if (textures->simpleNAND) SDL_DestroyTexture(textures->simpleNAND);
	if (textures->simpleNOR) SDL_DestroyTexture(textures->simpleNOR);
	if (textures->simpleXOR) SDL_DestroyTexture(textures->simpleXOR);
	if (textures->simpleXNOR) SDL_DestroyTexture(textures->simpleXNOR);
	if (textures->boneEditorIcon) SDL_DestroyTexture(textures->boneEditorIcon);

	textures->simpleAND = NULL;
	textures->simpleOR = NULL;
	textures->simpleNOT = NULL;
	textures->simpleNAND = NULL;
	textures->simpleNOR = NULL;
	textures->simpleXOR = NULL;
	textures->simpleXNOR = NULL;
	textures->boneEditorIcon = NULL;
}
