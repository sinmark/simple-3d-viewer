#pragma once

#include <glad/glad.h>

#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <string>
#include <vector>

namespace Simple3D
{

struct Slider
{
  std::string text;
  float defaultValue;
  float currentValue;
  float minValue;
  float maxValue;
};

using Sliders = std::vector<Slider>;

struct Checkbox
{
  std::string text;
  bool value;
};

using Checkboxes = std::vector<Checkbox>;

class ImGuiWrapper
{
 public:
  enum class Event
  {
    PostprocessesControlsChange,
    LightingControlsChange,
    VisualizeLightPositionCheckboxChange,
    ModelControlsChange,
    ModelLoadingConfigurationChange,
    CameraControlsChange,
    LoadModel,
    ReloadProgram,
  };

  // We will notify the outside world about different events through an object
  // of this class
  class Mediator
  {
   public:
    virtual ~Mediator() = default;

    virtual void notify(Event e) = 0;
  };

  ImGuiWrapper(
      GLFWwindow* window,
      const std::vector<std::string>& postprocesses);
  ~ImGuiWrapper()
  {
    release();
  }

  void release();

  void render();

  void update();

  void setMediator(std::shared_ptr<Mediator> mediator)
  {
    mediator_ = std::move(mediator);
  }

  [[nodiscard]] const std::filesystem::path& getModelFilePath() const
  {
    return modelFilePath_;
  }

  [[nodiscard]] const Checkboxes& getPostprocessesCheckboxes() const
  {
    return postprocessesCheckboxes_;
  }

  [[nodiscard]] const Checkboxes& getLightControlsCheckboxes() const
  {
    return lightControlsCheckboxes_;
  }

  [[nodiscard]] const Checkboxes& getModelLoadingConfigurationCheckboxes() const
  {
    return modelLoadingConfigurationCheckboxes_;
  }

  [[nodiscard]] const Sliders& getLightControlsSliders() const
  {
    return lightControlsSliders_;
  }

  [[nodiscard]] const Sliders& getModelControlsSliders() const
  {
    return modelTransformSliders_;
  }

  [[nodiscard]] const Sliders& getCameraControlsSliders() const
  {
    return cameraControlsSliders_;
  }

  void printError(std::string_view errorMessage)
  {
    cachedErrorMessage_ = errorMessage;
    ImGui::OpenPopup("errorPopup");
  }

 private:
  std::shared_ptr<Mediator> mediator_;
  Checkboxes postprocessesCheckboxes_;
  Sliders lightControlsSliders_;
  Checkboxes lightControlsCheckboxes_;
  Sliders modelTransformSliders_;
  Checkboxes modelLoadingConfigurationCheckboxes_;
  Sliders cameraControlsSliders_;
  std::filesystem::path modelFilePath_;
  std::string cachedErrorMessage_;

  void drawSettingsWindow();
};
}  // namespace Simple3D
