#include "Application.h"


Application* Application::m_instance(nullptr);

Application::Application(){}
Application::~Application(){}


Application* Application::GetInstance()
{
  if(m_instance == nullptr)m_instance = new Application();
  return m_instance;
}

void Application::DestroyInstance()
{
  if(m_instance != nullptr)delete m_instance;
}

void Application::Run()
{
  m_instance->m_manager = new Manager();
}
