#include "shader_utils.h"
#include <iostream>
#include <fstream>

ShaderProgram::ShaderProgram() : ID(glCreateProgram())
{
}

ShaderProgram::~ShaderProgram()
{
   glDeleteProgram(ID);
}

void ShaderProgram::attach(GLint shader, eShaderType type)
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

void ShaderProgram::attach(GLint vertexShader, GLint fragmentShader)
{
   glAttachShader(ID, vertexShader);
   glAttachShader(ID, fragmentShader);
}

GLint ShaderProgram::set_attribute(const char* attribName)
{
   GLint attrib = glGetAttribLocation(ID, attribName);
   if (attrib == -1) {
      std::cout << "Could not bind uniform " << attribName << std::endl;
      return -1;
   }
   return attrib;
}

GLint ShaderProgram::GetUniform(const char* uniformName)
{
   GLint uniform = glGetUniformLocation(ID, uniformName);
   if (uniform == -1) {
      std::cout << "Could not bind uniform " << uniformName << std::endl;
      return -1;
   }
   return uniform;
}

void ShaderProgram::link()
{
   GLint success;
   GLchar infoLog[512];
   glLinkProgram(ID);
   // check for linking errors
   glGetProgramiv(ID, GL_LINK_STATUS, &success);
   if (!success) {
      glGetProgramInfoLog(ID, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
   }
}

GLuint compileShader(const char* filename, eShaderType type)
{
   char* buffer;

   std::ifstream infile;
   infile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

   if (!infile.is_open())
   {
      std::cout << "error\n";
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
         std::cout << buffer[i];
      }
      std::cout << std::endl;
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
   }

   delete[] buffer;
   return shader;
}
/*
Object create_cube()
{
   GLint vertices[] = {
      // front
      -1, -1,  1,
       1, -1,  1,
       1,  1,  1,
      -1,  1,  1,
      // back
      -1, -1, -1,
       1, -1, -1,
       1,  1, -1,
      -1,  1, -1
   };
   GLuint indices[] = {  // note that we start from 0!
		// front
      0, 1, 2,
      2, 3, 0,
      // right
      1, 5, 6,
      6, 2, 1,
      // back
      7, 6, 5,
      5, 4, 7,
      // left
      4, 0, 3,
      3, 7, 4,
      // bottom
      4, 5, 1,
      1, 0, 4,
      // top
      3, 2, 6,
      6, 7, 3
   };

   GLuint VBO;
   GLuint EBO;
   glGenBuffers(1, &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glGenBuffers(1, &EBO);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 3 * sizeof(int), (void*)0);
   glEnableVertexAttribArray(0);

   // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
   //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
   // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
   //glBindVertexArray(0);
   
   return Object(VBO, EBO);
}

Object bind_triangle()
{
   GLfloat vertices[] = {
      0.f, -1.f, -1.f,
      1.f, -1.f, 1.f,
      -1.f, -1.f, 1.f,
      0.f, 1.f, 0.f
   };
   GLuint indices[] = {  // note that we start from 0!
      0, 1, 2,
      2, 3, 1,
      1, 0, 3,
      3, 0, 2
   };
   GLuint VBO;
   GLuint EBO;
   glGenBuffers(1, &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glGenBuffers(1, &EBO);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
   //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
   // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
   //glBindVertexArray(0);

   return Object(VBO, EBO);
}

Object bind_sphere(GLuint& VBO, GLuint& EBO)
{
   return Object(VBO, EBO);
}

Object::Object(GLuint VBO, GLuint EBO) : VBO(VBO), EBO(EBO)
{
}

Object::~Object()
{
   glDeleteBuffers(1, &VBO);
   glDeleteBuffers(1, &EBO);
}
*/