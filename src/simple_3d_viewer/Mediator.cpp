#include "simple_3d_viewer/ImGuiWrapper.hpp"
#include "simple_3d_viewer/Viewer.hpp"
#include "simple_3d_viewer/rendering/Model.hpp"
#include "simple_3d_viewer/utils/constants.hpp"
#include <filesystem>
#include <functional>
#include <simple_3d_viewer/Mediator.hpp>
#include <unordered_map>

namespace Simple3D
{

namespace
{

void handlePostprocessesControlsChange(
    const ImGuiWrapper& imGuiWrapper,
    Viewer& viewer)
{
  for (const auto& postprocessCheckbox :
       imGuiWrapper.getPostprocessesCheckboxes())
  {
    viewer.setPostprocessActiveFlag(
        postprocessCheckbox.text, postprocessCheckbox.value);
  }
}

void handleLightingControlsChange(
    const ImGuiWrapper& imGuiWrapper,
    Viewer& viewer)
{
  const auto& sliders = imGuiWrapper.getLightControlsSliders();
  const glm::vec3 lightPosition{ sliders[0].currentValue,
                                 sliders[1].currentValue,
                                 sliders[2].currentValue };
  viewer.getScene().lightPosition = lightPosition;
}

void handleVisualizeLightPositionCheckboxChange(
    const ImGuiWrapper& imGuiWrapper,
    Viewer& viewer)
{
  const auto& checkboxes = imGuiWrapper.getLightControlsCheckboxes();
  viewer.getRenderer().drawLight_ = checkboxes[0].value;
}

void handleModelControlsChange(const ImGuiWrapper& imGuiWrapper, Viewer& viewer)
{
  const auto& sliders = imGuiWrapper.getModelControlsSliders();
  const glm::vec3 translation(
      sliders[0].currentValue,
      sliders[1].currentValue,
      sliders[2].currentValue);
  const glm::vec3 rotation(
      sliders[3].currentValue,
      sliders[4].currentValue,
      sliders[5].currentValue);
  const glm::vec3 scale(
      sliders[6].currentValue,
      sliders[7].currentValue,
      sliders[8].currentValue);
  viewer.setModelTransform({ translation, rotation, scale });
}

struct StringHash
{
  using is_transparent = void;  // enables heterogenous lookup

  std::size_t operator()(std::string_view sv) const
  {
    std::hash<std::string_view> hasher;
    return hasher(sv);
  }
};

const std::unordered_map<
    std::string,
    Model::Configuration::Flag,
    StringHash,
    std::equal_to<>>
    kStringToModelConfigurationFlag = {
      { "Flip UVs", Model::Configuration::Flag::FlipUVs }
    };

void handleModelLoadingConfigurationChange(
    const ImGuiWrapper& imGuiWrapper,
    Viewer& viewer)
{
  const auto& modelLoadingConfigurationCheckboxes =
      imGuiWrapper.getModelLoadingConfigurationCheckboxes();
  for (const auto& checkbox : modelLoadingConfigurationCheckboxes)
  {
    viewer.getModelConfiguration().set(
        kStringToModelConfigurationFlag.at(checkbox.text), checkbox.value);
  }
}

void handleCameraControlsChange(
    const ImGuiWrapper& imGuiWrapper,
    Viewer& viewer)
{
  const auto& sliders = imGuiWrapper.getCameraControlsSliders();
  viewer.setCameraSettings(
      Camera::Settings{ sliders[0].currentValue, sliders[1].currentValue });
}

void handleLoadModel(const ImGuiWrapper& imGuiWrapper, Viewer& viewer)
{
  viewer.loadModel(imGuiWrapper.getModelFilePath());
}

void handleReloadProgram(
    [[maybe_unused]] const ImGuiWrapper& imGuiWrapper,
    Viewer& viewer)
{
  viewer.reloadProgram();
}

void handleModelLoaded(const ImGuiWrapper& imGuiWrapper, Viewer& viewer)
{
  const auto& sliders = imGuiWrapper.getModelControlsSliders();
  const glm::vec3 translation(
      sliders[0].currentValue,
      sliders[1].currentValue,
      sliders[2].currentValue);
  const glm::vec3 rotation(
      sliders[3].currentValue,
      sliders[4].currentValue,
      sliders[5].currentValue);
  const glm::vec3 scale(
      sliders[6].currentValue,
      sliders[7].currentValue,
      sliders[8].currentValue);
  viewer.setModelTransform({ translation, rotation, scale });
}

using enum ImGuiWrapper::Event;

const std::unordered_map<
    ImGuiWrapper::Event,
    std::function<void(const ImGuiWrapper&, Viewer&)>>
    kGUIEventHandlers{
      { ImGuiWrapper::Event::PostprocessesControlsChange,
        handlePostprocessesControlsChange },
      { ImGuiWrapper::Event::LightingControlsChange,
        handleLightingControlsChange },
      { ImGuiWrapper::Event::VisualizeLightPositionCheckboxChange,
        handleVisualizeLightPositionCheckboxChange },
      { ImGuiWrapper::Event::ModelControlsChange, handleModelControlsChange },
      { ImGuiWrapper::Event::ModelLoadingConfigurationChange,
        handleModelLoadingConfigurationChange },
      { ImGuiWrapper::Event::CameraControlsChange, handleCameraControlsChange },
      { ImGuiWrapper::Event::LoadModel, handleLoadModel },
      { ImGuiWrapper::Event::ReloadProgram, handleReloadProgram }
    };

using enum Viewer::Event;

const std::unordered_map<
    Viewer::Event,
    std::function<void(const ImGuiWrapper&, Viewer&)>>
    kViewerEventHandlers{ { Viewer::Event::ModelLoaded, handleModelLoaded } };

}  // namespace

void Mediator::notify(GUIEvent e)
{
  kGUIEventHandlers.at(e)(imGuiWrapper_, viewer_);
}

// Viewer event handlers
void Mediator::notify(ViewerEvent e)
{
  kViewerEventHandlers.at(e)(imGuiWrapper_, viewer_);
}

// Viewer error handlers
void Mediator::notify(ViewerError e, const std::string& errorMessage)
{
  switch (e)
  {
    case ViewerError::ReloadProgram:
      imGuiWrapper_.printError(errorMessage);
      break;
    case ViewerError::LoadModel: imGuiWrapper_.printError(errorMessage); break;
    default: assert(false);
  }
}

}  // namespace Simple3D
