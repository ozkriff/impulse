#ifndef SCENE_H
#define SCENE_H

#include <cstddef>
#include "imath.h"

const int MAX_LINES_COUNT = 16;
const int MAX_CIRCLES_COUNT = 256;
const int MAX_COLLISIONS_COUNT = 1024;
const Vec2 SCENE_MIN(0, 0);
const Vec2 SCENE_MAX(80, 60);

struct Line {
  Vec2 a, b;
};

struct Circle {
  float radius;
  Vec2 position;
  Vec2 velocity;
  float inverse_mass;

  void set_static();
  bool is_static() const;
};

struct Collision {
  Circle* a;
  Circle* b;
  float penetration;
  Vec2 normal;
  bool is_collision_with_line; // TODO: fix this

  Collision(
    Circle* a = NULL,
    Circle* b = NULL,
    float penetration = 0.0f,
    const Vec2& normal = Vec2(),
    bool is_collision_with_line = false
  )
    : a(a),
      b(b),
      penetration(penetration),
      normal(normal),
      is_collision_with_line(is_collision_with_line)
  {}
};

class Scene {
public:

  Scene();

  void draw() const;
  void draw_line(const Line& line) const;
  void draw_circle(const Circle& circle) const;

  void step();

  void add_circle(float radius, const Vec2& position);
  void delete_circle(int index);
  void clear_circles();
  void add_line(const Vec2& a, const Vec2& b);
  void add_collision(const Collision& collision);
  void clear_collisions();

  bool frame_stepping() const;
  void set_frame_stepping(bool value);
  void set_can_step(bool value);

private:
  void fixed_step();
  void integrate_forces();
  void integrate_velocity();
  void detect_circle_to_circle_collisions();
  void detect_circle_to_line_collisions();
  void collisions_resolution();
  void penetration_correction();
  void gc();

  int m_circles_count;
  Circle m_circles[MAX_CIRCLES_COUNT];
  int m_lines_count;
  Line m_lines[MAX_LINES_COUNT];
  int m_collisions_count;
  Collision m_collisions[MAX_COLLISIONS_COUNT];

  float m_accumulator;
  int m_start_time;
  bool m_frame_stepping;
  bool m_can_step;
};

#endif // SCENE_H

// vim: set tabstop=2 shiftwidth=2 softtabstop=2 expandtab:
