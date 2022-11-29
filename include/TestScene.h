#pragma once
#include <RenderUtilities.h>
#include <memory>

struct BallObject : public Object
{
   glm::vec3 m_velocity = glm::vec3(0.f);
   void Update(GLfloat timeElapsed)
   {
      m_worldPosition = m_worldPosition + m_velocity * timeElapsed;
   }
};

class SphereScene : public Scene<SphereScene>
{
private:

   friend Scene;
   static void _init()
   {
      fprintf(stderr, "test scene 1 init\n");
      GenBindFromBlender("resources\\sphere.obj");
      AddBufferObj();
   }
   static void _destroy()
   {
      fprintf(stderr, "test scene 1 destroy\n");
   }

   void _render(Camera& cam, ShaderProgram& shaders, double screenWidth, double screenHeight)
   {
      // First render the scene box
      GLint size;

      glUseProgram(shaders.ID);
      glBindVertexArray(m_VAO);
      glBindBuffer(GL_ARRAY_BUFFER, m_VBOS[2]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOS[2]);
      glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

      auto model = glm::translate(glm::mat4(1.f), m_pos);
      auto view = glm::lookAt(cam.Position(), cam.Position() + cam.Looking(), glm::vec3(0.f, 1.f, 0.f));
      auto projection = glm::perspective(glm::radians(45.f), 1.f * static_cast<float>(screenWidth / screenHeight), 0.1f, 160.f);
      auto mvp = projection * view * model;
      glUniformMatrix4fv(shaders.GetUniform("m_transform"), 1, GL_FALSE, glm::value_ptr(mvp));

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
      glDrawElements(GL_LINES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   }

protected:


public:

   SphereScene(glm::vec3 pos) : Scene(pos)
   {
   }
   ~SphereScene() { fprintf(stderr, "delete scene1\n"); }

   void AddSphere(glm::vec3 pos, glm::vec3 vel)
   {
      GLint w = m_width >> 1;
      GLint h = m_height >> 1;
      GLint d = m_depth >> 1;
      if (pos.x > w || pos.x < -w || pos.y > h || pos.y < -h || pos.z > d || pos.z < -d) return;

      std::unique_ptr<BallObject> obj = std::make_unique<BallObject>();
      obj->m_worldPosition = pos;
      obj->m_velocity = vel;
      CreateObject(2, nullptr, std::move(obj));
   }

   void Update(GLfloat timeElapsed)
   {
      for (auto& obj : m_objects)
      {
         auto ballObj = static_cast<BallObject*>(obj.get());
         ballObj->Update(timeElapsed);
         if (obj->m_worldPosition.x <= -25.f || obj->m_worldPosition.x >= 25.f)
         {
            ballObj->m_velocity.x = -ballObj->m_velocity.x;
         }
         if (obj->m_worldPosition.y <= -25.f || obj->m_worldPosition.y >= 25.f)
         {
            ballObj->m_velocity.y = -ballObj->m_velocity.y;
         }
         if (obj->m_worldPosition.z <= -25.f || obj->m_worldPosition.z >= 25.f)
         {
            ballObj->m_velocity.z = -ballObj->m_velocity.z;
         }
      }
   }

   void AddLine(Camera cam, glm::vec3 ray, GLfloat len)
   {
      glm::vec3 camPos = cam.Position();
      GLfloat verts[6] = {
         camPos.x, camPos.y, camPos.z,
         (camPos.x + ray.x * len), (camPos.y + ray.y * len), (camPos.z + ray.z * len)
      };
      GLuint indis[4] = {
         0, 1,
         1, 0
      };
      ReBind(3, verts, 6 * sizeof(GLfloat), GL_FLOAT, indis, 4 * sizeof(GLuint));
   }

   void CheckIntersections(Camera cam, glm::vec3 ray)
   {

      AddLine(cam, ray, 300.f);
      for (auto& obj : m_objects)
      {
         glm::vec3 pos = obj->m_worldPosition;
         glm::vec3 camPos = cam.Position();
         glm::vec3 d = camPos - pos;
         GLfloat len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);

         glm::vec3 point = glm::vec3((camPos.x + ray.x * len), (camPos.y + ray.y * len), (camPos.z + ray.z * len));

         if (fabsf(point.x - pos.x) <= 1.f && fabsf(point.y - pos.y) <= 1.f && fabsf(point.z - pos.z) <= 1.f)
         {
            fprintf(stderr, "inbound %p\n", obj.get());
         }
      }
   }
};

class HexaScene : public Scene<HexaScene>
{
private:

   friend Scene;
   static void _init()
   {
      fprintf(stderr, "test scene 2 init\n");
      GenBindFromBlender("resources\\hexa.obj");
   }
   static void _destroy()
   {
      fprintf(stderr, "test scene 2 destroy\n");
   }

   void _render(Camera& cam, ShaderProgram& shaders, double screenWidth, double screenHeight)
   {

   }

protected:

public:

   HexaScene(glm::vec3 pos) : Scene(pos)
   {
   }

   ~HexaScene()
   {
      fprintf(stderr, "delete scene2\n");
   }

   void AddHexa(glm::vec3 pos)
   {
      GLint w = m_width >> 1;
      GLint h = m_height >> 1;
      GLint d = m_depth >> 1;
      if (pos.x > w || pos.x < -w || pos.y > h || pos.y < -h || pos.z > d || pos.z < -d) return;

      std::unique_ptr<Object> obj = std::make_unique<Object>();
      obj->m_worldPosition = pos;
      CreateObject(2, nullptr, std::move(obj));
   }
};