#include "render_utils.h"
#include <assert.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>


Object::Object(glm::vec3 inWorldPosition) : m_worldPosition(inWorldPosition), m_ID(-1), m_program(nullptr)
{

}

GLsizei Object::ID() const
{
   return m_ID;
}

void Object::Update(GLfloat inTimeElapsed)
{
}

glm::vec3 Object::WorldPosition() const
{
   return m_worldPosition;
}

// ====================================================================================================================

Camera::Camera(glm::vec3 inPos, glm::vec3 inLook) : m_pos(inPos), m_look(inLook)
{
}

glm::vec3 Camera::Position() const
{
   return m_pos;
}

glm::vec3 Camera::Looking() const
{
   return m_look;
}

void Camera::Move(glm::vec3 inPos)
{
   m_pos = inPos;
}

void Camera::LookAt(glm::vec3 inLook)
{
   m_look = inLook;
}

// ====================================================================================================================

void Scene::AddBufferObj()
{
   m_numBuffers += 1;
   GLuint VBO;
   GLuint EBO;
   glGenBuffers(1, &VBO);
   glGenBuffers(1, &EBO);
   m_VBOS.push_back(VBO);
   m_EBOS.push_back(EBO);

   assert(m_numBuffers == m_VBOS.size() && m_numBuffers == m_EBOS.size());
}

void Scene::Render(GLsizei inID, ShaderProgram inShaders, glm::vec3 inPos) const
{
   GLint size;
   GLsizei in = inID - GLsizei(1);
   assert(in >= GLsizei(0) && in < m_numBuffers);

   glBindVertexArray(m_VAO);
   glBindBuffer(GL_ARRAY_BUFFER, m_VBOS[in]);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOS[in]);
   glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

   auto model = glm::translate(glm::mat4(1.f), inPos);
   auto view = glm::lookAt(cam.Position(), cam.Position() + cam.Looking(), glm::vec3(0.f, 1.f, 0.f));
   auto projection = glm::perspective(glm::radians(45.f), 1.f * m_screenWidth / m_screenHeight, 0.1f, 160.f);
   auto mvp = projection * view * model;
   glUniformMatrix4fv(inShaders.GetUniform("m_transform"), 1, GL_FALSE, glm::value_ptr(mvp));

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
   glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLsizei Scene::GenBind(const GLvoid* inData, GLsizeiptr inDataSize, GLenum inType, const GLuint* inIndices, GLsizeiptr inIndicesSize)
{
   AddBufferObj();
   GLsizei ID = static_cast<GLsizei>(m_VBOS.size());
   ReBind(ID, inData, inDataSize, inType, inIndices, inIndicesSize);
   return ID;
}

void Scene::ReBind(GLsizei inID, const GLvoid* inData, GLsizeiptr inDataSize, GLenum inType, const GLuint* inIndices, GLsizeiptr inIndicesSize) const
{
   GLsizei in = inID - GLsizei(1);
   assert(in >= GLsizei(1) && in < m_numBuffers);

   glBindVertexArray(m_VAO);
   glBindBuffer(GL_ARRAY_BUFFER, m_VBOS[in]);
   glBufferData(GL_ARRAY_BUFFER, inDataSize, inData, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOS[in]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, inIndicesSize, inIndices, GL_STATIC_DRAW);

   GLsizei stride = 0;
   switch (inType)
   {
   case GL_UNSIGNED_INT:
   {
      stride = 3 * sizeof(GLuint);
   } break;
   case GL_INT:
   {
      stride = 3 * sizeof(GLint);
   } break;
   case GL_FLOAT:
   {
      stride = 3 * sizeof(GLfloat);
   } break;
   }
   glVertexAttribPointer(0, 3, inType, GL_FALSE, stride, (GLvoid*)0);
   glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Scene::ReBindFromBlender(GLsizei inID, const char* inBlenderFilename)
{
   GLsizei in = inID - GLsizei(1);
   assert(in >= GLsizei(1) && in < m_numBuffers);

   std::vector<GLfloat> vertices;
   std::vector<GLfloat> uvs;
   std::vector<GLfloat> normals;
   std::vector<GLuint> vertIndices, uvIndices, normalIndices;

   std::ifstream infile;
   infile.open(inBlenderFilename, std::ios::in);

   if (!infile.is_open())
   {
      std::cerr << "error\n";
      return;
   }
   infile.seekg(0, std::ios::beg);
   while (infile.peek() != EOF)
   {
      char ch;
      infile.read(&ch, 1);
      if (ch == 'v')
      {
         GLfloat x, y, z;
         infile.read(&ch, 1);
         if (ch == ' ')
         {
            infile >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
         }
         else if (ch == 'n')
         {
            infile >> x >> y >> z;
            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);
         }
         else if (ch == 't')
         {
            infile >> x >> y;
            uvs.push_back(x);
            uvs.push_back(y);
         }
      }
      else if (ch == 'f')
      {
         infile.read(&ch, 1); // space
         while (ch != '\n')
         {
            GLuint v1, v2, v3, n1, n2, n3, t1, t2, t3;
            infile >> v1;
            infile.read(&ch, 1); // slash
            infile >> t1;
            infile.read(&ch, 1); // slash
            infile >> n1;
            infile.read(&ch, 1); // space
            infile >> v2;
            infile.read(&ch, 1); // slash
            infile >> t2;
            infile.read(&ch, 1); // slash
            infile >> n2;
            infile.read(&ch, 1); // space
            infile >> v3;
            infile.read(&ch, 1); // slash
            infile >> t3;
            infile.read(&ch, 1); // slash
            infile >> n3;
            infile.read(&ch, 1); // new line
            vertIndices.push_back(v1 - 1);
            vertIndices.push_back(v2 - 1);
            vertIndices.push_back(v3 - 1);
            normalIndices.push_back(n1 - 1);
            normalIndices.push_back(n2 - 1);
            normalIndices.push_back(n3 - 1);
            uvIndices.push_back(t1 - 1);
            uvIndices.push_back(t2 - 1);
            uvIndices.push_back(t3 - 1);
         }
      }
   }
   infile.close();


   glBindVertexArray(m_VAO);
   glBindBuffer(GL_ARRAY_BUFFER, m_VBOS[in]);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOS[in]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertIndices.size() * sizeof(GLuint), vertIndices.data(), GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
   glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Scene::CreateObject(GLsizei inID, ShaderProgram& inProgram, Object& outObj)
{
   outObj.m_ID = inID;
   outObj.m_program = &inProgram;
   m_objects.push_back(&outObj);
}

Scene::Scene(GLsizei inWidth, GLsizei inHeight, GLsizei inDepth, GLsizei inNumBuffers) : m_width(inWidth), m_height(inHeight), m_depth(inDepth), m_numBuffers(inNumBuffers)
{
   glGenVertexArrays(1, &m_VAO);
   m_VBOS = std::vector<GLuint>(m_numBuffers);
   m_EBOS = std::vector<GLuint>(m_numBuffers);
   glGenBuffers(inNumBuffers, m_VBOS.data());
   glGenBuffers(inNumBuffers, m_EBOS.data());

   assert(m_numBuffers == m_VBOS.size() && m_numBuffers == m_EBOS.size());

   GLsizei l, w, h;
   l = inDepth >> 1;
   w = inWidth >> 1;
   h = inHeight >> 1;
   GLint vertices[] = {
      // front
      -1 * w, -1 * h,  1 * l,
       1 * w, -1 * h,  1 * l,
       1 * w,  1 * h,  1 * l,
      -1 * w,  1 * h,  1 * l,
      // back
      -1 * w, -1 * h, -1 * l,
       1 * w, -1 * h, -1 * l,
       1 * w,  1 * h, -1 * l,
      -1 * w,  1 * h, -1 * l
   };
   GLuint indices[] = {  // note that we start from 0!
      0, 1,
      1, 2,
      2, 3,
      3, 0,

      1, 5,
      5, 6,
      6, 2,
      2, 1,

      7, 6,
      6, 5,
      5, 4,
      4, 7,

      4, 0,
      0, 3,
      3, 7,
      7, 4,

      4, 5,
      5, 1,
      1, 0,
      0, 4,

      3, 2,
      2, 6,
      6, 7,
      7, 3
   };
   glBindVertexArray(m_VAO);
   glBindBuffer(GL_ARRAY_BUFFER, m_VBOS[0]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOS[0]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
   glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Scene::~Scene()
{
   assert(m_numBuffers == m_VBOS.size() && m_numBuffers == m_EBOS.size());

   glDeleteVertexArrays(1, &m_VAO);
   glDeleteBuffers(m_numBuffers, m_VBOS.data());
   glDeleteBuffers(m_numBuffers, m_EBOS.data());
}

void Scene::Render(ShaderProgram& inShaders) const
{
   // First render the scene box
   GLint size;

   glUseProgram(inShaders.ID);
   glBindVertexArray(m_VAO);
   glBindBuffer(GL_ARRAY_BUFFER, m_VBOS[0]);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOS[0]);
   glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

   auto model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
   auto view = glm::lookAt(cam.Position(), cam.Position()  + cam.Looking(), glm::vec3(0.f, 1.f, 0.f));
   auto projection = glm::perspective(glm::radians(45.f), 1.f * m_screenWidth / m_screenHeight, 0.1f, 160.f);
   auto mvp = projection * view * model;
   glUniformMatrix4fv(inShaders.GetUniform("m_transform"), 1, GL_FALSE, glm::value_ptr(mvp));

   glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 0, (GLvoid*)0);
   glDrawElements(GL_LINES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


   // Render the rest
   for (auto& obj : m_objects)
   {
      Render(obj->ID(), *obj->m_program, obj->m_worldPosition);
   }
}

GLuint Scene::ID() const
{
   return m_VAO;
}

void Scene::EnableUpdates(bool inUpdate)
{
   m_updatesEnabled = inUpdate;
}

// ====================================================================================================================

std::unordered_map<GLsizei, Scene*> SceneManager::m_scenes = std::unordered_map<GLsizei, Scene*>(0);
Scene* SceneManager::m_current = nullptr;
GLfloat SceneManager::m_screenWidth = 0.f;
GLfloat SceneManager::m_screenHeight = -1.f;
void SceneManager::OnSize(GLFWwindow*, int inWidth, int inHeight)
{
   if (m_current)
   {
      m_current->OnSize(inWidth, inHeight);
   }
}

void SceneManager::OnMouseMove(GLFWwindow*, double inXpos, double inYpos)
{
   if (m_current)
   {
      m_current->OnMouseMove(inXpos, inYpos);
   }
}

void SceneManager::OnMouseScroll(GLFWwindow*, double inXoffset, double inYoffset)
{
   if (m_current)
   {
      m_current->OnMouseScroll(inXoffset, inYoffset);
   }
}

void SceneManager::OnKeyboard(GLFWwindow* inWindow, int inKey, int inScancode, int inAction, int inMods)
{
   if (inKey == GLFW_KEY_ESCAPE && inAction == GLFW_PRESS)
   {
      glfwSetWindowShouldClose(inWindow, true);
   }
   if (m_current)
   {
      m_current->OnKeyboard(inKey, inScancode, inAction, inMods);
   }
}

void SceneManager::SetWindow(GLFWwindow* inWindow)
{
   int width, height;
   glfwGetWindowSize(inWindow, &width, &height);
   m_screenWidth = (GLfloat)width;
   m_screenHeight = (GLfloat)height;
   glfwSetFramebufferSizeCallback(inWindow, SceneManager::OnSize);
   glfwSetKeyCallback(inWindow, SceneManager::OnKeyboard);
   glfwSetCursorPosCallback(inWindow, SceneManager::OnMouseMove);
   glfwSetScrollCallback(inWindow, SceneManager::OnMouseScroll);
}

void SceneManager::AddScene(Scene& inScene)
{
   m_scenes.insert({ inScene.ID(), &inScene });
}

void SceneManager::SetScene(GLsizei inID)
{
   auto it = m_scenes.find(inID);
   if (it == m_scenes.end())
   {
      return;
   }
   m_current = it->second;
   m_current->m_screenWidth = m_screenWidth;
   m_current->m_screenHeight = m_screenHeight;
}

Scene* SceneManager::GetScene(GLsizei inID)
{
   auto it = m_scenes.find(inID);
   return it == m_scenes.end() ? nullptr : it->second;
}

void SceneManager::Render(ShaderProgram& inProgram) // render without updating
{
   if (m_current)
   {
      m_current->Render(inProgram);
   }
}

void SceneManager::Render(ShaderProgram& inProgram, GLfloat inTimeElapsed) // render and update
{
   if (m_current)
   {
      m_current->Render(inProgram);
      m_current->Update(inTimeElapsed);
      glfwPostEmptyEvent();
   }
}