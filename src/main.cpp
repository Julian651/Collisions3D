#include <GL/glad.h>
#include <GL/glfw3.h>
#include "PhysicsScene.h"
#include <iostream>
#include <chrono>

// settings
unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 720;

int main()
{
   // glfw: initialize and configure
   // ------------------------------
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

   // glfw window creation
   // --------------------
   GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
   if (window == NULL)
   {
      std::cerr << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   if (glfwRawMouseMotionSupported())
      glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cerr << "Failed to initialize GLAD" << std::endl;
      return -1;
   }

   GLuint vertexShader = compileShader("resources\\myshader.vert", eShaderType::VERTEX);
   GLuint fragmentShader = compileShader("resources\\myfragment.frag", eShaderType::FRAGMENT);
   ShaderProgram program;
   program.attach(vertexShader, eShaderType::VERTEX);
   program.attach(fragmentShader, eShaderType::FRAGMENT);
   program.link();
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

   PhysicsScene scene1;
   BallObject obj(glm::vec3(-3.f, 0.f, 0.f));
   BallObject obj2(glm::vec3(3.f, 0.f, 0.f));
   obj2.m_velocity = glm::vec3(25.f, -3.f, -12.5f);
   obj.m_velocity = glm::vec3(-25.f, 3.f, 12.5f);
   scene1.AddBall(obj, program);
   scene1.AddBall(obj2, program);
   scene1.EnableUpdates();

   // uncomment this call to draw in wireframe polygons.
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   glEnable(GL_DEPTH_TEST);
   glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

   SceneManager::SetWindow(window);
   SceneManager::AddScene(scene1);
   SceneManager::SetScene(scene1.ID());
   //glfwSetTime(0.0);
   while (!glfwWindowShouldClose(window))
   {
      glfwSetTime(0.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      SceneManager::Render(program, 0.0006f);

      glfwSwapBuffers(window);
      glfwPollEvents();
      std::cout << "fps: " << 1.f / glfwGetTime() << std::endl;
   }

   glfwTerminate();
   return 0;
}
