#include <PhysicsManager.h>

void PhysicsManager::_initalize()
{
   cam.Move(glm::vec3(0.f, 0.f, 100.f));

   SphereScene::Initialize();
   sc = std::make_unique<SphereScene>(glm::vec3(0.f));
   sc->AddSphere(glm::vec3(0.f), glm::vec3(25.f, 0.f, 0.f));
   sc->AddSphere(glm::vec3(-24.f, 24.f, 24.f), glm::vec3(-25.f, 0.f, 0.f));

   HexaScene::Initialize();
   hsc = std::make_unique<HexaScene>(glm::vec3(60.f, 0.f, 0.f));
   hsc->AddHexa(glm::vec3(0.f));
   hsc->AddHexa(glm::vec3(24.f, -24.f, -24.f));
}

void PhysicsManager::_destroy()
{
   SphereScene::Destroy();
   HexaScene::Destroy();
}

void PhysicsManager::_render(ShaderProgram& shaders)
{
   sc->Render(cam, shaders, m_screenWidth, m_screenHeight);
   hsc->Render(cam, shaders, m_screenWidth, m_screenHeight);
}

void PhysicsManager::_update(GLfloat timeElapsed)
{
   sc->Update(timeElapsed);
}

void PhysicsManager::_onSize(int width, int height)
{
}

void PhysicsManager::_onMouseMove(double xpos, double ypos)
{
   if (!isRightClick) return;
   GLfloat xposIn = static_cast<GLfloat>(xpos);
   GLfloat yposIn = static_cast<GLfloat>(ypos);
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

void PhysicsManager::_onMouseClick(int button, int action, int mods, double mouse_x, double mouse_y)
{
   if (button == GLFW_MOUSE_BUTTON_RIGHT)
   {
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
      {
         isRightClick = true;
         firstMouse = true;
      }
      else
      {
         isRightClick = false;
         firstMouse = false;
      }
   }
   else if (button == GLFW_MOUSE_BUTTON_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
   {
      float x = (2.f * static_cast<float>(mouse_x)) / m_screenWidth - 1.f;
      float y = 1.f - (2.f * static_cast<float>(mouse_y)) / m_screenHeight;
      float z = 1.f;
      glm::vec3 ray_nds = glm::vec3(x, y, z);

      glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.f, 1.f);

      auto projection = glm::perspective(glm::radians(45.f), 1.f * static_cast<float>(m_screenWidth / m_screenHeight), 0.1f, 160.f);
      glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;

      ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.f, 0.f);

      auto view = glm::lookAt(cam.Position(), cam.Position() + cam.Looking(), glm::vec3(0.f, 1.f, 0.f));
      glm::vec4 w = glm::inverse(view) * ray_eye;
      glm::vec3 ray_wor = glm::vec3(w.x, w.y, w.z);
      // don't forget to normalise the vector at some point
      ray_wor = glm::normalize(ray_wor);

      sc->CheckIntersections(cam, ray_wor);
   }
}

void PhysicsManager::_onMouseScroll(double xoffset, double yoffset)
{
}

void PhysicsManager::_onKeyboad(int key, int scancode, int action, int mods)
{
   if (action == GLFW_PRESS || action == GLFW_REPEAT)
   {
      glm::vec3 camCurrentPos = cam.Position();
      glm::vec3 camCurrentLook = cam.Looking();
      switch (key)
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
            cam.Move(glm::vec3(camCurrentPos.x, camCurrentPos.y - 1.f, camCurrentPos.z));
            break;
         }
      }
   }
}

Camera PhysicsManager::cam;

std::unique_ptr<SphereScene> PhysicsManager::sc = nullptr;
std::unique_ptr<HexaScene> PhysicsManager::hsc = nullptr;

bool PhysicsManager::isRightClick = false;
bool PhysicsManager::firstMouse = true;

GLfloat PhysicsManager::lastX = 0.f;
GLfloat PhysicsManager::lastY = 0.f;
GLfloat PhysicsManager::yaw = -90.f;
GLfloat PhysicsManager::pitch = 0.f;