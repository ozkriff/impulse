
#include <cstdio>
#include <GL/glut.h>
#include "scene.h"

// glut requires use of global state, sorry
Scene g_scene;
float g_new_circle_size = 3.0f;

static Vec2 screen_to_scene(int x, int y) {
  return Vec2(x /= 10.0f, y /= 10.0f);
}

static void mouse(int button, int state, int x, int y) {
  const Vec2 pos = screen_to_scene(x, y);
  if (state == GLUT_DOWN) {
    switch (button) {
      case GLUT_LEFT_BUTTON: {
        g_scene.add_circle(g_new_circle_size, pos);
        break;
      }
      case GLUT_RIGHT_BUTTON: {
        break;
      }
    }
  }
}

static void keyboard(unsigned char key, int /*x*/, int /*y*/) {
  switch (key) {
    case 'q': {
      exit(0);
      break;
    }
    case 'f': {
      g_scene.set_frame_stepping(!g_scene.frame_stepping());
      break;
    }
    case ' ': {
      g_scene.set_frame_stepping(true);
      g_scene.set_can_step(true);
      break;
    }
    case 'd': {
      g_scene.clear_circles();
      break;
    }
    case '+':
    case '=':
    {
      g_new_circle_size = clamp(1.0f, 10.0f, g_new_circle_size + 0.5f);
      printf("new circle size = %f\n", g_new_circle_size);
      break;
    }
    case '-': {
      g_new_circle_size = clamp(1.0f, 10.0f, g_new_circle_size - 0.5f);
      printf("new circle size = %f\n", g_new_circle_size);
      break;
    }
  }
}

static void mainloop() {
  g_scene.step();
  g_scene.draw();
}

static void init_camera() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(SCENE_MIN.x, SCENE_MAX.x, SCENE_MAX.y, SCENE_MIN.y);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

static void init_context(int ac, char** av) {
  glutInit(&ac, av);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(800, 600);
  glutCreateWindow("PhyEngine");
}

static void init_callbacks() {
  glutDisplayFunc(mainloop);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutIdleFunc(mainloop);
}

int main(int ac, char** av) {
  srand(1);
  init_context(ac, av);
  init_callbacks();
  init_camera();
  glutMainLoop();
  return 0;
}

// vim: set tabstop=2 shiftwidth=2 softtabstop=2 expandtab:
