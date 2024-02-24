#include <filesystem>
#include <unordered_map>

#include <simple_3d_viewer/Mediator.hpp>

namespace Simple3D {
// GUI event handlers
void Mediator::notify(GUIEvent e) {
  switch (e) {
  case GUIEvent::PostprocessesControlsChange:
    processPostprocessesControlsChange();
    break;
  case GUIEvent::LightingControlsChange:
    processLightingControlsChange();
    break;
  case GUIEvent::VisualizeLightPositionCheckboxChange:
    processVisualizeLightPositionCheckboxChange();
    break;
  case GUIEvent::ModelControlsChange:
    processModelControlsChange();
    break;
  case GUIEvent::ModelLoadingConfigurationChange:
    processModelLoadingConfigurationChange();
    break;
  case GUIEvent::CameraControlsChange:
    processCameraControlsChange();
    break;
  case GUIEvent::LoadModel:
    processLoadModel();
    break;
  case GUIEvent::ReloadProgram:
    processReloadProgram();
    break;
  default:
    assert(false);
  }
}

void Mediator::processPostprocessesControlsChange() {
  for (const auto &postprocessCheckbox :
       imGuiWrapper_.getPostprocessesCheckboxes()) {
    viewer_.setPostprocessActiveFlag(postprocessCheckbox.text,
                                     postprocessCheckbox.value);
  }
}

void Mediator::processLightingControlsChange() {
  const auto &sliders = imGuiWrapper_.getLightControlsSliders();
  const glm::vec3 lightPosition{sliders[0].currentValue,
                                sliders[1].currentValue,
                                sliders[2].currentValue};
  viewer_.scene_.lightPosition = lightPosition;
}

void Mediator::processVisualizeLightPositionCheckboxChange() {
  const auto &checkboxes = imGuiWrapper_.getLightControlsCheckboxes();
  viewer_.renderer_.drawLight_ = checkboxes[0].value;
}

void Mediator::processModelControlsChange() {
  const auto &sliders = imGuiWrapper_.getModelControlsSliders();
  const glm::vec3 translation(sliders[0].currentValue, sliders[1].currentValue,
                              sliders[2].currentValue);
  const glm::vec3 rotation(sliders[3].currentValue, sliders[4].currentValue,
                           sliders[5].currentValue);
  const glm::vec3 scale(sliders[6].currentValue, sliders[7].currentValue,
                        sliders[8].currentValue);
  viewer_.setModelTransform({translation, rotation, scale});
}

static const std::unordered_map<std::string, Model::Configuration::Flag>
    stringToModelConfigurationFlag =
        {{"Flip UVs", Model::Configuration::Flag::FlipUVs}};

void Mediator::processModelLoadingConfigurationChange() {
  const auto &modelLoadingConfigurationCheckboxes =
      imGuiWrapper_.getModelLoadingConfigurationCheckboxes();
  for (const auto &checkbox : modelLoadingConfigurationCheckboxes) {
    viewer_.modelConfig_.set(stringToModelConfigurationFlag.at(checkbox.text), checkbox.value);
  }
}

void Mediator::processCameraControlsChange() {
  const auto &sliders = imGuiWrapper_.getCameraControlsSliders();
  viewer_.setCameraSettings(
      Camera::Settings{sliders[0].currentValue, sliders[1].currentValue});
}

void Mediator::processLoadModel() {
  std::filesystem::path path(imGuiWrapper_.getSelectedFilePath());
  viewer_.loadModel(path.generic_string());
}

void Mediator::processReloadProgram() { viewer_.reloadProgram(); }

// Viewer event handlers
void Mediator::notify(ViewerEvent e) {
  switch (e) {
  case ViewerEvent::ModelLoaded:
    processModelLoaded();
    break;
  default:
    assert(false);
  }
}

void Mediator::processModelLoaded() {
  const auto &sliders = imGuiWrapper_.getModelControlsSliders();
  assert(sliders.size() == 9);
  const glm::vec3 translation(sliders[0].currentValue, sliders[1].currentValue,
                              sliders[2].currentValue);
  const glm::vec3 rotation(sliders[3].currentValue, sliders[4].currentValue,
                           sliders[5].currentValue);
  const glm::vec3 scale(sliders[6].currentValue, sliders[7].currentValue,
                        sliders[8].currentValue);
  viewer_.setModelTransform({translation, rotation, scale});
}

// Viewer error handlers
void Mediator::notify(ViewerError e, const std::string &errorMessage) {
  switch (e) {
  case ViewerError::ReloadProgram:
    imGuiWrapper_.printError(errorMessage);
    break;
  case ViewerError::LoadModel:
    imGuiWrapper_.printError(errorMessage);
    break;
  default:
    assert(false);
  }
}
} // namespace Simple3D
