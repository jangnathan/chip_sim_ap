#pragma once
#include "unit.h"
#include <SDL3/SDL.h>

typedef struct {
	SDL_Cursor *cursorDefault; // cursor icons
	SDL_Cursor *cursorMove;
	Vec2i position;
	Vec2i oldCenterPosition;
	Vec2i centerPosition;

	u8 leftClick;
	u8 leftHeld;
	u8 leftDown;
	u8 rightClick;
} Mouse;

typedef struct {
	Mouse mouse;

	u16 focusedElementID;
} Input;

void initInput(Input *input);
void updateInput(Input *input);
void closeInput(Input *input);
