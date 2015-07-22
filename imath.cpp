
#include "imath.h"
#include <cassert>
#include <algorithm>

Vec2::Vec2(float x, float y)
  : x(x),
    y(y)
{}

void Vec2::set(float x, float y) {
  x = x;
  y = y;
}

Vec2 Vec2::operator-() const {
  return Vec2(-x, -y);
}

Vec2 Vec2::operator*(float s) const {
  return Vec2(x * s, y * s);
}

Vec2 Vec2::operator/(float s) const {
  return Vec2(x / s, y / s);
}

void Vec2::operator*=(float s) {
  x *= s;
  y *= s;
}

Vec2 Vec2::operator+(const Vec2& rhs) const {
  return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator+(float s) const {
  return Vec2(x + s, y + s);
}

void Vec2::operator+=(const Vec2& rhs) {
  x += rhs.x;
  y += rhs.y;
}

Vec2 Vec2::operator-(const Vec2& rhs) const {
  return Vec2(x - rhs.x, y - rhs.y);
}

void Vec2::operator-=(const Vec2& rhs) {
  x -= rhs.x;
  y -= rhs.y;
}

float Vec2::len_sqr() const {
  return x * x + y * y;
}

float Vec2::len() const {
  return std::sqrt(x * x + y * y);
}

void Vec2::rotate_self(float radians) {
  const float c = std::cos(radians);
  const float s = std::sin(radians);
  const float xp = x * c - y * s;
  const float yp = x * s + y * c;
  x = xp;
  y = yp;
}

Vec2 Vec2::rotate(float radians) const {
  Vec2 v = *this;
  v.rotate_self(radians);
  return v;
}

void Vec2::normalize_self() {
  const float l = len();
  if (l > EPSILON) {
    const float inv_l = 1.0f / l;
    x *= inv_l;
    y *= inv_l;
  }
}

Vec2 Vec2::normalize() const {
  Vec2 v = *this;
  v.normalize_self();
  return v;
}

Vec2 operator*(float s, const Vec2& v) {
  return Vec2(s * v.x, s * v.y);
}

float dot(const Vec2& a, const Vec2& b) {
  return a.x * b.x + a.y * b.y;
}

float dist_sqr(const Vec2& a, const Vec2& b) {
  const Vec2 c = a - b;
  return dot(c, c);
}

float projection_on(const Vec2& v, const Vec2& axis) {
  return dot(v, axis.normalize());
}

bool is_equal_eps(float a, float b) {
  return std::abs(a - b) <= EPSILON;
}

float clamp(float min, float max, float a) {
  if (a < min) {
    return min;
  }
  if (a > max) {
    return max;
  }
  return a;
}

float random(float l, float h) {
  float a = (float)rand();
  a /= RAND_MAX;
  a = (h - l) * a + l;
  return a;
}

// vim: set tabstop=2 shiftwidth=2 softtabstop=2 expandtab:
