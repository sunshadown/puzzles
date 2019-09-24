#include "../lib/interface_lib.h"

static struct IsNEWUE* temp_ue = NULL;
static struct ChangeUE* state_ue = NULL;
static struct UEStates* ue_states = NULL;
static pthread_mutex_t lock;


void* InterfaceLoop(void* table)
{
  srand(time(0));
  InitExtensions();
  struct Frame* frame = CreateFrame(800,600,"app",3);
  WindowInit(frame);
  printf("is ready to go\n");
  WindowLoop(frame, (HashTable*)table);
  WindowClean(frame);

  return NULL;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void InitExtensions()
{
  if (!glfwInit())exit(EXIT_FAILURE);

  //glfwWindowHint(GLFW_MAXIMIZED,GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE,GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);
  glfwWindowHint(GLFW_SAMPLES,GLFW_DONT_CARE);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void InitGlWrangler()
{
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    exit(EXIT_FAILURE);
  }
}

struct Frame* CreateFrame(size_t window_size_x,size_t window_size_y,const char* window_name,size_t window_name_size)
{
  if(window_size_x <= 0 || window_size_y <= 0)return NULL;

  struct Frame* frame = malloc(sizeof(struct Frame));
  frame->window_size_x = window_size_x;
  frame->window_size_y = window_size_y;
  glm_mat4_identity(frame->ortho_matrix);
  glm_ortho(0, window_size_x, 0, window_size_y, -1.0f, 1.0f, frame->ortho_matrix);
  frame->window_name = malloc(window_max_name);
  memset(frame->window_name,0,window_max_name);
  if(strlen(window_name) > 0)memcpy(frame->window_name,window_name,window_name_size);
  else frame->window_name = "app";

  return frame;
}

void WindowInit(struct Frame* frame)
{
  frame->window = glfwCreateWindow(frame->window_size_x, frame->window_size_y, frame->window_name, NULL, NULL);
  if (!frame->window)
  {
    glfwTerminate();
    printf("Cant initialize window \n");
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(frame->window);
  InitGlWrangler();
  glViewport(0, 0, frame->window_size_x, frame->window_size_y);
  glfwSetFramebufferSizeCallback(frame->window, framebuffer_size_callback);
  glEnable(GL_DEPTH_TEST);
  glfwSwapInterval(1);
  glClearColor(0.7f,0.0f,0.8f,1.0f);
}

void WindowLoop(struct Frame* frame, HashTable* table)
{
  struct Model* model_bg= CreateModel();
  struct Model* model_wave = CreateModel();
  struct Model* model_enodeb = CreateModel();
  struct Model* model_ue = CreateModel();
  struct Model** models_seq = CreateWaveSequence(20, 0.1f);

  model_wave->size[0] = 5;
  model_wave->size[1] = 5;
  model_wave->position[0] = 420;
  model_wave->position[1] = 325;
  model_wave->m_depth = 0.2f;
  model_wave->timer = 10.0f;

  model_enodeb->size[0] = 250;
  model_enodeb->size[1] = 400;
  model_enodeb->position[0] = 300;
  model_enodeb->position[1] = 0;
  model_enodeb->m_depth = 0.1f;
  model_enodeb->rotate = 180.0f;


  model_ue->size[0] = 75;
  model_ue->size[1] = 100;
  model_ue->position[0] = 100;
  model_ue->position[1] = 0;
  model_ue->m_depth = 0.2f;
  model_ue->rotate = 180.0f;

  CreateBackground(model_bg,"./assets/images/background.png");
  CreateFixed(model_enodeb,"./assets/images/enodeb.png");
  CreateFixed(model_ue,"./assets/images/ue.png");
  CreateWave(model_wave);

  InitMutex();
  SetupIsUE();
  SetupUEStates();

  while (!glfwWindowShouldClose(frame->window))
  {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    DrawBackground(model_bg);
    DrawFixed(frame, model_enodeb);

    CheckIsUE(table);
    DrawUEs(table, frame);
    //DrawFixed(frame, model_ue);

    //DrawWave(frame, model_wave);
    DrawWaveSequence(frame, models_seq, 20);


    glfwSwapBuffers(frame->window);
    glfwPollEvents();
  }
}

void WindowClean(struct Frame* frame)
{
  ClearIsUE();
  ClearUEStates();
  glfwDestroyWindow(frame->window);
  glfwTerminate();
}

struct Uniform* LoadImage(const char* path, const char* uniform_name)
{
  unsigned error;
  unsigned char* image = 0;
  unsigned width, height;
  unsigned char* png = 0;
  size_t pngsize;

  error = lodepng_load_file(&png, &pngsize, path);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
  if(!error) error = lodepng_decode32(&image, &width, &height, png, pngsize);
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  free(png);

  struct Uniform* uniform = malloc(sizeof(struct Uniform));
  if(uniform == NULL)return NULL;
  struct Uniform temp_uniform = {.uniform_name = uniform_name};
  memcpy(uniform,&temp_uniform,sizeof(struct Uniform));

  glGenTextures(1,&uniform->uniform_id);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,uniform->uniform_id);

  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,width,height,0, GL_RGBA, GL_UNSIGNED_BYTE,image);

  free(image);
  return uniform;
}

char* LoadShader(const char* path, size_t buff_size)
{
  char* buff = malloc(buff_size);
  memset(buff,0,buff_size);
  buff[buff_size - 1] = '\0';
  int fd = open(path,O_RDONLY);
  if(fd == -1)
  {
    printf("%s \n",strerror(errno));
    return NULL;
  }
  size_t size = read(fd,buff,buff_size);
  if(size < 1)return NULL;

  return buff;
}

void AttachShader(char* shader, struct Model* model, int flag)
{
  GLuint sh;
  if(flag == 0)sh = glCreateShader(GL_VERTEX_SHADER);
  else sh = glCreateShader(GL_FRAGMENT_SHADER);

  if(shader == 0)
  {
    printf("AttachShader failed, cant create shader");
    return;
  }
  const GLchar* shader_source = shader;
  glShaderSource(sh, 1, &shader_source, 0);
  glCompileShader(sh);
  GLint succes = 0;
  glGetShaderiv(sh, GL_COMPILE_STATUS, &succes);
  if(succes == GL_FALSE)
  {
    GLint logSize = 0;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &logSize);
    char buff[logSize];
    glGetShaderInfoLog(sh,logSize, &logSize, buff);
    printf("Shader Compilation error flag: %d %s \n", flag, buff);
    return;
  }

  glAttachShader(model->program_id, sh);
}


void LinkShader(struct Model* model)
{
  glLinkProgram(model->program_id);
  GLint isLinked = 0;
  glGetProgramiv(model->program_id, GL_LINK_STATUS, (int*)&isLinked);
  if(isLinked == GL_FALSE)
  {
    GLint logSize = 0;
    glGetProgramiv(model->program_id, GL_INFO_LOG_LENGTH, &logSize);
    char buff[logSize];
    glGetProgramInfoLog(model->program_id, logSize, &logSize, buff);
    printf("Linking %s \n",buff);
  }
}

struct Uniform* CreateUniform(GLuint uniform_id, const char* uniform_name)
{
  struct Uniform* uniform = malloc(sizeof(struct Uniform));
  struct Uniform temp_uniform = {.uniform_name = uniform_name};
  memcpy(uniform,&temp_uniform,sizeof(struct Uniform));
  uniform->uniform_id = uniform_id;
  return uniform;
}

void ClearUniform(struct Uniform** uniform)
{
  if(*uniform == NULL)return;
  free(*uniform);
  *uniform = NULL;
}

struct Model* CreateModel()
{
  struct Model* model = malloc(sizeof(struct Model));
  glGenVertexArrays(1,&model->vao);
  glGenBuffers(1,&model->vbo);
  model->program_id = glCreateProgram();
  model->position[0] = 0.0f;
  model->position[1] = 0.0f;
  model->position[2] = 0.0f;
  model->size[0] = 1.0f;
  model->size[1] = 1.0f;
  model->size[2] = 1.0f;
  model->rotate = 0.0f;
  model->elements_size = 0;
  model->m_depth = 0.0f;
  model->number_of_instances = 0.0f;
  model->timer = 0.0f;
  model->timer_inc = 0.05f;
  model->counter = 0.0f;
  model->state = NULL;
  model->range= NULL;
  model->range_number = 0;
  model->blink = NULL;
  glm_mat4_identity(model->model_matrix);

  return model;
}

void GenerateFullScreen(struct Model* model)
{
  if(model == NULL)return;

  glBindVertexArray(model->vao);
  glBindBuffer(GL_ARRAY_BUFFER, model->vbo);

  float marray[] = {-1.0f,-1.0f,0.0f, //left-bottom
                   1.0f,-1.0f,0.0f, //right-bottom
                   -1.0f,1.0f,0.0f, //left-top
                   -1.0f,1.0f,0.0f, //left-top
                   1.0f,1.0f,0.0f, //right-top
                   1.0f,-1.0f,0.0f}; //right-bottom

  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*18, marray, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  model->elements_size = 6;

  glBindVertexArray(0);
}

void GenerateFixedSize(struct Model* model)
{
  if(model == NULL)return;

  glBindVertexArray(model->vao);
  glBindBuffer(GL_ARRAY_BUFFER, model->vbo);

  float marray[] = {
                // Pos      // Tex
                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,

                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f
        };

  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*24, marray, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  model->elements_size = 6;

  glBindVertexArray(0);
}

void GenerateWave(struct Model* model, size_t number_of_instances)
{
  if(model == NULL)return;

  glBindVertexArray(model->vao);
  glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
  float marray[] = {
                // Pos      // Tex
                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,

                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f
        };

  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*24, marray, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);


  vec3 particles[1000];
  for (size_t i = 0; i < number_of_instances; i++) {
    float angle = (float)i/(float)number_of_instances * 360.0f;
    float x = sinf(angle);
    float y = cosf(angle);
    particles[i][0] = x;
    particles[i][1] = y;
    particles[i][2] = 0.0f;
  }


  glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*number_of_instances, particles, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glVertexAttribDivisor(0,0);
  glVertexAttribDivisor(1,1);

  model->elements_size = 6;
  model->number_of_instances = number_of_instances;

  glBindVertexArray(0);
}

void CreateBackground(struct Model* model, const char* pathtoimage)
{
  char* vs = LoadShader("./assets/shaders/fullscreen.vs",1024);
  char* fs = LoadShader("./assets/shaders/fullscreen.fs",1024);
  AttachShader(vs, model, 0);
  AttachShader(fs, model, 1);
  LinkShader(model);
  GenerateFullScreen(model); //generate vrticies
  model->tex_uniform = LoadImage(pathtoimage,"background_uniform");
}

void CreateFixed(struct Model* model, const char* pathtoimage)
{
  char* vs = LoadShader("./assets/shaders/fixed.vs",5012);
  char* fs = LoadShader("./assets/shaders/fixed.fs",5012);
  AttachShader(vs, model, 0);
  AttachShader(fs, model, 1);
  LinkShader(model);
  GenerateFixedSize(model); //generate vrticies
  model->tex_uniform = LoadImage(pathtoimage,"fixed_uniform");
}

void CreateWave(struct Model* model)
{
  char* vs = LoadShader("./assets/shaders/wave.vs",1024);
  char* fs = LoadShader("./assets/shaders/wave.fs",1024);
  AttachShader(vs, model, 0);
  AttachShader(fs, model, 1);
  LinkShader(model);
  GenerateWave(model,1000);
}

struct Model** CreateWaveSequence(size_t number_of_models, float gap)
{
    struct Model** models = malloc(sizeof(struct Model*)*number_of_models);
    for (size_t i = 0; i < number_of_models; i++) {
      struct Model* model = CreateModel();
      char* vs = LoadShader("./assets/shaders/wave.vs",1024);
      char* fs = LoadShader("./assets/shaders/wave.fs",1024);
      AttachShader(vs, model, 0);
      AttachShader(fs, model, 1);
      LinkShader(model);
      GenerateWave(model,1000);
      model->size[0] = 5;
      model->size[1] = 5;
      model->position[0] = 420;
      model->position[1] = 325;
      model->m_depth = 0.3f;
      model->timer = 10.0f ;
      model->timer_inc = 0.01f + gap * (float)i / 30;//+ gap * (float)i;
      models[i] = model;
    }
    return models;
}

void DrawBackground(struct Model* model)
{
    glBindVertexArray(model->vao);
    glUseProgram(model->program_id);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,model->tex_uniform->uniform_id);

    glUniform1i(glGetUniformLocation(model->program_id,model->tex_uniform->uniform_name),0);


    glDrawArrays(GL_TRIANGLES, 0, model->elements_size);
    glBindVertexArray(0);
}

void DrawFixed(struct Frame* frame, struct Model* model)
{
  mat4 model_matrix;
  glm_mat4_identity(model_matrix);
  glm_translate(model_matrix, model->position);
  vec3 npos;
  npos[0] = model->size[0]*0.5f;
  npos[1] = model->size[1]*0.5f;
  npos[2] = 0.0f;
  vec3 pivot;
  pivot[0] = 0;
  pivot[1] = 0;
  pivot[2] = 1;

  glm_translate(model_matrix, npos);
  float radians = (model->rotate) *(M_PI/180.0f);
  glm_rotate(model_matrix, radians, pivot);

  npos[0] = model->size[0]*(-0.5f);
  npos[1] = model->size[1]*(-0.5f);
  glm_translate(model_matrix, npos);
  glm_scale(model_matrix, model->size);

  glBindVertexArray(model->vao);
  glUseProgram(model->program_id);

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,model->tex_uniform->uniform_id);

  glUniform1i(glGetUniformLocation(model->program_id,model->tex_uniform->uniform_name),0);
  glUniformMatrix4fv(glGetUniformLocation(model->program_id,"projection"), 1, GL_FALSE, frame->ortho_matrix[0]);
  glUniformMatrix4fv(glGetUniformLocation(model->program_id,"model"), 1, GL_FALSE, model_matrix[0]);
  glUniform1f(glGetUniformLocation(model->program_id,"m_depth"),model->m_depth);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glDrawArrays(GL_TRIANGLES, 0, model->elements_size);

  glDisable(GL_BLEND);
  glBindVertexArray(0);
}

void DrawWave(struct Frame* frame, struct Model* model)
{
  glBindVertexArray(model->vao);
  glUseProgram(model->program_id);

  mat4 model_matrix;
  glm_mat4_identity(model_matrix);
  glm_translate(model_matrix, model->position);
  glm_scale(model_matrix, model->size);

  glUniformMatrix4fv(glGetUniformLocation(model->program_id,"projection"), 1, GL_FALSE, frame->ortho_matrix[0]);
  glUniformMatrix4fv(glGetUniformLocation(model->program_id,"model"), 1, GL_FALSE, model_matrix[0]);
  glUniform1f(glGetUniformLocation(model->program_id,"m_depth"),model->m_depth);

  model->counter += model->timer_inc;

  if(model->counter >= model->timer)
  {
    model->counter = 0;
    model->timer_inc = sinf(model->timer_inc);
  }

  glUniform1f(glGetUniformLocation(model->program_id,"time"),model->counter);

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE,GL_ONE);

  glDrawArraysInstanced(GL_TRIANGLES, 0, model->elements_size, model->number_of_instances);

  glDisable(GL_BLEND);

  glBindVertexArray(0);
}

void DrawWaveSequence(struct Frame* frame, struct Model** models, size_t number_of_models)
{
  for (size_t i = 0; i < number_of_models; i++) {
    DrawWave(frame,models[i]);
  }
}

void DrawRange(struct Frame* frame, struct Model* model)
{
    for (size_t i = 0; i < model->range_number; i++) {
      DrawFixed(frame, model->range[i]);
    }
}

void DrawBlink(struct Frame* frame, struct Blink* blink)
{
  blink->interval += blink->time_inc/2.0f;

  if(blink->interval >= 1.0f)
  {
    blink->interval = 0.7f;
    blink->flag = !blink->flag;
  }

  if(blink->flag)
  {
    blink->model->size[0] += blink->interval;
    blink->model->size[1] += blink->interval;
    blink->model->position[0] -= blink->interval / 2.0f;
    blink->model->position[1] -= blink->interval / 2.0f;
  }
  else
  {
    blink->model->size[0] -= blink->interval;
    blink->model->size[1] -= blink->interval;
    blink->model->position[0] += blink->interval / 2.0f;
    blink->model->position[1] += blink->interval / 2.0f;
  }


  DrawFixed(frame, blink->model);
}

struct Model* CreateUE()
{
  struct Model* model_ue = CreateModel();
  model_ue->size[0] = 75;
  model_ue->size[1] = 100;
  model_ue->position[0] = (float)(rand()% 700) + 20.0f;
  model_ue->position[1] = (float)(rand()% 500) + 10.0f;
  model_ue->m_depth = 0.2f;
  model_ue->rotate = 180.0f;
  CreateFixed(model_ue,"./assets/images/ue.png");

  model_ue->state = CreateModel();
  model_ue->state->size[0] = 17;
  model_ue->state->size[1] = 25;
  model_ue->state->position[0] = model_ue->position[0] + 25.0f;
  model_ue->state->position[1] = model_ue->position[1] + 15.0f;
  model_ue->state->m_depth = 0.25f;
  model_ue->state->rotate = 180.0f;
  CreateFixed(model_ue->state,"./assets/images/ue.png");

  CreateRange(model_ue);

  model_ue->blink = malloc(sizeof(struct Blink));
  model_ue->blink->model = CreateModel();
  CreateFixed(model_ue->blink->model,"./assets/images/blink.png");
  model_ue->blink->interval = 5.0f;
  model_ue->blink->time_inc = 0.01f;
  model_ue->blink->model->size[0] = 100.0f;
  model_ue->blink->model->size[1] = 100.0f;
  model_ue->blink->model->position[0] = model_ue->position[0] - 10.0f;
  model_ue->blink->model->position[1] = model_ue->position[1] - 10.0f;
  model_ue->blink->model->m_depth = 0.17f;
  model_ue->blink->flag = true;
  glm_vec3_copy(model_ue->blink->model->position, model_ue->blink->position);

  return model_ue;
}

void CreateRange(struct Model* model)
{
    model->range_number = rand() % ue_max_range;
    model->range = malloc(sizeof(struct Model*)*model->range_number);

    for (size_t i = 0; i <  model->range_number; i++) {
      model->range[i] = malloc(sizeof(struct Model));
      model->range[i] = CreateModel();
      model->range[i]->size[0] = 5;
      model->range[i]->size[1] = 25 + (float)i*5.0f;
      model->range[i]->position[0] = model->position[0] + 55.0f + 8.0f * (float)i;//+ i*5.0f;
      model->range[i]->position[1] = model->position[1] + 5.0f;// + model->size[1] ;//+ 5.0f;
      model->range[i]->m_depth = 0.25f;
      model->range[i]->rotate = 180.0f;
      CreateFixed(model->range[i],"./assets/images/bar.png");
    }
}

HashTable* CreateHashTable()
{
  HashTable *table;
  if (hashtable_new(&table) != CC_OK) {
    // something went wrong
    printf("Creating hashtable failed");
    return NULL;
  }
  return table;
}

bool AddUEToHashTable(HashTable* table, void* key)
{
  struct Model* model = CreateUE();
  // Add key-value pair
  if (hashtable_add(table, key, (void*)model) != CC_OK) {
    // something went wrong
    printf("Can't add UE to hashtable");
    return false;
  }
  return true;
}

void AddUEToInterface(void* key)
{
  temp_ue->key = key;
  temp_ue->flag = true;
}

void RemoveUEFromInterface(HashTable* table, void* key)
{
  pthread_mutex_lock(&lock);

  void* model;
  if(hashtable_remove(table, key, &model) == CC_ERR_KEY_NOT_FOUND)return;

  free(model);

  pthread_mutex_unlock(&lock);
}

void ChangeUEStateInInterface(HashTable* table, void* key, int state)
{
  //pthread_mutex_lock(&lock);
  void* model;
  if(hashtable_get(table, key, &model) == CC_ERR_KEY_NOT_FOUND)
  {
    printf("Can't find key ,ChangeUESTATE \n");
    return;
  }

  struct Model* model_ue = (struct Model*)model;

  printf("state: %d \n", state);

  switch (state) {
    case ue_state_none:
      model_ue->state->tex_uniform->uniform_id = ue_states->tex_state_none->uniform_id;
      break;
    case ue_state_preamble:
      printf("Changing ue state preamble \n");
      model_ue->state->tex_uniform->uniform_id = ue_states->tex_state_preamble->uniform_id;
      break;
    case ue_state_response:
      model_ue->state->tex_uniform->uniform_id = ue_states->tex_state_response->uniform_id;
      break;
    case ue_state_request:
      model_ue->state->tex_uniform->uniform_id = ue_states->tex_state_request->uniform_id;
      break;
    case ue_state_setup:
      model_ue->state->tex_uniform->uniform_id = ue_states->tex_state_setup->uniform_id;
      break;
    case ue_state_complete:
      model_ue->state->tex_uniform->uniform_id = ue_states->tex_state_complete->uniform_id;
      break;
    default:

      model_ue->state->tex_uniform->uniform_id = ue_states->tex_state_none->uniform_id;
  }
  printf("Changed ue state bar \n");

  //pthread_mutex_unlock(&lock);
}

void ChangeUEState(void* key, int state)
{
  //pthread_mutex_lock(&lock);

  state_ue->state = state;
  state_ue->key = key;
  state_ue->flag_state = true;

  //pthread_mutex_unlock(&lock);
}

void SetupIsUE()
{
  temp_ue = malloc(sizeof(struct IsNEWUE));
  temp_ue->flag = false;

  state_ue = malloc(sizeof(struct ChangeUE));
  state_ue->flag_state = false;

}

void SetupUEStates()
{
  ue_states = malloc(sizeof(struct UEStates));
  ue_states->tex_state_none = malloc(sizeof(struct Uniform));
  ue_states->tex_state_preamble = malloc(sizeof(struct Uniform));
  ue_states->tex_state_response = malloc(sizeof(struct Uniform));
  ue_states->tex_state_request = malloc(sizeof(struct Uniform));
  ue_states->tex_state_setup = malloc(sizeof(struct Uniform));
  ue_states->tex_state_complete = malloc(sizeof(struct Uniform));

  ue_states->tex_state_none = LoadImage("./assets/images/none.png", "fixed_uniform");
  ue_states->tex_state_preamble = LoadImage("./assets/images/preamble.png", "fixed_uniform");
  ue_states->tex_state_response = LoadImage("./assets/images/response.png", "fixed_uniform");
  ue_states->tex_state_request = LoadImage("./assets/images/response.png", "fixed_uniform");
  ue_states->tex_state_setup = LoadImage("./assets/images/ue.png", "fixed_uniform");
  ue_states->tex_state_complete = LoadImage("./assets/images/request.png", "fixed_uniform");
}

void ClearIsUE()
{
  free(temp_ue);
  free(state_ue);
  state_ue = NULL;
  temp_ue = NULL;
}

void ClearUEStates()
{
  free(ue_states->tex_state_none);
  free(ue_states->tex_state_preamble);
  free(ue_states->tex_state_response);
  free(ue_states->tex_state_request);
  free(ue_states->tex_state_setup);
  free(ue_states->tex_state_complete);
  free(ue_states);
  ue_states = NULL;
}

void CheckIsUE(HashTable* table)
{
  if(temp_ue->flag)
  {
    AddUEToHashTable(table, temp_ue->key);
    temp_ue->flag = false;
    return;
  }
  if(state_ue->flag_state)
  {
    ChangeUEStateInInterface(table, state_ue->key, state_ue->state);
    state_ue->flag_state = false;
    return;
  }
}

void InitMutex()
{
  if (pthread_mutex_init(&lock, NULL) != 0)
  {
    printf("\n mutex init failed\n");
    exit(EXIT_FAILURE);
  }
}

void DrawUEs(HashTable* table, struct Frame* frame)
{
  HashTableIter hti;
  hashtable_iter_init(&hti, table);
  TableEntry *entry;
  while (hashtable_iter_next(&hti, &entry) != CC_ITER_END)
  {
    struct Model* model = (struct Model*)entry->value;
    DrawBlink(frame,model->blink);
    DrawFixed(frame, model);
    DrawRange(frame, model);
    if(model->state != NULL)DrawFixed(frame, model->state);
  }
}
