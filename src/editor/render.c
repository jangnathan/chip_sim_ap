#include "render.h"
#include <math.h>

SDL_FRect createRenderRect(Camera camera, float x, float y, i32 width, i32 height) {
	Vec2i screenPos = world2screenVec2i(camera, newVec2f(x, y));
	SDL_FRect rect = {screenPos.x - width * camera.zoom / 2,
		screenPos.y - height * camera.zoom / 2, width * camera.zoom, height * camera.zoom};
	return rect;
}

void drawSwitch(SDL_Renderer *renderer, Camera camera, u8 on, float x, float y) {
	SDL_FRect indicator = createRenderRect(camera, x, y + 50.0f, 50.0f, 10.0f);
	SDL_FRect box = createRenderRect(camera, x, y, 50.0f, 90.0f);
	SDL_FRect switchPath = createRenderRect(camera, x, y, 40.0f, 80.0f);
	SDL_FRect switchBox = createRenderRect(camera, x, y - 20.0f + (on * 40.0f), 40.0f, 40.0f);

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

void drawClock(SDL_Renderer *renderer, Camera camera, float x, float y) {
	SDL_FRect background = createRenderRect(camera, x, y, 50.0f, 50.0f);
	SDL_FRect inside = createRenderRect(camera, x + 2.0f, y + 2.0f, 46.0f, 46.0f);
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
	SDL_RenderFillRect(renderer, &background);

	// use a clock texture
}

void renderSimpleChip(SDL_Renderer *renderer, Textures *textures, Camera camera, Vec2f pos, SimpleChip *simpleChip) {
	SDL_FRect box = createRenderRect(camera, pos.x, pos.y, 50.0f, 50.0f);
	SDL_SetRenderDrawColor(renderer, 25, 25, 25, 0);
	SDL_RenderFillRect(renderer, &box);

	SDL_FRect text = createRenderRect(camera, pos.x, pos.y, 40.0f, 20.0f);
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

	SDL_FRect indicator = createRenderRect(camera, pos.x, pos.y + 30, 50.0f, 10.0f);

	if (simpleChip->out) {
		SDL_SetRenderDrawColor(renderer, 205, 20, 20, 0);
	} else {
		SDL_SetRenderDrawColor(renderer, 80, 30, 30, 0);
	}
	SDL_RenderFillRect(renderer, &indicator);
}

void renderInputChip(SDL_Renderer *renderer, Camera camera, Vec2f pos, InputChip *inputChip) {
	switch (inputChip->type) {
		case CLOCK:
			drawClock(renderer, camera, pos.x, pos.y);
			break;
		case SWITCH:
			drawSwitch(renderer, camera, inputChip->out, pos.x, pos.y);
			break;
	}
}

void renderChip(SDL_Renderer *renderer, Textures *textures, Camera camera, Chips *chips, u32 i) {
	u32 typeID = chips->array[i].typeID;

	switch (chips->array[i].type) {
		case CE_NONE:
			break;
		case CE_SIMPLE:
			renderSimpleChip(renderer, textures, camera, chips->array[i].position, chips->simpleChipsArray + typeID);
			break;
		case CE_INPUT:
			renderInputChip(renderer, camera, chips->array[i].position, chips->inputChipsArray + typeID);
			break;
	}
}

void renderGrid(SDL_Renderer *renderer, Editor *editor, i32 x, i32 y, i32 w, i32 h) {
	SDL_SetRenderDrawColor(renderer, 180, 180, 180, editor->bgColor.a);

	for (u16 i = 1; i < h / editor->gridSize; i++) {
		// line across x axis
		SDL_FRect lineX = {0, i * editor->gridSize - ((i32)editor->camera.position.y % editor->gridSize) * editor->camera.zoom + y, w, 1.0f};
		SDL_RenderFillRect(renderer, &lineX);
	}
	for (u16 i = 1; i < w / editor->gridSize; i++) {
		SDL_FRect lineY = {i * editor->gridSize - ((i32)editor->camera.position.x % editor->gridSize) * editor->camera.zoom + x, y, 1.0f, h};
		SDL_RenderFillRect(renderer, &lineY);
	}
}

void renderEditor(SDL_Renderer *renderer, Textures *textures, Editor *editor) {
	Camera camera = editor->camera;
	Chips *chips = &editor->ctx->chips;
	Color bgColor = editor->bgColor;

	renderGrid(renderer, editor, 0, editor->menubarHeight, camera.viewportSize.x, camera.viewportSize.y);

	// render chips
	for (u32 i = 1; i < chips->len; i++) {
		renderChip(renderer, textures, camera, chips, i);
	}

	if (editor->selectBoxActive) {
		SDL_FRect box = {editor->selectBoxPos.x - editor->selectBoxSize.x / 2, editor->selectBoxPos.y - editor->selectBoxSize.y / 2,
			editor->selectBoxSize.x, editor->selectBoxSize.y};
		SDL_SetRenderDrawColor(renderer, 50, 50, 255, 0);
		SDL_RenderRect(renderer, &box);
	}

	//renderBox(renderer, &ui->simulateButton);
	switch (editor->state) {
		case EDIT_SELECT_OPTION: {
			//renderBox(renderer, &ui->editChipBox);
			//renderBox(renderer, &ui->editChipMoveButton);
			//renderBox(renderer, &ui->editChipLinkButton);
			break;
		}
		case EDIT_MOVE_CHIP: {
			//renderInputText(renderer, ui->textInputs + textInputPositionX);
			//renderInputText(renderer, ui->textInputs + textInputPositionY);
			break;
		}
		case EDIT_SELECT_IN_LINK_CHIP: {
			/*renderBox(renderer, &ui->selectLinkChipBox);
			for (u8 i = 0; i < app->editChipNumInputs; i++) {
				renderBox(renderer, ui->selectLinkChipOption + i);
			}*/
			break;
		}
		case EDIT_FIND_LINK_CHIP: {
			/*Vec2i mousePos = scaleVec2i(translateVec2i(editor->camera.position, input->mouse.position), 1.0f / editor->camera.zoom);
			mousePos.y -= editor->menubarHeight;*/
			break;
		}
		case EDIT_SELECT_OUT_LINK_CHIP: {
			/*renderBox(renderer, &ui->selectLinkChipBox);
			for (u8 i = 0; i < app->tempChipNumOutputs; i++) {
				renderBox(renderer, ui->selectLinkChipOption + i);
			}*/
			break;
		}
		default: {
			break;
		}
	}
}
