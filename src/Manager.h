#ifndef MANAGER_H
#define MANAGER_H

//OpenGl wrangler
#include "../lib/includes/GL/glew.h"
//WINDOW HANDLER
#include "../lib/includes/GLFW/glfw3.h"
//MATH LIBRARY
#include "../lib/includes/cglm/cglm.h"
#include "../lib/includes/cglm/vec3.h"
#include "../lib/includes/cglm/vec4.h"
#include "../lib/includes/cglm/mat4.h"
#include "../lib/includes/cglm/affine.h"
#include "../lib/includes/cglm/common.h"
#include "../lib/includes/cglm/types.h"
//IMAGELOADER
//#include "lodepng.h"
#include <string>
#include <iostream>

#include "Paint.h"
#include "../lib/includes/imgui.h"
#include "../lib/includes/imgui_impl_glfw.h"
#include "../lib/includes/imgui_impl_opengl3.h"

class Manager
{
private:
  GLFWwindow* m_window;
  void Init();
  void InitMatrcies();
  void CreateWindow(int width, int height, std::string window_name);
  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
  static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
  mat4 ortho_matrix;
  double m_x,m_y;
public:
  Manager();
  ~Manager();

  static Paint* paint;

  void GUI();
  void Loop();
  void Update(float dt);
  void Render();
};

#endif
