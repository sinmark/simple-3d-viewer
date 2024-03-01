#include <array>
#include <iterator>
#include <simple_3d_viewer/Viewer.hpp>
#include <simple_3d_viewer/utils/Size.hpp>
#include <simple_3d_viewer/utils/glfwUtils.hpp>

namespace Simple3D
{
Viewer::Viewer(
    GLFWwindow* window,
    const std::vector<std::string>& postprocessIDs)
    : window_(window),
      scene_(getFramebufferSize(window)),
      renderer_(postprocessIDs, getFramebufferSize(window))
{
  init();
}

void Viewer::init()
{
  // Skybox initialization
  scene_.skyboxProgram.doOperations([](Program& program)
                                    { program.setInt("skybox", 0); });

  // Model initialization
  modelConfig_.set(Model::Configuration::Flag::IssueRenderingAPICalls, false);
}

void Viewer::render(const Size framebufferSize)
{
  isModelLoaded();

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
    scene_.modelProgram = Program(modelProgramFilename);
  }
  catch (std::invalid_argument& e)
  {
    mediator_->notify(Error::ReloadProgram, e.what());
  }
}

void Viewer::isModelLoaded()
{
  const std::chrono::milliseconds amountToWait(0);
  if (modelFuture_.valid() &&
      (modelFuture_.wait_for(amountToWait) != std::future_status::timeout))
  {
    try
    {
      scene_.model = modelFuture_.get();
      scene_.model->complete();
      assert(mediator_);
      mediator_->notify(Event::ModelLoaded);
    }
    catch (std::invalid_argument& e)
    {
      mediator_->notify(Error::LoadModel, e.what());
    }
  }
}
}  // namespace Simple3D