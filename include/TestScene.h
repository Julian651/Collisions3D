#pragma once
#include <RenderUtilities.h>


class TestScene : public Scene<TestScene>
{
private:

   friend Scene;
   static void _init()
   {
      fprintf(stderr, "test scene 1 init\n");
      GenBindFromBlender("resources\\sphere.obj");
   }
   static void _destroy()
   {
      fprintf(stderr, "test scene 1 destroy\n");
   }

   void _render(Camera& cam, ShaderProgram& shaders)
   {

   }

protected:

public:

   TestScene(glm::vec3 pos) : Scene(pos) {}
};

class TestScene2 : public Scene<TestScene2>
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

   void _render(Camera& cam, ShaderProgram& shaders)
   {

   }

protected:

public:

   TestScene2(glm::vec3 pos) : Scene(pos) {}
};