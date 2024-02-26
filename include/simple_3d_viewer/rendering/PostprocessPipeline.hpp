#pragma once

#include <glad/glad.h>

#include <memory>
#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/utils/Size.hpp>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace Simple3D
{
struct Postprocess
{
  Program program;
  bool active;
};
using PostprocessID = std::string;

class PostprocessPipeline
{
 public:
  using OnResizeFunction = std::function<void(Size)>;

  PostprocessPipeline(
      const std::vector<PostprocessID>& postprocessIDs,
      Size size);
  PostprocessPipeline(const PostprocessPipeline&) = delete;
  PostprocessPipeline(PostprocessPipeline&& postprocessPipeline) noexcept
      : idToPostprocess_(std::move(postprocessPipeline.idToPostprocess_)),
        postprocessesOrder_(std::move(postprocessPipeline.postprocessesOrder_)),
        postprocessesToUpdate_(
            std::move(postprocessPipeline.postprocessesToUpdate_)),
        size_(postprocessPipeline.size_),
        framebuffers_(std::move(postprocessPipeline.framebuffers_)),
        colorBuffers_(std::move(postprocessPipeline.colorBuffers_)),
        renderBuffers_(std::move(postprocessPipeline.renderBuffers_)),
        screenQuadVBO_(postprocessPipeline.screenQuadVBO_),
        screenQuadVAO_(postprocessPipeline.screenQuadVAO_)
  {
    postprocessPipeline.screenQuadVBO_ = 0;
    postprocessPipeline.screenQuadVAO_ = 0;
  }
  PostprocessPipeline& operator=(const PostprocessPipeline&) = delete;
  PostprocessPipeline& operator=(PostprocessPipeline&&) = delete;
  ~PostprocessPipeline()
  {
    release();
  }

  void start();
  void finalize();
  void setPostprocessActiveFlag(const std::string& id, bool active)
  {
    auto& postprocess = idToPostprocess_.at(id);
    if (postprocess.active == active)
      return;
    postprocess.active = active;

    Program* program = &postprocess.program;
    if (postprocess.active)
      postprocessesOrder_.push_back(program);
    else
      postprocessesOrder_.erase(
          std::remove(
              postprocessesOrder_.begin(), postprocessesOrder_.end(), program),
          postprocessesOrder_.end());
  }
  void resize(Size size)
  {
    if (size == size_)
      return;
    size_ = size;
    updatePostprocesses();

    resizeFramebuffersAttachments();
  }
  void release()
  {
    releaseFramebuffers();
    releaseScreenQuad();
  }

 private:
  std::unordered_map<PostprocessID, Postprocess> idToPostprocess_;
  std::vector<Program*> postprocessesOrder_;
  std::vector<Program*> postprocessesToUpdate_;
  Size size_;
  std::vector<GLuint> framebuffers_;
  std::vector<GLuint> colorBuffers_;
  std::vector<GLuint> renderBuffers_;
  GLuint screenQuadVAO_ = 0;
  GLuint screenQuadVBO_ = 0;

  void init();
  void initPostprocesses();
  void makeFramebuffers();
  void makeScreenQuad();
  void resizeFramebuffersAttachments();
  void releaseFramebuffers();
  void releaseScreenQuad();
  void updatePostprocesses();
};
}  // namespace Simple3D
