#include "scene.h"
#include <algorithm>
#include <cassert>
#include <GL/glut.h>
#include "imath.h"

const float GRAVITY_SCALE = 5.0f;
const Vec2 GRAVITY(0, 10.0f * GRAVITY_SCALE);
const int FPS = 60.0f * 6;
const float DT = 1.0f / FPS;
const float DENSITY = 2.0f;
const float RESTITUTION = 0.6f;
const int CIRCLE_SEGMENTS_COUNT = 16;

int elapsed_time() {
  return glutGet(GLUT_ELAPSED_TIME);
}

void Circle::set_static() {
  inverse_mass = 0.0f;
}

bool Circle::is_static() const {
  return inverse_mass == 0.0f;
}

Scene::Scene() 
  : m_circles_count(0),
    m_lines_count(0),
    m_collisions_count(0),
    m_accumulator(0.0f),
    m_start_time(0),
    m_frame_stepping(false),
    m_can_step(false)
{
  Vec2 v0(0, 20);
  Vec2 v1(15, 54);
  Vec2 v2(65, 58);
  Vec2 v3(SCENE_MAX.x, 20);
  add_line(v0, v1);
  add_line(v1, v2);
  add_line(v2, v3);
}

void Scene::draw() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (int i = 0; i < m_circles_count; ++i) {
    draw_circle(m_circles[i]);
  }
  for (int i = 0; i < m_lines_count; ++i) {
    draw_line(m_lines[i]);
  }
  glutSwapBuffers();
}

void Scene::draw_line(const Line& line) const {
  glColor3f(1, 1, 1);
  glBegin(GL_LINES);
  glVertex2f(line.a.x, line.a.y);
  glVertex2f(line.b.x, line.b.y);
  glEnd();
}

void Scene::draw_circle(const Circle& circle) const {
  glColor3f(1, 1, 1);
  glBegin(GL_LINE_LOOP);
  float theta = 0.0f;
  float inc = M_PI * 2.0f / CIRCLE_SEGMENTS_COUNT;
  for (int i = 0; i < CIRCLE_SEGMENTS_COUNT; ++i) {
    theta += inc;
    Vec2 p(std::cos(theta), std::sin(theta));
    p *= circle.radius;
    p += circle.position;
    glVertex2f(p.x, p.y);
  }
  glEnd();
}

void Scene::step() {
  m_accumulator += (elapsed_time() - m_start_time) / 1000.0f;
  m_start_time = elapsed_time();
  m_accumulator = clamp(0.0f, 0.1f, m_accumulator);
  while (m_accumulator >= DT) {
    if (!m_frame_stepping) {
      fixed_step();
    } else {
      if (m_can_step) {
        fixed_step();
        m_can_step = false;
      }
    }
    m_accumulator -= DT;
  }
}

void Scene::integrate_forces() {
  for (int i = 0; i < m_circles_count; ++i) {
    Circle& circle = m_circles[i];
    if (circle.is_static()) {
      continue;
    }
    circle.velocity += (Vec2(circle.inverse_mass) + GRAVITY) * DT;
  }
}

void Scene::detect_circle_to_circle_collisions() {
  for (int i = 0; i < m_circles_count; ++i) {
    Circle& a = m_circles[i];
    for (int j = i + 1; j < m_circles_count; ++j) {
      if (i == j) {
        continue;
      }
      Circle& b = m_circles[j];
      if (a.is_static() && b.is_static()) {
        continue;
      }
      const Vec2 normal = (a.position - b.position);
      const float distance = normal.len();
      const float penetration = (a.radius + b.radius) - distance;
      if (penetration > 0.0f) {
        add_collision(Collision(&a, &b, penetration, normal.normalize()));
      }
    }
  }
}

void Scene::detect_circle_to_line_collisions() {
  for (int i = 0; i < m_circles_count; ++i) {
    Circle& circle = m_circles[i];
    if (circle.is_static()) {
      continue;
    }
    for (int j = 0; j < m_lines_count; ++j) {
      const Line& line = m_lines[j];
      const Vec2 segment = (line.b - line.a);
      const Vec2 normal = segment.rotate(M_PI * -0.5f).normalize();
      const Vec2 perpendicular = circle.position - line.a;
      const float on_normal = projection_on(perpendicular, normal);
      const float on_line = projection_on(perpendicular, segment);
      if (on_line < -circle.radius
        || on_line > segment.len() + circle.radius)
      {
        continue;
      }
      if (on_normal > 0 && (on_normal) <= circle.radius) {
        const float penetration = circle.radius - on_normal;
        add_collision(Collision(
          &circle, NULL, penetration, normal.normalize(), true));
      }
    }
  }
}

void Scene::collisions_resolution() {
  for (int i = 0; i < m_collisions_count; ++i) {
    Collision& c = m_collisions[i];
    Circle& a = *c.a;
    const Vec2 b_vel = c.is_collision_with_line ? Vec2() : c.b->velocity;
    const Vec2 relative_velocity = a.velocity - b_vel;
    const float velocity_along_normal = dot(relative_velocity, c.normal);
    const bool is_velocities_separating = velocity_along_normal > 0;
    if (is_velocities_separating) {
      continue;
    }
    const float b_inv_mass = c.is_collision_with_line ? 0.0f : c.b->inverse_mass;
    const float impulse_scalar = (-(1 + RESTITUTION) * velocity_along_normal)
      / (a.inverse_mass + b_inv_mass);
    const Vec2 impulse = impulse_scalar * c.normal;
    a.velocity += a.inverse_mass * impulse;
    if (!c.is_collision_with_line) {
      Circle& b = *c.b;
      b.velocity -= b.inverse_mass * impulse;
    }
  }
}

void Scene::penetration_correction() {
  for (int i = 0; i < m_collisions_count; ++i) {
    Collision& c = m_collisions[i];
    Circle& a = *c.a;
    Circle& b = *c.b;
    const float percent = 0.2f; // 20% .. 80%
    const float slop = 0.01f; // 0.01 .. 0.1
    const float b_inverse_mass = c.is_collision_with_line ? 0.0f : b.inverse_mass;
    const float inv_mass_sum = a.inverse_mass + b_inverse_mass;
    const Vec2 correction = percent * c.normal
      * (std::max(c.penetration - slop, 0.0f) / inv_mass_sum);
    a.position += a.inverse_mass * correction;
    if (!c.is_collision_with_line) {
      b.position -= b.inverse_mass * correction;
    }
  }
}

void Scene::integrate_velocity() {
  for (int i = 0; i < m_circles_count; ++i) {
    Circle& circle = m_circles[i];
    circle.position += circle.velocity * DT;
  }
}

void Scene::gc() {
  for (int i = 0; i < m_circles_count; ++i) {
    Circle& circle = m_circles[i];
    if (circle.position.y > circle.radius + SCENE_MAX.y
      || circle.position.x < -circle.radius
      || circle.position.x > circle.radius + SCENE_MAX.x
    ) {
      printf("delete circle: %d\n", i);
      delete_circle(i);
    }
  }
}

void Scene::fixed_step() {
  integrate_forces();
  detect_circle_to_circle_collisions();
  detect_circle_to_line_collisions();
  collisions_resolution();
  integrate_velocity();
  penetration_correction();
  clear_collisions();
}

void Scene::add_collision(const Collision& collision) {
  if (m_collisions_count == MAX_COLLISIONS_COUNT) {
    printf("Collisions limit reached\n");
    return;
  }
  m_collisions[m_collisions_count] = collision;
  ++m_collisions_count;
}

void Scene::clear_collisions() {
  m_collisions_count = 0;
}

void Scene::add_circle(float radius, const Vec2& position) {
  if (m_circles_count == MAX_CIRCLES_COUNT) {
    printf("Circles limit reached\n");
    return;
  }
  Circle& circle = m_circles[m_circles_count];
  ++m_circles_count;
  assert(m_circles_count <= MAX_CIRCLES_COUNT);
  circle.radius = radius,
  circle.position = position;
  circle.velocity = Vec2();
  const float mass = M_PI * circle.radius * circle.radius * DENSITY;
  circle.inverse_mass = 1.0f / mass;
}

void Scene::delete_circle(int index) {
  assert(index >= 0 && index < m_circles_count);
  --m_circles_count;
  m_circles[index] = m_circles[m_circles_count];
}

void Scene::add_line(const Vec2& a, const Vec2& b) {
  m_lines[m_lines_count].a = a;
  m_lines[m_lines_count].b = b;
  ++m_lines_count;
}

void Scene::clear_circles() {
  m_circles_count = 0;
}

bool Scene::frame_stepping() const {
  return m_frame_stepping;
}

void Scene::set_frame_stepping(bool value) {
  m_frame_stepping = value;
}

void Scene::set_can_step(bool value) {
  m_can_step = value;
}

// vim: set tabstop=2 shiftwidth=2 softtabstop=2 expandtab:
