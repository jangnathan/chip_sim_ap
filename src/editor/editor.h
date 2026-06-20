#pragma once
#include "simulation/ctx.h"
#include "ui.h"
#include "simulation/chip.h"
#include "core/input.h"
#include "core/camera.h"

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

	// ui
	u16 menubarHeight;
	UICachedText startSimulationText;
	UICachedText stopSimulationText;
	UICachedText switchText;

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
void initEditorUI(Editor *editor, UICtx *ctx);

void editorHandleKeypress(Editor *editor, SDL_KeyboardEvent event);


// undoing / redoing
typedef enum {
	ACT_CREATE_CHIP,
	ACT_DELETE_CHIP,
	ACT_TRANSLATE_CHIP,
} ActionType;

// data needed to undo
typedef struct {
	Vec2f oldPos;
} UndoTranslateChip;

typedef struct {
	ActionType type;
	u32 ID;
	union {
		UndoTranslateChip translate;
	} data;
} Undo;