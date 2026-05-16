#pragma once
#include "unit.h"

typedef struct {
	Vec2 oldPosition;
	Vec2 position;
	float zoom;

	Vec2 viewport;
} Camera;

Vec2 world2screenVec2(Camera camera, Vec2 a);
