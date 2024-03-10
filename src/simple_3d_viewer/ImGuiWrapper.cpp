#include <glad/glad.h>

#include "simple_3d_viewer/Mediator.hpp"
#include <ImGuiFileDialog.h>
#include <filesystem>
#include <iterator>
#include <mutex>
#include <optional>
#include <simple_3d_viewer/ImGuiWrapper.hpp>
#include <stdexcept>

namespace Simple3D
{

namespace
{

void init(GLFWwindow* window)
{
  // Setup ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char* glslVersion = "#version 330";
  ImGui_ImplOpenGL3_Init(glslVersion);
}

void sync(ImGuiWrapper::Mediator& mediator)
{
  using enum Simple3D::ImGuiWrapper::Event;
  mediator.notify(PostprocessesControlsChange);
  mediator.notify(LightingControlsChange);
  mediator.notify(VisualizeLightPositionCheckboxChange);
  mediator.notify(CameraControlsChange);
  mediator.notify(ModelLoadingConfigurationChange);
}

bool BeginPopupCentered(const std::string& name)
{
  const ImGuiIO& io = ImGui::GetIO();
  const ImVec2 pos(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
  ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  const int width = 400;
  const int height = 100;
  const ImVec2 size(width, height);
  ImGui::SetNextWindowSize(size);
  const ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar |
                                 ImGuiWindowFlags_HorizontalScrollbar |
                                 ImGuiWindowFlags_NoSavedSettings;
  return ImGui::BeginPopup(name.c_str(), flags);
}

void drawErrorPopup(const std::string& errorMessage)
{
  if (BeginPopupCentered("errorPopup"))
  {
    ImGui::Text("%s", errorMessage.c_str());
    ImGui::End();
  }
}

bool drawCheckboxes(Checkboxes& checkboxes)
{
  bool changed = false;
  for (auto& checkbox : checkboxes)
  {
    changed =
        ImGui::Checkbox(checkbox.text.c_str(), &checkbox.value) || changed;
  }
  return changed;
}

bool drawCheckbox(Checkbox& checkbox)
{
  return ImGui::Checkbox(checkbox.text.c_str(), &checkbox.value);
}

bool drawSliders(Sliders& sliders)
{
  bool changed = false;
  for (auto& slider : sliders)
  {
    changed = ImGui::SliderFloat(
                  slider.text.c_str(),
                  &slider.currentValue,
                  slider.minValue,
                  slider.maxValue) ||
              changed;
  }
  return changed;
}

void resetSliders(Sliders& sliders)
{
  for (auto& slider : sliders)
  {
    slider.currentValue = slider.defaultValue;
  }
}

void drawMainArea()
{
  ImGui::Text(
      "Application average %.1f FPS",
      static_cast<double>(ImGui::GetIO().Framerate));
  ImGui::Text("Made by: Sinisa Markovic");
  ImGui::Separator();
}

void drawPostprocessesArea(
    Checkboxes& postprocessesCheckboxes,
    ImGuiWrapper::Mediator& mediator)
{
  ImGui::Text("Postprocesses controls:");
  if (drawCheckboxes(postprocessesCheckboxes))
  {
    mediator.notify(ImGuiWrapper::Event::PostprocessesControlsChange);
  }
  ImGui::Separator();
}

void drawLightingArea(
    Sliders& lightControlsSliders,
    Checkboxes& lightControlsCheckboxes,
    ImGuiWrapper::Mediator& mediator)
{
  ImGui::Text("Light controls:");
  if (drawSliders(lightControlsSliders))
  {
    mediator.notify(ImGuiWrapper::Event::LightingControlsChange);
  }
  if (ImGui::Button("Reset light position"))
  {
    resetSliders(lightControlsSliders);
    mediator.notify(ImGuiWrapper::Event::LightingControlsChange);
  }

  ImGui::Spacing();
  if (drawCheckboxes(lightControlsCheckboxes))
  {
    mediator.notify(ImGuiWrapper::Event::VisualizeLightPositionCheckboxChange);
  }
  ImGui::Separator();
}

std::optional<std::filesystem::path> loadModelDialog()
{
  if (ImGui::Button("Load model"))
  {
    ImGuiFileDialog::Instance()->OpenDialog(
        "ChooseFileDlgKey",
        "Choose a 3D model file",
        ".obj,.gltf,.fbx,.dxf",
        ".");
  }

  const int width = 400;
  const int height = 360;
  const ImVec2 size(width, height);
  std::optional<std::filesystem::path> result;
  if (ImGuiFileDialog::Instance()->Display(
          "ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, size, size))
  {
    if (ImGuiFileDialog::Instance()->IsOk())
    {
      result = ImGuiFileDialog::Instance()->GetFilePathName();
    }

    ImGuiFileDialog::Instance()->Close();
  }

  return result;
}

std::optional<std::filesystem::path> drawModelArea(
    Sliders& modelTransformSliders,
    Checkboxes& modelLoadingConfigurationCheckboxes,
    ImGuiWrapper::Mediator& mediator)
{
  ImGui::Text("Model controls:");
  if (drawSliders(modelTransformSliders))
  {
    mediator.notify(ImGuiWrapper::Event::ModelControlsChange);
  }
  if (ImGui::Button("Reset model transform"))
  {
    resetSliders(modelTransformSliders);
    mediator.notify(ImGuiWrapper::Event::ModelControlsChange);
  }

  ImGui::Spacing();
  ImGui::Text("Model loading configuration:");
  if (drawCheckboxes(modelLoadingConfigurationCheckboxes))
  {
    mediator.notify(ImGuiWrapper::Event::ModelLoadingConfigurationChange);
  }

  auto result = loadModelDialog();
  ImGui::SameLine();
  if (ImGui::Button("Reload model shaders"))
  {
    mediator.notify(ImGuiWrapper::Event::ReloadProgram);
  }
  ImGui::Separator();

  return result;
}

void drawCameraArea(
    Sliders& cameraControlsSliders,
    ImGuiWrapper::Mediator& mediator)
{
  ImGui::Text("Camera controls:");
  if (drawSliders(cameraControlsSliders))
  {
    mediator.notify(ImGuiWrapper::Event::CameraControlsChange);
  }

  if (ImGui::Button("Reset camera controls"))
  {
    resetSliders(cameraControlsSliders);
    mediator.notify(ImGuiWrapper::Event::CameraControlsChange);
  }
}

}  // namespace

ImGuiWrapper::ImGuiWrapper(
    GLFWwindow* window,
    const std::vector<std::string>& postprocesses)
    : postprocessesCheckboxes_(
          [&postprocesses]()
          {
            std::vector<Checkbox> postprocessesCheckboxes;
            std::ranges::transform(
                postprocesses,
                std::back_inserter(postprocessesCheckboxes),
                [](const std::string& postprocess) {
                  return Checkbox{ postprocess, false };
                });
            return postprocessesCheckboxes;
          }()),
      lightControlsSliders_{ { "Position X##1", 0.f, 0.f, -100.f, 100.f },
                             { "Position Y##1", 0.f, 0.f, -100.f, 100.f },
                             { "Position Z##1", 0.f, 0.f, -100.f, 100.f } },
      lightControlsCheckboxes_{ { "Visualize light position", true } },
      modelTransformSliders_{ { "Position X", 0.f, 0.f, -100.f, 100.f },
                              { "Position Y", 0.f, 0.f, -100.f, 100.f },
                              { "Position Z", 0.f, 0.f, -100.f, 100.f },
                              { "Rotation X", 0.f, 0.f, 0.f, 360.f },
                              { "Rotation Y", 0.f, 0.f, 0.f, 360.f },
                              { "Rotation Z", 0.f, 0.f, 0.f, 360.f },
                              { "Scale X", 1.f, 1.f, 0.f, 100.f },
                              { "Scale Y", 1.f, 1.f, 0.f, 100.f },
                              { "Scale Z", 1.f, 1.f, 0.f, 100.f } },
      modelLoadingConfigurationCheckboxes_{ { "Flip UVs", false } },
      cameraControlsSliders_{ { "Speed", 2.f, 2.f, 0.1f, 100.f },
                              { "Sensitivity", 5.0f, 5.0f, 0.1f, 20.f } }
{
  init(window);
}

void ImGuiWrapper::update()
{
  static std::once_flag sync_flag;
  std::call_once(sync_flag, sync, *mediator_);

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  drawSettingsWindow();
  drawErrorPopup(cachedErrorMessage_);
  ImGui::End();
}

void ImGuiWrapper::drawSettingsWindow()
{
  ImGui::Begin("Settings", nullptr);
  auto& mediator = *mediator_;
  drawMainArea();
  drawPostprocessesArea(postprocessesCheckboxes_, mediator);
  drawLightingArea(lightControlsSliders_, lightControlsCheckboxes_, mediator);
  if (auto maybeModelFilePath = drawModelArea(
          modelTransformSliders_,
          modelLoadingConfigurationCheckboxes_,
          mediator);
      maybeModelFilePath.has_value())
  {
    modelFilePath_ = *maybeModelFilePath;
    mediator.notify(Event::LoadModel);
  }
  drawCameraArea(cameraControlsSliders_, mediator);
}

void ImGuiWrapper::render()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiWrapper::release()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

}  // namespace Simple3D