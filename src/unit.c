#include "unit.h"

Color newColor(u8 r, u8 g, u8 b, u8 a) {
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;

	return color;
}

Vec2f newVec2f(float x, float y) {
	Vec2f vec2;
	vec2.x = x;
	vec2.y = y;

	return vec2;
}

Vec2i newVec2i(i32 x, i32 y) {
	Vec2i vec2;
	vec2.x = x;
	vec2.y = y;

	return vec2;
}

Vec2f vec2ItoF(Vec2i vec2i) {
	Vec2f vec2f;
	vec2f.x = (float)vec2i.x;
	vec2f.y = (float)vec2i.y;
	return vec2f;
}

// vec2f 
Vec2f translateVec2f(Vec2f a, Vec2f b) {
	return newVec2f(a.x + b.x, a.y + b.y);
}
Vec2f subtractVec2f(Vec2f a, Vec2f b) {
	return newVec2f(a.x - b.x, a.y - b.y);
}
Vec2f scaleVec2f(Vec2f a, float scale) {
	return newVec2f(a.x * scale, a.y * scale);
}
// vec2i
Vec2i translateVec2i(Vec2i a, Vec2i b) {
	return newVec2i(a.x + b.x, a.y + b.y);
}
Vec2i subtractVec2i(Vec2i a, Vec2i b) {
	return newVec2i(a.x - b.x, a.y - b.y);
}
Vec2i scaleVec2i(Vec2i a, float scale) {
	return newVec2i(a.x * scale, a.y * scale);
}

u8 collideABB(Vec2i a_pos, Vec2i b_pos, Vec2i b_size) {
	if (a_pos.x >= b_pos.x && a_pos.x <= b_pos.x + b_size.x &&
		 a_pos.y >= b_pos.y && a_pos.y <= b_pos.y + b_size.y)
		return 1;

	return 0;
}
u8 collideAABB(Vec2i a_pos, Vec2i a_size, Vec2i b_pos, Vec2i b_size) {
	if (a_pos.x + a_size.x >= b_pos.x && a_pos.x <= b_pos.x + b_size.x &&
		a_pos.y <= b_pos.y + b_size.y && a_pos.y + a_size.y >= b_pos.y) {
		return 1;
	}
	return 0;
}

u8 worldCollideABB(Vec2f a_pos, Vec2f b_pos, Vec2f b_size) {
	if (a_pos.x * 2 >= b_pos.x * 2 - b_size.x && a_pos.x * 2 <= b_pos.x * 2 + b_size.x &&
		 a_pos.y * 2 >= b_pos.y * 2- b_size.y && a_pos.y * 2 <= b_pos.y * 2 + b_size.y)
		return 1;

	return 0;
}
u8 worldCollideAABB(Vec2f a_pos, Vec2f a_size, Vec2f b_pos, Vec2f b_size) {
	if (a_pos.x * 2 + a_size.x >= b_pos.x * 2 - b_size.x && a_pos.x * 2 - a_size.x <= b_pos.x * 2 + b_size.x &&
		a_pos.y * 2 - a_size.y <= b_pos.y * 2 + b_size.y && a_pos.y * 2+ a_size.y >= b_pos.y * 2- b_size.y) {
		return 1;
	}
	return 0;
}
