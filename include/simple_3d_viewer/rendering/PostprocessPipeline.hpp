#pragma once

#include <simple_3d_viewer/opengl_object_wrappers/Program.hpp>
#include <simple_3d_viewer/utils/Size.hpp>
#include <unordered_map>
#include <vector>

namespace Simple3D
{

struct Postprocess
{
  Program program;
  bool active;
};

struct Framebuffers
{
  explicit Framebuffers(size_t framebuffersCount)
      : framebuffers(framebuffersCount),
        colorBuffers(framebuffersCount),
        renderBuffers(framebuffersCount)
  {
  }

  std::vector<GLuint> framebuffers;
  std::vector<GLuint> colorBuffers;
  std::vector<GLuint> renderBuffers;
};

struct ScreenQuad
{
  GLuint vbo;
  GLuint vao;
};

using PostprocessID = std::string;

class PostprocessPipeline
{
 public:
  using OnResizeFunction = std::function<void(Size)>;

  PostprocessPipeline(
      const std::vector<PostprocessID>& postprocessIDs,
      Size framebufferSize);
  PostprocessPipeline(const PostprocessPipeline&) = delete;
  PostprocessPipeline& operator=(const PostprocessPipeline&) = delete;
  PostprocessPipeline(PostprocessPipeline&& other) noexcept
      : idToPostprocess_(std::move(other.idToPostprocess_)),
        postprocessesOrder_(std::move(other.postprocessesOrder_)),
        postprocessesToUpdate_(std::move(other.postprocessesToUpdate_)),
        framebuffers_(std::move(other.framebuffers_)),
        screenQuad_(other.screenQuad_)
  {
    other.screenQuad_.vbo = 0;
    other.screenQuad_.vao = 0;
  }
  PostprocessPipeline& operator=(PostprocessPipeline&&) = delete;
  ~PostprocessPipeline()
  {
    release();
  }

  void start();

  void finalize();

  void setPostprocessActiveFlag(const PostprocessID& id, bool active);

  void resize(Size framebufferSize);

  void release();

 private:
  std::unordered_map<PostprocessID, Postprocess> idToPostprocess_;
  std::vector<PostprocessID> postprocessesOrder_;
  std::vector<PostprocessID> postprocessesToUpdate_;
  Framebuffers framebuffers_;
  ScreenQuad screenQuad_;
};

}  // namespace Simple3D
