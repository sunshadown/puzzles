#ifndef APPLICATION_H
#define APPLICATION_H

#include "Manager.h"

class Application
{
private:
  Application();
  ~Application();
  static Application* m_instance;
  Manager* m_manager;
public:
  static Application* GetInstance();
  static void DestroyInstance();
  void Run();
};


#endif
