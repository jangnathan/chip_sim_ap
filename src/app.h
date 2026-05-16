#pragma once
#include "chip.h"
#include "ui/ui.h"
#include "unit.h"
#include "camera.h"
#include "input.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
	Chips chips;
} Ctx;

typedef struct {
	SDL_Texture *simpleAND;
	SDL_Texture *simpleOR;
	SDL_Texture *simpleNOT;
	SDL_Texture *simpleNAND;
	SDL_Texture *simpleNOR;
	SDL_Texture *simpleXOR;
	SDL_Texture *simpleXNOR;
} Textures;

typedef enum {
	EDIT_NONE,
	EDIT_SELECT_OPTION,
	EDIT_MOVE_CHIP,
	EDIT_SELECT_IN_LINK_CHIP,	
	EDIT_FIND_LINK_CHIP,	
	EDIT_SELECT_OUT_LINK_CHIP
} EditState;

typedef struct {
	SDL_Texture *stopSimulateTexture;
	SDL_Texture *startSimulateTexture;

	// menu bar
	UIBox simulateButton;
	UIBox createButton;
	UIBox createMenu;

	// menu chip edit select menu
	UIBox editChipBox;
	UIBox editChipMoveButton;
	UIBox editChipLinkButton;

	// select input/output of linking chip
	SDL_Texture *selectLinkInTitleTexture;
	SDL_Texture *selectLinkOutTitleTexture;
	UIBox title;
	UIBox selectLinkChipBox;
	UIBox selectLinkChipOption[4];
	u8 selectLinkChipOptionsShown; // num shown

	TextInputID activeTextInput;
	UITextInput textInputs[NUM_TEXT_INPUTS];
} UI;

typedef struct {
	Ctx ctx;
	SDL_Window *window;
	SDL_Renderer *renderer;

	Textures textures;
	TTF_Font *font;
	Color bgColor;

	EditState editState;

	i32 winWidth;
	i32 winHeight;

	u16 menubarHeight;
	u16 gridSize;
	UI ui;

	u8 selectBoxActive;
	Vec2 selectBoxPos;
	Vec2 selectBoxSize;

	// editor logic
	u32 editChipID;
	u8 editChipNumInputs;
	u8 editChipInOption;
	u8 tempChipID;
	u8 tempChipNumOutputs;

	Camera camera;

	Input input;
	u8 running;
	u8 simulating;
} App;

void initApp(App *app);
void update(App *app);
void closeApp(App *app);
