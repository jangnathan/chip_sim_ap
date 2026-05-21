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
} Vec2f;

typedef struct {
	i32 x;
	i32 y;
} Vec2i;

Color newColor(u8 r, u8 g, u8 b, u8 a);
Vec2f newVec2f(float x, float y);
Vec2i newVec2i(i32 x, i32 y);
Vec2f vec2ItoF(Vec2i vec2);

Vec2f translateVec2f(Vec2f a, Vec2f b);
Vec2f subtractVec2f(Vec2f a, Vec2f b);
Vec2f scaleVec2f(Vec2f a, float scale);

Vec2i translateVec2i(Vec2i a, Vec2i b);
Vec2i subtractVec2i(Vec2i a, Vec2i b);
Vec2i scaleVec2i(Vec2i a, float scale);

u8 collideABB(Vec2i a_pos, Vec2i b_pos, Vec2i b_size);
u8 collideAABB(Vec2i a_pos, Vec2i a_size, Vec2i b_pos, Vec2i b_size);

u8 worldCollideABB(Vec2f a_pos, Vec2f b_pos, Vec2f b_size);
u8 worldCollideAABB(Vec2f a_pos, Vec2f a_size, Vec2f b_pos, Vec2f b_size);
