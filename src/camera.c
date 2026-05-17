#include "camera.h"

// to account for camera, zoom, offset etc
Vec2 world2screenVec2(Camera camera, Vec2 a) {
	Vec2 b = subtractVec2(a, camera.position);
	b.y *= -1;
	Vec2 out = scaleVec2(b, camera.zoom);
	out.x += camera.viewport.x / 2;
	out.y += camera.viewport.y / 2;
	return out;
}
