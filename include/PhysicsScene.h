#pragma once
#include "render_utils.h"

class BallObject : public Object
{
private:

public:

   BallObject(glm::vec3 inWorldPosition = glm::vec3(0.f));
   glm::vec3 m_velocity = glm::vec3(0.f);
   glm::vec3 m_initPosition;
   void Update(GLfloat inTimeElapsed) override;
};


class PhysicsScene : public Scene
{
private:

   GLfloat lastX = 0.f;
   GLfloat lastY = 0.f;
   GLfloat yaw = -90.0f;
   GLfloat pitch = 0.f;
   GLboolean firstMouse = true;

public:

   PhysicsScene();

   void Update(GLfloat inTimeElapsed) override;
   void OnSize(int inWidth, int inHeight) override;
   void OnMouseMove(double inXpos, double inYpos) override;
   void OnMouseScroll(double inXoffset, double inYoffset) override;
   void OnKeyboard(int inKey, int inScancode, int inAction, int inMods) override;
   

   void AddBall(BallObject& inObj, ShaderProgram& inProgram);
};

