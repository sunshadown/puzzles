#include "Manager.h"

Paint* Manager::paint(nullptr);

Manager::Manager()
{
  Init();
  InitMatrcies();
  Loop();
}

Manager::~Manager()
{
  glfwDestroyWindow(m_window);
  glfwTerminate();
}

void Manager::Init()
{
  if (!glfwInit())exit(EXIT_FAILURE);

  //glfwWindowHint(GLFW_MAXIMIZED,GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE,GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES,GLFW_DONT_CARE);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  CreateWindow(800, 600, "app");
  glfwMakeContextCurrent(m_window);

  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    exit(EXIT_FAILURE);
  }
  glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
  glfwSetCursorPosCallback(m_window, cursor_position_callback);
  glfwSetMouseButtonCallback(m_window, mouse_button_callback);
  glViewport(0, 0, 800, 600);
  glEnable(GL_DEPTH_TEST);
  glfwSwapInterval(1);
  glClearColor(0.2f,0.6f,1.0f,1.0f);
}

void Manager::InitMatrcies()
{
  glm_mat4_identity(ortho_matrix);
  glm_ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f, ortho_matrix);
}

void Manager::CreateWindow(int width, int height, std::string window_name)
{
  m_window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);
  if (!m_window)
  {
    glfwTerminate();
    std::cout << "Cant initialize window " << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Manager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void Manager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
  {
    if(Manager::paint == nullptr)return;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    ypos = 600 - ypos;
    Manager::paint->CheckTile((size_t)xpos, (size_t)ypos);
  }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
      if(Manager::paint == nullptr)return;
      Manager::paint->flag_clear = !Manager::paint->flag_clear;
    }
}

void Manager::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  //m_x = xpos;
  //m_y = ypos;
}

void Manager::Loop()
{
  Manager::paint = new Paint();

  double time = glfwGetTime();
  float last_time = time;

  while (!glfwWindowShouldClose(m_window))
  {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    float dt = time - last_time;
    last_time = time;
    Update(dt);
    Render();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }
}

void Manager::Update(float dt)
{

}

void Manager::Render()
{
  paint->Render(ortho_matrix);
}
