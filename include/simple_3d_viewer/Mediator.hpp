#pragma once

#include <simple_3d_viewer/ImGuiWrapper.hpp>
#include <simple_3d_viewer/Viewer.hpp>

namespace Simple3D
{
class Mediator : public ImGuiWrapper::Mediator, public Viewer::Mediator
{
 public:
  using GUIEvent = ImGuiWrapper::Event;
  using ViewerEvent = Viewer::Event;
  using ViewerError = Viewer::Error;

  Mediator(ImGuiWrapper& imGuiWrapper, Viewer& viewer)
      : imGuiWrapper_(imGuiWrapper),
        viewer_(viewer)
  {
    imGuiWrapper_.setMediator(this);
    viewer_.setMediator(this);
  }

  void notify(GUIEvent e) override;
  void notify(ViewerEvent e) override;
  void notify(ViewerError e, const std::string& errorMessage) override;

 private:
  Viewer& viewer_;
  ImGuiWrapper& imGuiWrapper_;

  // GUI event handlers
  void processPostprocessesControlsChange();
  void processLightingControlsChange();
  void processVisualizeLightPositionCheckboxChange();
  void processModelControlsChange();
  void processModelLoadingConfigurationChange();
  void processCameraControlsChange();
  void processLoadModel();
  void processReloadProgram();

  // Viewer event handlers
  void processModelLoaded();
};
}  // namespace Simple3D