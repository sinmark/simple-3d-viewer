#include <glad/glad.h>

#include <ImGuiFileDialog.h>
#include <iterator>
#include <mutex>
#include <simple_3d_viewer/ImGuiWrapper.hpp>

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

}  // namespace

ImGuiWrapper::ImGuiWrapper(
    GLFWwindow* window,
    const std::vector<std::string>& postprocesses)
    : lightControlsSliders_{ { "Position X##1", 0.f, 0.f, -100.f, 100.f },
                             { "Position Y##1", 0.f, 0.f, -100.f, 100.f },
                             { "Position Z##1", 0.f, 0.f, -100.f, 100.f } },
      modelTransformSliders_{ { "Position X", 0.f, 0.f, -100.f, 100.f },
                              { "Position Y", 0.f, 0.f, -100.f, 100.f },
                              { "Position Z", 0.f, 0.f, -100.f, 100.f },
                              { "Rotation X", 0.f, 0.f, 0.f, 360.f },
                              { "Rotation Y", 0.f, 0.f, 0.f, 360.f },
                              { "Rotation Z", 0.f, 0.f, 0.f, 360.f },
                              { "Scale X", 1.f, 1.f, 0.f, 100.f },
                              { "Scale Y", 1.f, 1.f, 0.f, 100.f },
                              { "Scale Z", 1.f, 1.f, 0.f, 100.f } },
      cameraControlsSliders_{ { "Speed", 2.f, 2.f, 0.1f, 100.f },
                              { "Sensitivity", 0.2f, 0.2f, 0.1f, 10.f } },
      postprocessesCheckboxes_(
          [&postprocesses]()
          {
            std::vector<Checkbox> postprocessesCheckboxes;
            std::transform(
                postprocesses.begin(),
                postprocesses.end(),
                std::back_inserter(postprocessesCheckboxes),
                [](const std::string& postprocess) {
                  return Checkbox{ postprocess, false };
                });
            return postprocessesCheckboxes;
          }()),
      lightControlsCheckboxes_{ { "Visualize light position", true } },
      modelLoadingConfigurationCheckboxes_{ { "Flip UVs", false } }
{
  init(window);
}

void ImGuiWrapper::update()
{
  assert(mediator_);
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
  drawMainArea();
  drawPostprocessesArea(postprocessesCheckboxes_, *mediator_);
  drawLightingArea();
  drawModelArea();
  drawCameraArea();
}

void ImGuiWrapper::drawLightingArea()
{
  ImGui::Text("Light controls:");
  if (drawSliders(lightControlsSliders_))
  {
    mediator_->notify(Event::LightingControlsChange);
  }
  if (ImGui::Button("Reset light position"))
  {
    resetSliders(lightControlsSliders_);
    mediator_->notify(Event::LightingControlsChange);
  }

  ImGui::Spacing();
  if (drawCheckbox(lightControlsCheckboxes_[0]))
  {
    mediator_->notify(Event::VisualizeLightPositionCheckboxChange);
  }
  ImGui::Separator();
}

void ImGuiWrapper::drawModelArea()
{
  ImGui::Text("Model controls:");
  if (drawSliders(modelTransformSliders_))
  {
    mediator_->notify(Event::ModelControlsChange);
  }
  if (ImGui::Button("Reset model transform"))
  {
    resetSliders(modelTransformSliders_);
    mediator_->notify(Event::ModelControlsChange);
  }

  ImGui::Spacing();
  ImGui::Text("Model loading configuration:");
  if (drawCheckboxes(modelLoadingConfigurationCheckboxes_))
  {
    mediator_->notify(Event::ModelLoadingConfigurationChange);
  }

  loadModelDialog();
  ImGui::SameLine();
  if (ImGui::Button("Reload model shaders"))
  {
    mediator_->notify(Event::ReloadProgram);
  }
  ImGui::Separator();
}

void ImGuiWrapper::drawCameraArea()
{
  ImGui::Text("Camera controls:");
  if (drawSliders(cameraControlsSliders_))
    mediator_->notify(Event::CameraControlsChange);
  if (ImGui::Button("Reset camera controls"))
  {
    resetSliders(cameraControlsSliders_);
    mediator_->notify(Event::CameraControlsChange);
  }
}

void ImGuiWrapper::loadModelDialog()
{
  if (ImGui::Button("Load model"))
    ImGuiFileDialog::Instance()->OpenDialog(
        "ChooseFileDlgKey",
        "Choose a 3D model file",
        ".obj,.gltf,.fbx,.dxf",
        ".");

  const int width = 400, height = 360;
  const ImVec2 size(width, height);
  if (ImGuiFileDialog::Instance()->Display(
          "ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, size, size))
  {
    if (ImGuiFileDialog::Instance()->IsOk())
    {
      filePath_ = ImGuiFileDialog::Instance()->GetFilePathName();
      mediator_->notify(Event::LoadModel);
    }

    ImGuiFileDialog::Instance()->Close();
  }
}
}  // namespace Simple3D