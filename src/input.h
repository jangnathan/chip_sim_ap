#pragma once
#include "ui/ui.h"
#include "unit.h"
#include <SDL3/SDL.h>

typedef struct {
	Vec2 position;
	Vec2 size;
} Hitbox;

typedef struct {
	SDL_Cursor *cursorDefault;
	SDL_Cursor *cursorMove;
	Vec2 oldPosition;
	Vec2 position;
	Vec2 centerPosition;

	u8 leftClick;
	u8 rightClick;
	u8 leftKeyHeld;
} Mouse;

typedef struct {
	Mouse mouse;
} Input;

u8 hitboxClicked(Mouse *mouse, Hitbox hitbox);
void resetInput(Input *input);
void closeInput(Input *input);
