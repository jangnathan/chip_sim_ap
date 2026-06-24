#pragma once
#include "unit.h"

typedef struct {
	Vec2f oldPosition;
	Vec2f position;
	float zoom;

	Vec2i viewportSize;
	Vec2i viewportPos;
} Camera;

Vec2i world2screenVec2i(Camera camera, Vec2f a);
