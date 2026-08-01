#include "unit.h"
#include <math.h>

Color newColor(u8 r, u8 g, u8 b, u8 a) {
  Color color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.a = a;

  return color;
}

u8 equalColor(Color a, Color b) {
  if ((a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a)) {
    return 1;
  }
  return 0;
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

Vec2i vec2FtoI(Vec2f vec2f) {
  Vec2i vec2i;
  vec2i.x = (u32)vec2f.x;
  vec2i.y = (u32)vec2f.y;
  return vec2i;
}

// vec2f
Vec2f translateVec2f(Vec2f a, Vec2f b) {
  return newVec2f(a.x + b.x, a.y + b.y);
}
Vec2f subtractVec2f(Vec2f a, Vec2f b) { return newVec2f(a.x - b.x, a.y - b.y); }
Vec2f scaleVec2f(Vec2f a, float scale) {
  return newVec2f(a.x * scale, a.y * scale);
}
Vec2f squareVec2f(Vec2f a) { return newVec2f(a.x * a.x, a.y * a.y); }
float sumVec2f(Vec2f a) { return a.x + a.y; }

// vec2i
Vec2i translateVec2i(Vec2i a, Vec2i b) {
  return newVec2i(a.x + b.x, a.y + b.y);
}
Vec2i subtractVec2i(Vec2i a, Vec2i b) { return newVec2i(a.x - b.x, a.y - b.y); }
Vec2i scaleVec2i(Vec2i a, float scale) {
  return newVec2i(a.x * scale, a.y * scale);
}

u8 equalVec2i(Vec2i a, Vec2i b) { return (a.x == b.x) && (a.y == b.y); }
u8 isNullVec2i(Vec2i a) { return a.x == 0 && a.y == 0; }

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

u8 cartesianCollideABB(Vec2i a_pos, Vec2i b_pos, Vec2i b_size) {
  if (a_pos.x * 2 >= b_pos.x * 2 - b_size.x &&
      a_pos.x * 2 <= b_pos.x * 2 + b_size.x &&
      a_pos.y * 2 >= b_pos.y * 2 - b_size.y &&
      a_pos.y * 2 <= b_pos.y * 2 + b_size.y)
    return 1;

  return 0;
}
u8 cartesianCollideAABB(Vec2f a_pos, Vec2f a_size, Vec2f b_pos, Vec2f b_size) {
  if (a_pos.x * 2 + a_size.x >= b_pos.x * 2 - b_size.x &&
      a_pos.x * 2 - a_size.x <= b_pos.x * 2 + b_size.x &&
      a_pos.y * 2 - a_size.y <= b_pos.y * 2 + b_size.y &&
      a_pos.y * 2 + a_size.y >= b_pos.y * 2 - b_size.y) {
    return 1;
  }
  return 0;
}

u8 cartesianCollideLine(Vec2f p_pos, Vec2f a_pos, Vec2f b_pos,
                        float thickness) {
  Vec2f ab = subtractVec2f(b_pos, a_pos);
  Vec2f ap = subtractVec2f(p_pos, a_pos);

  // equation of a line that goes through a1 b1, x and y are any points on the
  // line (by - ay)(x - ax) - (bx - ax)(y - ay) = 0

  // find closest line by using line perpendicular to line _ab_ that goes
  // through point p (by - ay)(x - px) + (ax - bx)(y - py) = 0

  // solve for x and y
  /*
  1. ab.y * (x - a.x) - ab.x * (y - a.y) = 0
  2. ab.x * (x - p.x) + ab.y * (y - p.y) = 0

  get y from 1
  better suited if ab.x > ab.y to prevent overflow
  y = (ab.y * (x - a.x)) / ab.x + a.y

  sub into 2
  ab.x * (x - p.x) + ab.y * ((ab.y * (x - a.x)) / ab.x + a.y - p.y) = 0

  = (ab.x + (ab.y * ab.y) / ab.x) * x +
  (ab.x * -p.x + ab.y * (ab.y * -a.x / ab.x + a.y - p.y)) = 0
  x = - constant / x coefficient

  y = (ab.y * (x - a.x)) / ab.x + a.y
  */

  float x, y;

  if (ab.x == 0) {
    x = a_pos.x;
    y = p_pos.y;
  } else if (ab.y == 0) {
    x = p_pos.x;
    y = a_pos.y;
  } else if (abs(ab.x) > abs(ab.y)) {
    float x_coefficient = ab.x + (ab.y * ab.y) / ab.x;
    float constant =
        ab.x * -p_pos.x + ab.y * (ab.y * -a_pos.x / ab.x + a_pos.y - p_pos.y);
    x = -constant / x_coefficient;
    y = (ab.y * (x - a_pos.x)) / ab.x + a_pos.y;

    if (p_pos.x < fminf(a_pos.x, b_pos.x) ||
        p_pos.x > fmaxf(a_pos.x, b_pos.x)) {
      return 0;
    }
  } else {
    float y_coefficient = ab.y + (ab.x * ab.x) / ab.y;
    float constant =
        ab.y * -p_pos.y + ab.x * (ab.x * -a_pos.y / ab.y + a_pos.x - p_pos.x);
    y = -constant / y_coefficient;
    x = (ab.x * (y - a_pos.y)) / ab.y + a_pos.x;

    if (p_pos.y < fminf(a_pos.y, b_pos.y) ||
        p_pos.y > fmaxf(a_pos.y, b_pos.y)) {
      return 0;
    }
  }

  // distance between point c and point p must be less than thickness
  Vec2f c_pos = newVec2f(x, y);
  Vec2f cp = subtractVec2f(p_pos, c_pos);

  if (sumVec2f(squareVec2f(cp)) <= thickness * thickness) {
    return 1;
  }
  return 0;
}

Vec4i newVec4i(i32 t, i32 r, i32 b, i32 l) {
  Vec4i rect;
  rect.t = t;
  rect.r = r;
  rect.b = b;
  rect.l = l;
  return rect;
}
u8 isNullVec4i(Vec4i a) { return a.t == 0 && a.r == 0 && a.b == 0 && a.l == 0; }
