#pragma once
#include <GL/glad.h>
#include <GL/glfw3.h>
#include <glm/vec3.hpp>
#include <vector>
#include <unordered_map>
#include "shader_utils.h"

class Scene;
class Object
{
private:

   friend Scene;
   GLsizei m_ID; // ID that corresponds to the bound IDs in the scene for each template object
   ShaderProgram* m_program; // The shader program for this object


protected:

   glm::vec3 m_worldPosition;

public:

   Object(glm::vec3 inWorldPosition);
   virtual GLsizei ID() const final;
   virtual void Update(GLfloat inTimeElapsed);
   virtual glm::vec3 WorldPosition() const final;

};

class Camera
{
private:

   glm::vec3 m_pos;
   glm::vec3 m_look;

public:

   Camera(glm::vec3 inPos = glm::vec3(0.f), glm::vec3 inLook = glm::vec3(0.f));
   glm::vec3 Position() const;
   glm::vec3 Looking() const;
   void Move(glm::vec3 inPos);
   void LookAt(glm::vec3 inLook);
};

class SceneManager;
/// <summary>
/// A VertexArrayObj stores a user defined amount of buffer objects.
/// These buffer objects represent a blueprint for the objects which are to be rendered in a scene.
/// 
/// So a VertexArrayObj with a size 3 of buffer objects will have 3 blueprints for 3 different objects
/// </summary>
class Scene
{
private:

   friend SceneManager;
   const GLsizei m_width;
   const GLsizei m_height;
   const GLsizei m_depth;
   GLsizei m_numBuffers;
   GLuint m_VAO;
   std::vector<GLuint> m_VBOS;
   std::vector<GLuint> m_EBOS;
   bool m_updatesEnabled = false;


   /// <summary>
   /// 
   /// </summary>
   virtual void AddBufferObj() final;


   /// <summary>
   /// Render the given buffer number on the screen.
   /// </summary>
   /// *Note: The buffer must have been bounded before hand or else it won't render
   /// <param name="inBuffer"></param>
   virtual void Render(GLsizei inID, ShaderProgram inShaders, glm::vec3 inPos) const final;


protected:

   GLfloat m_screenWidth = 0.f;
   GLfloat m_screenHeight = -1.f;
   Camera cam;
   std::vector<Object*> m_objects;


   /// <summary>
   /// Generate a bind with the given vertex and index data
   /// </summary>
   /// <param name="inData"></param>
   /// <param name="inDataSize"></param>
   /// <param name="inType"></param>
   /// <param name="inIndices"></param>
   /// <param name="inIndicesSize"></param>
   /// <returns>The buffer bind ID that was generated</returns>
   virtual GLsizei GenBind(const GLvoid* inData, GLsizeiptr inDataSize, GLenum inType, const GLuint* inIndices, GLsizeiptr inIndicesSize) final;


   /// <summary>
   /// Bind the data with the defined indices to the vertex array object
   /// </summary>
   /// <param name="inBuffer">The buffer we are to bind to</param>
   /// <param name="inData">The vertex array data</param>
   /// <param name="inDataSize">The size of the vertex array</param>
   /// <param name="inType">The type for the vertex array</param>
   /// <param name="inIndices">The index array</param>
   /// <param name="inIndicesSize">The size of the index array</param>
   virtual void ReBind(GLsizei inID, const GLvoid* inData, GLsizeiptr inDataSize, GLenum inType, const GLuint* inIndices, GLsizeiptr inIndicesSize) const final;


   virtual void ReBindFromBlender(GLsizei inID, const char* inBlenderFilename);


   virtual void CreateObject(GLsizei inID, ShaderProgram& inProgram, Object& outObj) final;

public:

   Scene(GLsizei inWidth = 50, GLsizei inHeight = 50, GLsizei inDepth = 50, GLsizei inNumBuffers = 1);
   virtual ~Scene();

   
   virtual void Render(ShaderProgram& inShaders) const final;

   /// <summary>
   /// 
   /// </summary>
   virtual GLuint ID() const final;


   virtual void EnableUpdates(bool inUpdate = true);


   virtual void Update(GLfloat inTimeElapsed) = 0;
   virtual void OnSize(int inWidth, int inHeight) = 0;
   virtual void OnMouseMove(double inXpos, double inYpos) = 0;
   virtual void OnMouseScroll(double inXoffset, double inYoffset) = 0;
   virtual void OnKeyboard(int inKey, int inScancode, int inAction, int inMods) = 0;
};

class SceneManager // there can only be one scene open at a time
{
private:

   static GLfloat m_screenWidth;
   static GLfloat m_screenHeight;
   static std::unordered_map<GLsizei, Scene*> m_scenes;
   static Scene* m_current;
   static void OnSize(GLFWwindow*, int inWidth, int inHeight);
   static void OnMouseMove(GLFWwindow*, double inXpos, double inYpos);
   static void OnMouseScroll(GLFWwindow*, double inXoffset, double inYoffset);
   static void OnKeyboard(GLFWwindow*, int inKey, int inScancode, int inAction, int inMods);

public:
   
   static void SetWindow(GLFWwindow* inWindow);
   
   static void AddScene(Scene& inScene);

   static void SetScene(GLsizei inID);

   static Scene* GetScene(GLsizei inID);

   static void Render(ShaderProgram& inProgram); // render without updating

   static void Render(ShaderProgram& inProgram, GLfloat inTimeElapsed); // render and update
};