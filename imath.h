
#ifndef IMATH_H
#define IMATH_H

const float EPSILON = 0.0001f;

struct Vec2 {
  float x;
  float y;

  Vec2(float x = 0.0f, float y = 0.0f);
  void set(float x, float y);

  Vec2 operator-() const;
  Vec2 operator*(float s) const;
  Vec2 operator/(float s) const;
  void operator*=(float s);
  Vec2 operator+(const Vec2& rhs) const;
  Vec2 operator+(float s) const;
  void operator+=(const Vec2& rhs);
  Vec2 operator-(const Vec2& rhs) const;
  void operator-=(const Vec2& rhs);

  float len_sqr() const;
  float len() const;
  void rotate_self(float radians);
  Vec2 rotate(float radians) const;
  void normalize_self();
  Vec2 normalize() const;
};

Vec2 operator*(float s, const Vec2& v);
float dot(const Vec2& a, const Vec2& b);
float dist_sqr(const Vec2& a, const Vec2& b);
float projection_on(const Vec2& v, const Vec2& axis);
bool is_equal_eps(float a, float b);
float clamp(float min, float max, float a);
float random(float l, float h);

#endif // IMATH_H

// vim: set tabstop=2 shiftwidth=2 softtabstop=2 expandtab:
