#include "PhysicsScene.h"
#include "GL/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>
PhysicsScene::PhysicsScene() : Scene(50, 50, 50, 2)
{
   cam.Move(glm::vec3(0.f, 0.f, 100.f));
   cam.LookAt(glm::vec3(0.f, 0.f, -1.f));
   ReBindFromBlender(2, "resources\\sphere.obj");
}

void PhysicsScene::Update(GLfloat inTimeElapsed)
{
   for (auto& obj : m_objects)
   {
      obj->Update(inTimeElapsed);
      auto ballObj = static_cast<BallObject*>(obj);
      if (obj->WorldPosition().x <= -25.f || obj->WorldPosition().x >= 25.f)
      {
         ballObj->m_velocity.x = -ballObj->m_velocity.x;
      }
      if (obj->WorldPosition().y <= -25.f || obj->WorldPosition().y >= 25.f)
      {
         ballObj->m_velocity.y = -ballObj->m_velocity.y;
      }
      if (obj->WorldPosition().z <= -25.f || obj->WorldPosition().z >= 25.f)
      {
         ballObj->m_velocity.z = -ballObj->m_velocity.z;
      }
   }
}

void PhysicsScene::OnSize(int inWidth, int inHeight)
{
   m_screenWidth = static_cast<GLfloat>(inWidth);
   m_screenHeight = static_cast<GLfloat>(inHeight);
   glViewport(0, 0, inWidth, inHeight);
}

void PhysicsScene::OnMouseMove(double inXpos, double inYpos)
{
   GLfloat xposIn = static_cast<GLfloat>(inXpos);
   GLfloat yposIn = static_cast<GLfloat>(inYpos);
   if (firstMouse)
   {
      lastX = xposIn;
      lastY = yposIn;
      firstMouse = false;
      return;
   }

   GLfloat xoffset = xposIn - lastX;
   GLfloat yoffset = lastY - yposIn;
   lastX = xposIn;
   lastY = yposIn;

   GLfloat sensitivity = 0.1f;
   xoffset *= sensitivity;
   yoffset *= sensitivity;

   yaw += xoffset;
   pitch += yoffset;

   if (pitch > 89.0f)
   {
      pitch = 89.0f;
   }
   if (pitch < -89.0f)
   {
      pitch = -89.0f;
   }

   glm::vec3 direction = glm::vec3(0.f);
   direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
   direction.y = sin(glm::radians(pitch));
   direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
   cam.LookAt(glm::normalize(direction));
}

void PhysicsScene::OnMouseScroll(double inXoffset, double inYoffset)
{
}

void PhysicsScene::OnKeyboard(int inKey, int inScancode, int inAction, int inMods)
{
   if (inKey == GLFW_PRESS || inAction == GLFW_REPEAT)
   {
      glm::vec3 camCurrentPos = cam.Position();
      glm::vec3 camCurrentLook = cam.Looking();
      switch (inKey)
      {
         case GLFW_KEY_W:
         {
            cam.Move(camCurrentPos + camCurrentLook);
            break;
         }
         case GLFW_KEY_A: // strafe left
         {
            glm::mat4 rotation = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
            glm::vec3 perp = rotation * glm::vec4(camCurrentLook, 1.f);
            perp.y = 0.f;
            cam.Move(camCurrentPos - glm::normalize(perp));
            break;
         }
         case GLFW_KEY_S:
         {
            cam.Move(camCurrentPos - camCurrentLook);
            break;
         }
         case GLFW_KEY_D: // strafe right
         {
            glm::mat4 rotation = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
            glm::vec3 perp = rotation * glm::vec4(camCurrentLook, 1.f);
            perp.y = 0.f;
            cam.Move(camCurrentPos + glm::normalize(perp));
            break;
         }
         case GLFW_KEY_SPACE:
         {
            cam.Move(glm::vec3(camCurrentPos.x, camCurrentPos.y + 1.f, camCurrentPos.z));
            break;
         }
         case GLFW_KEY_LEFT_CONTROL:
         {
            cam.Move(glm::vec3(camCurrentPos.x, camCurrentPos.y -1.f, camCurrentPos.z));
            break;
         }
      }
   }
}

void PhysicsScene::AddBall(BallObject& inObj, ShaderProgram& inProgram)
{
   CreateObject(2, inProgram, inObj);
}

BallObject::BallObject(glm::vec3 inWorldPosition) : m_initPosition(inWorldPosition), Object(inWorldPosition)
{

}
void BallObject::Update(GLfloat inTimeElapsed)
{
   m_worldPosition = m_worldPosition + m_velocity * inTimeElapsed;
}
