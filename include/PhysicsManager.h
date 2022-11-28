#pragma once
#include <SceneManager.h>
#include <TestScene.h>
#include <vector>

class PhysicsManager : public SceneManager<PhysicsManager>
{
private:

   static bool isRightClick;
   static bool firstMouse;

   static GLfloat lastX;
   static GLfloat lastY;
   static GLfloat yaw;
   static GLfloat pitch;

   static Camera cam;

   static std::unique_ptr<SphereScene> sc;
   static std::unique_ptr<HexaScene> hsc;

   friend SceneManager;

   static void _initalize();

   static void _destroy();

   static void _render(ShaderProgram& shaders);

   static void _update(GLfloat timeElapsed);

   static void _onSize(int width, int height);

   static void _onMouseMove(double xpos, double ypos);

   static void _onMouseClick(int button, int action, int mods, double mouse_x, double mouse_y);

   static void _onMouseScroll(double xoffset, double yoffset);

   static void _onKeyboad(int key, int scancode, int action, int mods);
};