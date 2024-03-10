#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <filesystem>
#include <future>
#include <memory>
#include <simple_3d_viewer/rendering/Renderer.hpp>
#include <simple_3d_viewer/rendering/Scene.hpp>

namespace Simple3D
{

class Viewer
{
 public:
  enum class Event
  {
    ModelLoaded
  };

  enum class Error
  {
    ReloadProgram,
    LoadModel
  };

  class Mediator
  {
   public:
    virtual ~Mediator() = default;

    virtual void notify(Event e) = 0;
    virtual void notify(Error e, const std::string& errorMessage) = 0;
  };

  Viewer(GLFWwindow* window, const std::vector<std::string>& postprocessIDs);

  Scene scene_;
  Renderer renderer_;
  Model::Configuration modelConfig_;

  void processInput(float delta)
  {
    if (mediator_ == nullptr)
    {
      throw std::logic_error("Mediator should be setup by now");
    }

    scene_.camera.processInput(delta, window_);
  }

  void render(Size framebufferSize);

  void setPostprocessActiveFlag(const std::string& ID, bool active)
  {
    renderer_.postprocessPipeline_.setPostprocessActiveFlag(ID, active);
  }

  void loadModel(const std::filesystem::path& pathToModel)
  {
    if (scene_.model)
    {
      scene_.model.reset();
    }
    modelFuture_ = std::async(
        std::launch::async,
        [pathToModel, modelConfig = modelConfig_]()
        { return Model(pathToModel, modelConfig); });
  }

  void setModelTransform(const Transform& transform)
  {
    if (scene_.model)
    {
      scene_.model->setTransform(transform);
    }
  }

  void setCameraSettings(Camera::Settings settings)
  {
    scene_.camera.setSettings(settings);
  }

  void reloadProgram();

  void setMediator(std::shared_ptr<Mediator> mediator)
  {
    mediator_ = std::move(mediator);
  }

 private:
  GLFWwindow* window_;
  std::future<Model> modelFuture_;
  std::shared_ptr<Mediator> mediator_;

  void init();
  void isModelLoaded();
};
}  // namespace Simple3D
