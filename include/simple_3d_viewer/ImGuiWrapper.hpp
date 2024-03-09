#pragma once

#include <glad/glad.h>

#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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

  void update();
  void render()
  {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
  void release()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }
  void setMediator(Mediator* mediator)
  {
    mediator_ = mediator;
  }

  [[nodiscard]] const std::string& getSelectedFilePath() const
  {
    return filePath_;
  }

  [[nodiscard]] const std::vector<Checkbox>& getPostprocessesCheckboxes() const
  {
    return postprocessesCheckboxes_;
  }

  [[nodiscard]] const std::vector<Checkbox>& getLightControlsCheckboxes() const
  {
    return lightControlsCheckboxes_;
  }

  [[nodiscard]] const std::vector<Checkbox>&
  getModelLoadingConfigurationCheckboxes() const
  {
    return modelLoadingConfigurationCheckboxes_;
  }

  [[nodiscard]] const std::vector<Slider>& getLightControlsSliders() const
  {
    return lightControlsSliders_;
  }

  [[nodiscard]] const std::vector<Slider>& getModelControlsSliders() const
  {
    return modelTransformSliders_;
  }

  [[nodiscard]] const std::vector<Slider>& getCameraControlsSliders() const
  {
    return cameraControlsSliders_;
  }

  void printError(std::string_view errorMessage)
  {
    cachedErrorMessage_ = errorMessage;
    ImGui::OpenPopup("errorPopup");
  }

 private:
  Mediator* mediator_{ nullptr };
  std::string filePath_;
  Checkboxes postprocessesCheckboxes_;
  Checkboxes lightControlsCheckboxes_;
  Checkboxes modelLoadingConfigurationCheckboxes_;
  Sliders lightControlsSliders_;
  Sliders modelTransformSliders_;
  Sliders cameraControlsSliders_;
  std::string cachedErrorMessage_;

  void drawSettingsWindow();
  void drawLightingArea();
  void drawModelArea();
  void drawCameraArea();
  void loadModelDialog();
};
}  // namespace Simple3D
