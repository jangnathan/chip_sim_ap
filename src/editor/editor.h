#pragma once
#include "ctx.h"
#include "ui.h"
#include "chip.h"
#include "input.h"
#include "camera.h"

typedef enum {
	EDIT_NONE,
	EDIT_SELECT_OPTION,
	EDIT_MOVE_CHIP,
	EDIT_SELECT_IN_LINK_CHIP,	
	EDIT_FIND_LINK_CHIP,	
	EDIT_SELECT_OUT_LINK_CHIP
} EditState;

typedef struct {
	EditState state;
	Ctx *ctx;

	u16 menubarHeight;

	// selection box
	Vec2i selectBoxPos;
	Vec2i selectBoxSize;
	u8 selectBoxActive;

	u32 editChipID;
	u8 editChipNumInputs;
	u8 editChipInOption;
	u8 tempChipID;
	u8 tempChipNumOutputs;

	Camera camera;

	Color bgColor;
	u16 gridSize;

	// input options
	SDL_Scancode zoomOutKey;
	SDL_Scancode zoomInKey;

	u8 simulating;
} Editor;

void initEditor(Editor *editor);
void editorUI(UICtx *ctx, Editor *editor);
void updateEditor(Editor *editor, Input *input, Chips *chips);

void editorHandleKeypress(Editor *editor, SDL_KeyboardEvent event);
