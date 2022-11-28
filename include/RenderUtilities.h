#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/matrix_double4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glad.h>
#include <vector>
#include <fstream>

// ====================================================================================================================

enum class eShaderType
{
   VERTEX, FRAGMENT
};

inline GLuint compileShader(const char* filename, eShaderType type)
{
   GLchar* buffer;

   std::ifstream infile;
   infile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

   if (!infile.is_open())
   {
      fprintf(stderr, "error\n");
      return -1;
   }
   infile.seekg(0, std::ios_base::end);
   int len = (int)infile.tellg();
   infile.seekg(0, std::ios_base::beg);

   buffer = new char[len + 1];
   infile.read(buffer, len);
   buffer[len] = '\0';

   infile.close();

   GLuint shader;
   GLint success;
   GLchar infoLog[512];
   if (type == eShaderType::VERTEX)
   {
      // vertex shader
      shader = glCreateShader(GL_VERTEX_SHADER);
   }
   else
   {
      // fragment shader
      shader = glCreateShader(GL_FRAGMENT_SHADER);
   }
   glShaderSource(shader, 1, &buffer, NULL);
   glCompileShader(shader);
   // check for shader compile errors
   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      for (int i = 0; i < len + 1; i++)
      {
         fprintf(stderr, "%c", buffer[i]);
      }
      fprintf(stderr, "\n");
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s", infoLog);
   }

   delete[] buffer;
   return shader;
}

struct ShaderProgram
{
   const GLint ID;

   inline ShaderProgram() : ID(glCreateProgram()) {}

   inline ~ShaderProgram()
   {
      glDeleteProgram(ID);
   }

   inline void Attach(GLuint shader, eShaderType type)
   {
      if (type == eShaderType::VERTEX)
      {
         glAttachShader(ID, shader);
      }
      else
      {
         glAttachShader(ID, shader);
      }
   }

   inline void Attach(GLuint vertexShader, GLuint fragmentShader)
   {
      glAttachShader(ID, vertexShader);
      glAttachShader(ID, fragmentShader);
   }

   GLint inline GetAttrib(const char* attribName)
   {
      GLint attrib = glGetAttribLocation(ID, attribName);
      if (attrib == -1)
      {
         fprintf(stderr, "Could not bind attribute %s\n", attribName);
         return -1;
      }
      return attrib;
   }

   inline GLint GetUniform(const char* uniformName)
   {
      GLint uniform = glGetUniformLocation(ID, uniformName);
      if (uniform == -1)
      {
         fprintf(stderr, "Could not bind uniform %s\n", uniformName);
         return -1;
      }
      return uniform;
   }

   inline void Link()
   {
      GLint success;
      GLchar infoLog[512];
      glLinkProgram(ID);
      // check for linking errors
      glGetProgramiv(ID, GL_LINK_STATUS, &success);
      if (!success)
      {
         glGetProgramInfoLog(ID, 512, NULL, infoLog);
         fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n %s", infoLog);
      }
   }
};

// ====================================================================================================================

class Camera
{
private:

   glm::vec3 m_pos;
   glm::vec3 m_look;

public:

   Camera(glm::vec3 pos = glm::vec3(0.f), glm::vec3 look = glm::vec3(0.f, 0.f, -1.f)) : m_pos(pos), m_look(look) {}
   glm::vec3 Position() const { return m_pos; }
   glm::vec3 Looking() const { return m_look; }
   void Move(glm::vec3 pos) { m_pos = pos; }
   void LookAt(glm::vec3 look) { m_look = look; }
};

// ====================================================================================================================

struct Object
{
   GLsizei m_ID;
   GLsizei m_RenderID;
   ShaderProgram* m_program;
   glm::vec3 m_worldPosition = glm::vec3(0.f);
   ~Object() {}
};

// ====================================================================================================================

template <class T>
class Scene
{
private:

   static bool m_initialized;
   
   inline void Render(GLsizei ID, Camera& cam, ShaderProgram& shaders, glm::vec3 pos, double screenWidth, double screenHeight)
   {
      GLint size;
      GLsizei in = ID - GLsizei(1);
      assert(in >= GLsizei(0) && in < m_numBuffers);

      glBindVertexArray(m_VAO);
      glBindBuffer(GL_ARRAY_BUFFER, m_VBOS[in]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOS[in]);
      glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

      auto model = glm::translate(glm::mat4(1.f), m_pos + pos);
      auto view = glm::lookAt(cam.Position(), cam.Position() + cam.Looking(), glm::vec3(0.f, 1.f, 0.f));
      auto projection = glm::perspective(glm::radians(45.f), 1.f * static_cast<float>(screenWidth / screenHeight), 0.1f, 160.f);
      auto mvp = projection * view * model;
      glUniformMatrix4fv(shaders.GetUniform("m_transform"), 1, GL_FALSE, glm::value_ptr(mvp));

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
      glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   }


protected:

   static GLuint m_VAO;
   static GLsizei m_numBuffers;
   static std::vector<GLuint> m_VBOS;
   static std::vector<GLuint> m_EBOS;

   const glm::vec3 m_pos;
   const GLsizei m_width;
   const GLsizei m_height;
   const GLsizei m_depth;

   std::vector<std::unique_ptr<Object>> m_objects;
   GLsizei numObjects;

   static inline void AddBufferObj()
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

   inline void CreateObject(GLsizei ID, ShaderProgram* program, std::unique_ptr<Object> obj)
   {
      obj->m_ID = ++numObjects;
      obj->m_RenderID = ID;
      obj->m_program = program;
      m_objects.push_back(std::move(obj));
   }

   inline void ReBind(GLsizei inID, const GLvoid* inData, GLsizeiptr inDataSize, GLenum inType, const GLuint* inIndices, GLsizeiptr inIndicesSize) const
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

   static inline GLsizei GenBindFromBlender(const char* inBlenderFilename)
   {
      AddBufferObj();
      GLsizei ID = static_cast<GLsizei>(m_VBOS.size());
      ReBindFromBlender(ID, inBlenderFilename);
      return ID;
   }

   static inline void ReBindFromBlender(GLsizei inID, const char* inBlenderFilename)
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
         fprintf(stderr, "error\n");
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

public:

   inline Scene(glm::vec3 pos = glm::vec3(0.0), GLsizei height = 50, GLsizei width = 50, GLsizei depth = 50) : m_pos(pos), m_width(width), m_height(height), m_depth(depth)
   {

      GLsizei l, w, h;
      l = depth >> 1;
      w = width >> 1;
      h = height >> 1;
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
      GLuint indices[] = {
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

   static inline void Initialize()
   {
      if (m_initialized) return;
      glGenVertexArrays(1, &m_VAO);
      glGenBuffers(m_numBuffers, m_VBOS.data());
      glGenBuffers(m_numBuffers, m_EBOS.data());

      assert(m_numBuffers == m_VBOS.size() && m_numBuffers == m_EBOS.size());

      T::_init();
      m_initialized = true;
   }

   static inline void Destroy()
   {
      assert(m_numBuffers == m_VBOS.size() && m_numBuffers == m_EBOS.size());
      assert(m_initialized);

      glDeleteVertexArrays(1, &m_VAO);
      glDeleteBuffers(m_numBuffers, m_VBOS.data());
      glDeleteBuffers(m_numBuffers, m_EBOS.data());

      T::_destroy();
      m_initialized = false;
   }

   inline void Render(Camera& cam, ShaderProgram& shaders, double screenWidth, double screenHeight)
   {
      // First render the scene box
      GLint size;

      glUseProgram(shaders.ID);
      glBindVertexArray(m_VAO);
      glBindBuffer(GL_ARRAY_BUFFER, m_VBOS[0]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOS[0]);
      glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

      auto model = glm::translate(glm::mat4(1.f), m_pos);
      auto view = glm::lookAt(cam.Position(), cam.Position() + cam.Looking(), glm::vec3(0.f, 1.f, 0.f));
      auto projection = glm::perspective(glm::radians(45.f), 1.f * static_cast<float>(screenWidth / screenHeight), 0.1f, 160.f);
      auto mvp = projection * view * model;
      glUniformMatrix4fv(shaders.GetUniform("m_transform"), 1, GL_FALSE, glm::value_ptr(mvp));

      glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 0, (GLvoid*)0);
      glDrawElements(GL_LINES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      for (auto& obj : m_objects)
      {
         if (obj->m_program)
         {
            Render(obj->m_RenderID, cam, *obj->m_program, obj->m_worldPosition, screenWidth, screenHeight);
         }
         else
         {
            Render(obj->m_RenderID, cam, shaders, obj->m_worldPosition, screenWidth, screenHeight);
         }
      }

      static_cast<T*>(this)->_render(cam, shaders, screenWidth, screenHeight);
   }
};

template <class T>
GLuint Scene<T>::m_VAO = 0;

template <class T>
GLsizei Scene<T>::m_numBuffers = 1;

template <class T>
std::vector<GLuint> Scene<T>::m_VBOS = std::vector<GLuint>(m_numBuffers);

template <class T>
std::vector<GLuint> Scene<T>::m_EBOS = std::vector<GLuint>(m_numBuffers);

template <class T>
bool Scene<T>::m_initialized = false;