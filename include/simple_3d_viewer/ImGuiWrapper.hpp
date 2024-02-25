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
class ImGuiWrapper
{
 public:
  struct Slider
  {
    std::string text;
    float defaultValue;
    float currentValue;
    float minValue;
    float maxValue;
  };
  struct Checkbox
  {
    std::string text;
    bool value;
  };
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
      GLFWwindow *window,
      const std::vector<std::string> &postprocesses);
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
  void setMediator(Mediator *mediator)
  {
    mediator_ = mediator;
  }
  void sync();

 public:
  const std::string &getSelectedFilePath() const
  {
    return filePath_;
  }
  const std::vector<Checkbox> &getPostprocessesCheckboxes() const
  {
    return postprocessesCheckboxes_;
  }
  const std::vector<Checkbox> &getLightControlsCheckboxes() const
  {
    return lightControlsCheckboxes_;
  }
  const std::vector<Checkbox> &getModelLoadingConfigurationCheckboxes() const
  {
    return modelLoadingConfigurationCheckboxes_;
  }
  const std::vector<Slider> &getLightControlsSliders() const
  {
    return lightControlsSliders_;
  }
  const std::vector<Slider> &getModelControlsSliders() const
  {
    return modelTransformSliders_;
  }
  const std::vector<Slider> &getCameraControlsSliders() const
  {
    return cameraControlsSliders_;
  }
  void printError(const std::string &errorMessage)
  {
    cachedErrorMessage_ = errorMessage;
    ImGui::OpenPopup("errorPopup");
  }

 private:
  Mediator *mediator_{ nullptr };
  std::string filePath_;
  std::vector<Checkbox> postprocessesCheckboxes_;
  std::vector<Checkbox> lightControlsCheckboxes_;
  std::vector<Checkbox> modelLoadingConfigurationCheckboxes_;
  std::vector<Slider> lightControlsSliders_;
  std::vector<Slider> modelTransformSliders_;
  std::vector<Slider> cameraControlsSliders_;
  bool synced_{ false };
  std::string cachedErrorMessage_;

  void init(GLFWwindow *window);
  void drawSettingsWindow();
  void drawErrorPopup();
  void drawMainArea();
  void drawPostprocessesArea();
  void drawLightingArea();
  void drawModelArea();
  void drawCameraArea();
  void loadModelDialog();
};
}  // namespace Simple3D
