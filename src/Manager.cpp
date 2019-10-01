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
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

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
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
      //DEPRACATED
      //if(Manager::paint == nullptr)return;
      //Manager::paint->flag_clear = !Manager::paint->flag_clear;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
      if(Manager::paint == nullptr)return;
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      if(ypos < 0.1f || ypos > 600 || xpos < 0 || xpos > 800)return;
      ypos = 600 - ypos;
      Manager::paint->CheckTile((size_t)xpos, (size_t)ypos);
    }
}

void Manager::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  if(Manager::paint == nullptr)return;
  double xpos_temp = xpos, ypos_temp = ypos;
  //glfwGetCursorPos(window, &xpos, &ypos);
  ypos_temp = 600 - ypos;
  if(ypos_temp <= 0.1f || ypos_temp >= 600 || xpos_temp <= 0.1 || xpos_temp >= 800)return;
  Manager::paint->CheckTileFocus((size_t)xpos_temp, (size_t)ypos_temp);
}

void Manager::CreateCursor()
{
  std::vector<unsigned char> png_image; //the raw pixels
  unsigned width, height;

  //decode
  unsigned error = lodepng::decode(png_image, width, height, "./assets/images/cursor4.png");

  //if there's an error, display it
  if(error)
  {
    std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    return;
  }

  image.width = 64;
  image.height = 64;
  image.pixels = &png_image[0];

  cursor = glfwCreateCursor(&image, 0, 0);
  if(cursor == NULL)
  {
    std::cout << "Cant create cursor" << std::endl;
    return;
  }
  glfwSetCursor(m_window, cursor);
}

void Manager::GUI()
{
  static vec3 clear_color;
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Toolbox");
  if(!ImGui::IsWindowFocused())Manager::paint->can_draw = true;
  else Manager::paint->can_draw = false;

  ImVec2 window_pos = ImGui::GetWindowPos();
  ImVec2 window_size = ImGui::GetWindowSize();
  ImGuiIO& io = ImGui::GetIO();
  if((io.MousePos.x >= window_pos.x && io.MousePos.x <= window_pos.x + window_size.x) ||(io.MousePos.y >= window_pos.y && io.MousePos.y <= window_pos.y + window_size.y))
  {
    //Manager::paint->can_draw = false;
  }
  /*std::cout << "IO: " << io.MousePos.x << " " << io.MousePos.y << std::endl;
  std::cout << "X: " << window_pos.x << "--" << window_pos.x + window_size.x<< std::endl;
  std::cout << "Y: " << window_pos.y << "--" << window_pos.y + window_size.y<< std::endl;
  std::cout << "window_size: " << window_size.x << " " << window_size.y << std::endl;
  */
  ImGui::ColorEdit3("clear color", (float*)clear_color); // Edit 3 floats representing a color
  ImGui::Checkbox("Draw", &Manager::paint->flag_draw);
  ImGui::SameLine();
  ImGui::Checkbox("Clear", &Manager::paint->flag_clear);
  if (ImGui::Button("Clear Me"))Manager::paint->ClearAll();
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();

  // Rendering
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glm_vec3_copy(clear_color, Manager::paint->color);
}

void Manager::PuzzleGUI()
{
  // Start the Dear ImGui frame

  ImGuiIO& io = ImGui::GetIO();
  io.MouseDrawCursor = false;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Toolbox");


  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Text("Number of puzzles %d", paint->tiles.size());
  // Animate a simple progress bar
  ImGui::ProgressBar(score / paint->tiles.size(), ImVec2(0.0f,0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("Score");
  ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
  GLuint tex_id = paint->tiles.at(0).GetTexture();
  ImTextureID m_tex = (void*)(intptr_t)tex_id;
  ImGui::Image(m_tex, ImVec2(200, 100), ImVec2(0,0), ImVec2(1,1), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));
  ImGui::End();

  // Rendering
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Manager::Loop()
{

  Manager::paint = new Paint();
  paint->TileShuffle();
  std::thread focus_thread(Paint::TileFocusTimer, &paint->tiles, 0.1f, m_window);
  std::thread score_thread(Paint::TileScore, &paint->tiles, &score);

  //GUI///////////////////////
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(m_window, true);
  const char* glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version);
  CreateCursor();
  ////////////////////////////

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

  focus_thread.join();
  score_thread.join();
}

void Manager::Update(float dt)
{
  /*
  int state = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
  if (state == GLFW_PRESS)
  {
    if(Manager::paint == nullptr)return;
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    ypos = 600 - ypos;
    if(ypos < 0 || ypos > 600 || xpos < 0 || xpos > 800)return;
    Manager::paint->CheckTile((size_t)xpos, (size_t)ypos);
  }
  */
}

void Manager::Render()
{
  paint->Render(ortho_matrix);
  //GUI();
  PuzzleGUI();
}
