#include "Application.h"



int main(int argc, char const *argv[]) {

  Application::GetInstance()->Run();
  Application::DestroyInstance();

  return 0;
}
