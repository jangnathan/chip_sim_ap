#include "unit.h"

Color newColor(u8 r, u8 g, u8 b, u8 a) {
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;

	return color;
}

u8 collideAABB(Vec2 a_pos, Vec2 a_size, Vec2 b_pos, Vec2 b_size) {
	if (a_pos.x + a_size.x >= b_pos.x && a_pos.x <= b_pos.x + b_size.x &&
		a_pos.y <= b_pos.y + b_size.y && a_pos.y + a_size.y >= b_pos.y) {
		return 1;
	}
	return 0;
}
