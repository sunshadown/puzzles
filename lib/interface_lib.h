#ifndef INTERFACE_H
#define INTERFACE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
//OpenGl wrangler
#include "./includes/GL/glew.h"
//WINDOW HANDLER
#include "./includes/GLFW/glfw3.h"
//MATH LIBRARY
#include "./includes/cglm/cglm.h"
#include "./includes/cglm/vec3.h"
#include "./includes/cglm/vec4.h"
#include "./includes/cglm/mat4.h"
#include "./includes/cglm/affine.h"
#include "./includes/cglm/common.h"
#include "./includes/cglm/types.h"
//IMAGELOADER
#include "lodepng.h"
//DATA STRUCTURES
#include "./includes/array.h"
#include "./includes/hashtable.h"
//INTERFACE
#include "./interface_lib.h"

typedef struct Frame frame;
typedef struct Model Model;
typedef struct Uniform Uniform;
typedef void (*Event)(int,int,int,int);

#define window_max_name 255
#define uniform_max_name 128
#define ue_max_range 5

#define ue_state_none 0
#define ue_state_preamble 1
#define ue_state_response 2
#define ue_state_request 3
#define ue_state_setup 4
#define ue_state_complete 5

struct Frame
{
  GLFWwindow* window;
  size_t window_size_x;
  size_t window_size_y;
  char* window_name;
  Event* events;
  mat4 ortho_matrix;
};

struct Model
{
  vec3 position;
  vec3 size;
  float rotate;
  GLuint program_id;
  GLuint vao,vbo;
  size_t elements_size;
  mat4 model_matrix;
  struct Uniform* tex_uniform;
  float m_depth;
  size_t number_of_instances;
  float timer;
  float timer_inc;
  float counter;
  struct Model* state;
  struct Model** range;
  size_t range_number;
  struct Blink* blink;
};

struct Blink
{
  struct Model* model;
  vec3 position;
  float interval;
  float time_inc;
  bool flag;
};

struct Uniform
{
  GLuint uniform_id;
  const char* uniform_name;
};

struct IsNEWUE
{
  void* key;
  bool flag;
};

struct ChangeUE
{
  int state;
  bool flag_state;
  void* key;
};

struct UEStates
{
  Uniform* tex_state_none;
  Uniform* tex_state_preamble;
  Uniform* tex_state_response;
  Uniform* tex_state_request;
  Uniform* tex_state_setup;
  Uniform* tex_state_complete;

};
//INTERFACE LOOP
extern void* InterfaceLoop(void* table);
//WINDOW HANDLING
extern void InitExtensions();
extern void InitGlWrangler();
extern struct Frame* CreateFrame(size_t window_size_x,size_t window_size_y,const char* window_name,size_t window_name_size);
extern void WindowInit(struct Frame* frame);
extern void WindowLoop(struct Frame* frame, HashTable* table);
extern void WindowClean(struct Frame* frame);

//IMAGES-PNG ONLY, Shader loading from file, Universal uniforms
extern struct Uniform* LoadImage(const char* path, const char* uniform_name);
extern char* LoadShader(const char* path, size_t buff_size);
extern void AttachShader(char* shader, struct Model* model, int flag);
extern void LinkShader(struct Model* model);
extern struct Uniform* CreateUniform(GLuint uniform_id, const char* uniform_name);
extern void ClearUniform(struct Uniform** uniform);

//MODEL initialize to default values
extern struct Model* CreateModel();

//Generate default figure fixtures
extern void GenerateFullScreen(struct Model* model);
extern void GenerateFixedSize(struct Model* model);
extern void GenerateWave(struct Model* model, size_t number_of_instances);
//Usefull functions
extern void CreateBackground(struct Model* model, const char* pathtoimage);
extern void CreateFixed(struct Model* model, const char* pathtoimage);
extern void CreateWave(struct Model* model);
extern struct Model** CreateWaveSequence(size_t number_of_models, float gap);
//DRAWING
extern void DrawBackground(struct Model* model);
extern void DrawFixed(struct Frame* frame, struct Model* model);
extern void DrawWave(struct Frame* frame, struct Model* model);
extern void DrawWaveSequence(struct Frame* frame, struct Model** models, size_t number_of_models);
extern void DrawRange(struct Frame* frame, struct Model* model);
extern void DrawBlink(struct Frame* frame, struct Blink* blink);
//DATA HANDLING
extern struct Model* CreateUE();
extern void CreateRange(struct Model* model);
extern HashTable* CreateHashTable();
extern bool AddUEToHashTable(HashTable* table, void* key);
extern void AddUEToInterface(void* key);
extern void RemoveUEFromInterface(HashTable* table, void* key);
extern void ChangeUEStateInInterface(HashTable* table, void* key, int state);
extern void ChangeUEState(void* key, int state);
extern void SetupIsUE();
extern void SetupUEStates();
extern void ClearIsUE();
extern void ClearUEStates();
extern void CheckIsUE(HashTable* table);
extern void InitMutex();
extern void DrawUEs(HashTable* table, struct Frame* frame);
#endif
