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

typedef struct { // top right bottom left
	i32 t;
	i32 r;
	i32 b;
	i32 l;
} Vec4i;

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

// comparing
u8 equalVec2i(Vec2i a, Vec2i b);
u8 isNullVec2i(Vec2i a);

u8 collideABB(Vec2i a_pos, Vec2i b_pos, Vec2i b_size);
u8 collideAABB(Vec2i a_pos, Vec2i a_size, Vec2i b_pos, Vec2i b_size);

u8 worldCollideABB(Vec2f a_pos, Vec2f b_pos, Vec2f b_size);
u8 worldCollideAABB(Vec2f a_pos, Vec2f a_size, Vec2f b_pos, Vec2f b_size);

Vec4i newVec4i(i32 t, i32 r, i32 b, i32 l);
u8 isNullVec4i(Vec4i a);
