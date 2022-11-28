#pragma once
#include <RenderUtilities.h>

template <class T>
class SceneManager
{
private:

   static bool m_initialized;

protected:

   static GLfloat m_screenWidth;
   static GLfloat m_screenHeight;

public:

   static inline void Initialize()
   {
      if (m_initialized) return;
      T::_initalize();
      m_initialized = true;
   }

   static inline void Destroy()
   {
      assert(m_initialized);

      T::_destroy();
      m_initialized = false;
   }

   static inline void Render(ShaderProgram& shaders)
   {
      T::_render(shaders);
   }

   static inline void Update(GLfloat timeElapsed)
   {
      T::_update(timeElapsed);
   }

   static inline void SetWindow(GLFWwindow* win)
   {
      int width, height;
      glfwGetWindowSize(win, &width, &height);
      m_screenWidth = (GLfloat)width;
      m_screenHeight = (GLfloat)height;

      glfwSetFramebufferSizeCallback(win, SceneManager::OnSize);
      glfwSetKeyCallback(win, SceneManager::OnKeyboard);
      glfwSetCursorPosCallback(win, SceneManager::OnMouseMove);
      glfwSetMouseButtonCallback(win, SceneManager::OnMouseClick);
      glfwSetScrollCallback(win, SceneManager::OnMouseScroll);
   }

   static inline void OnSize(GLFWwindow*, int width, int height)
   {
      m_screenWidth = static_cast<GLfloat>(width);
      m_screenHeight = static_cast<GLfloat>(height);
      glViewport(0, 0, width, height);

      T::_onSize(width, height);
   }
   
   static inline void OnMouseMove(GLFWwindow* win, double xpos, double ypos)
   {
      T::_onMouseMove(xpos, ypos);
   }

   static inline void OnMouseClick(GLFWwindow* win, int button, int action, int mods)
   {
      double x;
      double y;
      glfwGetCursorPos(win, &x, &y);
      T::_onMouseClick(button, action, mods, x, y);
   }

   static inline void OnMouseScroll(GLFWwindow*, double xoffset, double yoffset)
   {
      T::_onMouseScroll(xoffset, yoffset);
   }

   static inline void OnKeyboard(GLFWwindow* win, int key, int scancode, int action, int mods)
   {
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      {
         glfwSetWindowShouldClose(win, true);
      }

      T::_onKeyboad(key, scancode, action, mods);
   }

};

template <class T>
bool SceneManager<T>::m_initialized = false;

template <class T>
GLfloat SceneManager<T>::m_screenWidth = 0.f;

template <class T>
GLfloat SceneManager<T>::m_screenHeight = -1.f;