#pragma once
#include "constants.h"

typedef struct {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} Color;

typedef struct {
	float x;
	float y;
} Vec2;

Color newColor(u8 r, u8 g, u8 b, u8 a);
Vec2 newVec2(float x, float y);

Vec2 translatedVec2(Vec2 a, Vec2 b);
u8 collideABB(Vec2 a_pos, Vec2 b_pos, Vec2 b_size);
u8 collideAABB(Vec2 a_pos, Vec2 a_size, Vec2 b_pos, Vec2 b_size);
