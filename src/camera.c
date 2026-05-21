#include "camera.h"

// to account for camera, zoom, offset etc
Vec2i world2screenVec2i(Camera camera, Vec2f in) {
	Vec2i cameraU32 = newVec2i((i32)camera.position.x, (i32)camera.position.y);
	Vec2i a = newVec2i((i32)in.x, (i32)in.y);

	Vec2i b = subtractVec2i(a, cameraU32);
	b.y *= -1;

	Vec2i out = scaleVec2i(b, camera.zoom);
	out.x += camera.viewportSize.x / 2;
	out.y += camera.viewportSize.y / 2;

	out.x += camera.viewportPos.x;
	out.y += camera.viewportPos.y;
	return out;
}
