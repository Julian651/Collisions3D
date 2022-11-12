#pragma once
#include <GL/glad.h>

enum class eShaderType
{
   VERTEX, FRAGMENT
};
GLuint compileShader(const char* filename, eShaderType type);
struct ShaderProgram
{
   const GLint ID;
   ShaderProgram();
   ~ShaderProgram();
   void attach(GLint shader, eShaderType type);
   void attach(GLint vertexShader, GLint fragmentShader);
   GLint set_attribute(const char* attribName);
   GLint GetUniform(const char* uniformName);
   void link();
};

struct VertexBufferObject
{
   const GLenum type;
   const GLint ID;
   void set_buffer_data(GLsizeiptr size, const void* data);
   void set_attrib_ptr(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
};