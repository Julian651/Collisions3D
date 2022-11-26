#include <TestScene.h>
#include <iostream>
#include <GL/glfw3.h>

#define SCR_WIDTH 1200
#define SCR_HEIGHT 720


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
   GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scenes", NULL, NULL);
   if (window == NULL)
   {
      std::cerr << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }

   glfwSetWindowPos(window, 500, 100);
   glfwMakeContextCurrent(window);
   //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   //if (glfwRawMouseMotionSupported())
   //   glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cerr << "Failed to initialize GLAD" << std::endl;
      return -1;
   }

   ShaderProgram prog;
   prog.Attach(compileShader("resources\\myshader.vert", eShaderType::VERTEX),
      compileShader("resources\\myfragment.frag", eShaderType::FRAGMENT));
   prog.Link();

   Camera cam(glm::vec3(0.f, 0.f, 100.f));

   TestScene::Initialize();
   TestScene::Initialize();
   TestScene2::Initialize();

   TestScene sc(glm::vec3(0.f));
   TestScene2 sc2(glm::vec3(50.f, 0.f, 0.f));

   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glEnable(GL_DEPTH_TEST);
   glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
   while (!glfwWindowShouldClose(window))
   {
      glfwSetTime(0.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      sc.Render(cam, prog);
      sc2.Render(cam, prog);

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   TestScene::Destroy();
   TestScene2::Destroy();

   glfwTerminate();
   return 0;
}