#pragma once

#include <memory>
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

  static void setupCommunication(ImGuiWrapper& imGuiWrapper, Viewer& viewer)
  {
    auto mediator = std::make_shared<Mediator>(Mediator(imGuiWrapper, viewer));
    imGuiWrapper.setMediator(mediator);
    viewer.setMediator(std::move(mediator));
  }

  void notify(GUIEvent e) override;
  void notify(ViewerEvent e) override;
  void notify(ViewerError e, const std::string& errorMessage) override;

 private:
  Mediator(ImGuiWrapper& imGuiWrapper, Viewer& viewer)
      : viewer_(viewer),
        imGuiWrapper_(imGuiWrapper)
  {
  }

  Viewer& viewer_;
  ImGuiWrapper& imGuiWrapper_;
};

}  // namespace Simple3D