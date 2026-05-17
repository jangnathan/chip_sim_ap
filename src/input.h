#pragma once
#include "ui/ui.h"
#include "unit.h"
#include <SDL3/SDL.h>

typedef struct {
	SDL_Cursor *cursorDefault;
	SDL_Cursor *cursorMove;
	Vec2 position;
	Vec2 oldCenterPosition;
	Vec2 centerPosition;

	u8 leftClick;
	u8 rightClick;
	u8 leftKeyHeld;
} Mouse;

typedef struct {
	Mouse mouse;
} Input;

void resetInput(Input *input);
void closeInput(Input *input);
