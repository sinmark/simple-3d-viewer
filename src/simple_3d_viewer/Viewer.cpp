#include "simple_3d_viewer/rendering/Model.hpp"
#include "simple_3d_viewer/utils/constants.hpp"
#include "simple_3d_viewer/utils/factories.hpp"
#include <array>
#include <future>
#include <iterator>
#include <optional>
#include <simple_3d_viewer/Viewer.hpp>
#include <simple_3d_viewer/utils/Size.hpp>
#include <simple_3d_viewer/utils/glfwUtils.hpp>
#include <stdexcept>

namespace Simple3D
{

Viewer::Viewer(
    GLFWwindow* window,
    const std::vector<std::string>& postprocessIDs)
    : window_(window),
      scene_(getFramebufferSize(window)),
      renderer_(postprocessIDs, getFramebufferSize(window))
{
  // Skybox initialization
  scene_.skyboxProgram.doOperations([](Program& program)
                                    { program.setInt("skybox", 0); });

  // Model initialization
  modelConfig_.set(Model::Configuration::Flag::IssueRenderingAPICalls, false);
}

namespace
{

std::optional<Model> checkModelFuture(std::future<Model>& modelFuture)
{
  if (constexpr std::chrono::milliseconds amountToWait(0);
      modelFuture.valid() &&
      (modelFuture.wait_for(amountToWait) != std::future_status::timeout))
  {
    return modelFuture.get();
  }

  return std::nullopt;
}

}  // namespace

void Viewer::render(const Size framebufferSize)
{
  if (mediator_ == nullptr)
  {
    throw std::logic_error("Mediator should be setup by now");
  }

  if (auto maybeModel = checkModelFuture(modelFuture_); maybeModel.has_value())
  {
    try
    {
      maybeModel->complete();
      scene_.model = std::move(maybeModel);
      mediator_->notify(Event::ModelLoaded);
    }
    catch (std::invalid_argument& e)
    {
      mediator_->notify(Error::LoadModel, e.what());
    }
  }

  if (framebufferSize.width <= 0 || framebufferSize.height <= 0)
  {
    return;
  }

  renderer_.render(scene_, framebufferSize);
}

void Viewer::reloadProgram()
{
  try
  {
    scene_.modelProgram = createProgram(kModelShaderCommonName);
  }
  catch (std::invalid_argument& e)
  {
    mediator_->notify(Error::ReloadProgram, e.what());
  }
}

}  // namespace Simple3D