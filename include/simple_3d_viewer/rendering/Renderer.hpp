#pragma once

#include <glad/glad.h>

#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/opengl_object_wrappers/Texture.hpp>
#include <simple_3d_viewer/rendering/Mesh.hpp>
#include <simple_3d_viewer/rendering/Model.hpp>
#include <simple_3d_viewer/rendering/PostprocessPipeline.hpp>
#include <simple_3d_viewer/rendering/Scene.hpp>
#include <simple_3d_viewer/utils/Size.hpp>

namespace Simple3D
{

class Renderer
{
 public:
  Renderer(
      const std::vector<PostprocessID>& postprocessIDs,
      Size framebufferSize)
      : postprocessPipeline_(postprocessIDs, framebufferSize)
  {
  }

  PostprocessPipeline postprocessPipeline_;
  bool drawLight_{ true };

  void render(Scene& scene, Size framebufferSize);

 private:
  template<typename T>
  struct CachePair
  {
    T value;
    bool changed;

    void update(
        const T& newValue,
        const std::function<void()>& operationsBeforeUpdate)
    {
      if (changed = value != newValue; !changed)
      {
        return;
      }
      operationsBeforeUpdate();
      value = newValue;
    }
  };

  struct Cache
  {
    struct ModelProgramUniformsCache
    {
      using IDType = uint64_t;
      static constexpr IDType idTypeMaxValue{
        std::numeric_limits<IDType>::max()
      };
      static constexpr float floatMaxValue{ std::numeric_limits<float>::max() };

      CachePair<IDType> modelProgramID{ idTypeMaxValue, false };
      CachePair<IDType> materialID{ idTypeMaxValue, false };
      CachePair<glm::mat4x4> modelTransform{};
      CachePair<std::pair<glm::mat4x4, glm::mat4x4>> projectionViewTransform{};
      CachePair<glm::vec3> cameraPosition{
        { floatMaxValue, floatMaxValue, floatMaxValue },
        false
      };
      CachePair<glm::vec3> lightPosition{
        { floatMaxValue, floatMaxValue, floatMaxValue },
        false
      };

      void clear()
      {
        modelProgramID.value = idTypeMaxValue;
        materialID.value = idTypeMaxValue;
        modelTransform.value = {};
        projectionViewTransform.value = {};
        cameraPosition.value = { floatMaxValue, floatMaxValue, floatMaxValue };
        lightPosition.value = { floatMaxValue, floatMaxValue, floatMaxValue };
      }
    } modelProgramUniformsCache;
    CachePair<Size> framebufferSize{};
  };
  Cache cache_;
  GLFWwindow* window_;

  void performCacheChecks(Scene& scene, Size framebufferSize);
  void render(Model& model, Program& program);
  void render(Mesh& mesh, Program& program);
  void
  renderSkybox(Mesh& skybox, Program& skyboxProgram, Texture& skyboxTexture);
};
}  // namespace Simple3D
