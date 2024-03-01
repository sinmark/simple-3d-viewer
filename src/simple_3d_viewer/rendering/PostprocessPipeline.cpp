#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <simple_3d_viewer/rendering/PostprocessPipeline.hpp>

namespace Simple3D
{
const GLuint screenTextureSlot = 4;

static std::unordered_map<PostprocessID, Postprocess> createIDToPostprocessMap(
    const std::vector<PostprocessID>& postprocessIDs)
{
  std::unordered_map<PostprocessID, Postprocess> idToPostprocess;
  const std::string vertexShaderFilename = "postprocess";
  for (const auto& id : postprocessIDs)
  {
    auto lowerCaseId = id;
    std::ranges::transform(
        lowerCaseId,
        begin(lowerCaseId),
        [](unsigned char c) -> char { return std::tolower(c); });
    idToPostprocess.emplace(std::pair{
        id, Postprocess{ Program(vertexShaderFilename, lowerCaseId), false } });
  }

  return idToPostprocess;
}

PostprocessPipeline::PostprocessPipeline(
    const std::vector<std::string>& postprocessIDs,
    const Size framebufferSize)
    : idToPostprocess_(createIDToPostprocessMap(postprocessIDs)),
      size_(framebufferSize)
{
  init();
}

void PostprocessPipeline::init()
{
  initPostprocesses();
  const auto postprocessesCount = idToPostprocess_.size();
  postprocessesOrder_.reserve(postprocessesCount);
  framebuffers_.resize(postprocessesCount);
  colorBuffers_.resize(postprocessesCount);
  renderBuffers_.resize(postprocessesCount);
  updatePostprocesses();
  makeFramebuffers();
  makeScreenQuad();
}

void PostprocessPipeline::initPostprocesses()
{
  for (auto& [id, postprocess] : idToPostprocess_)
  {
    if (postprocess.program.hasUniform("inverseScreenSize"))
      postprocessesToUpdate_.push_back(&postprocess.program);
    postprocess.program.doOperations(
        [](Program& program)
        { program.setInt("screenTexture", screenTextureSlot); });
  }
}

void PostprocessPipeline::makeFramebuffers()
{
  const auto framebuffersCount = static_cast<GLsizei>(framebuffers_.size());
  glGenFramebuffers(framebuffersCount, framebuffers_.data());
  glGenTextures(framebuffersCount, colorBuffers_.data());
  glGenRenderbuffers(framebuffersCount, renderBuffers_.data());

  for (int i = 0; i < framebuffers_.size(); ++i)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers_[i]);

    glBindTexture(GL_TEXTURE_2D, colorBuffers_[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        size_.width,
        size_.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        colorBuffers_[i],
        0);

    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffers_[i]);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size_.width, size_.height);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        renderBuffers_[i]);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Framebuffer is not complete!" << '\n';
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostprocessPipeline::makeScreenQuad()
{
  static constexpr std::array screenQuadVertices = {
    -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f
  };

  glGenVertexArrays(1, &screenQuadVAO_);
  glGenBuffers(1, &screenQuadVBO_);
  glBindVertexArray(screenQuadVAO_);
  glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO_);
  glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(screenQuadVertices),
      &screenQuadVertices,
      GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PostprocessPipeline::start()
{
  if (postprocessesOrder_.empty())
    return;
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffers_.front());
}

void PostprocessPipeline::finalize()
{
  if (postprocessesOrder_.empty())
    return;

  glBindVertexArray(screenQuadVAO_);
  glDisable(GL_DEPTH_TEST);
  glActiveTexture(GL_TEXTURE0 + screenTextureSlot);

  int colorBufferToUse = 0;
  for (int i = 0; i < postprocessesOrder_.size() - 1; ++i)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers_[i + 1]);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, colorBuffers_[i]);
    postprocessesOrder_[i]->doOperations([](Program& program)
                                         { glDrawArrays(GL_TRIANGLES, 0, 6); });
    ++colorBufferToUse;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, colorBuffers_[colorBufferToUse]);

  postprocessesOrder_.back()->doOperations(
      [](Program& program) { glDrawArrays(GL_TRIANGLES, 0, 6); });

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void PostprocessPipeline::updatePostprocesses()
{
  for (Program* program : postprocessesToUpdate_)
    program->doOperations(
        [size = size_](Program& program)
        {
          program.setVec3f(
              "inverseScreenSize",
              glm::vec3(1.0f / size.width, 1.0f / size.height, 1.0f));
        });
}

void PostprocessPipeline::resizeFramebuffersAttachments()
{
  for (int i = 0; i < framebuffers_.size(); ++i)
  {
    glBindTexture(GL_TEXTURE_2D, colorBuffers_[i]);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        size_.width,
        size_.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffers_[i]);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size_.width, size_.height);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void PostprocessPipeline::releaseFramebuffers()
{
  const auto framebuffersCount = static_cast<GLsizei>(framebuffers_.size());
  glDeleteFramebuffers(framebuffersCount, framebuffers_.data());
  glDeleteTextures(framebuffersCount, colorBuffers_.data());
  glDeleteRenderbuffers(framebuffersCount, renderBuffers_.data());

  framebuffers_.clear();
  colorBuffers_.clear();
  renderBuffers_.clear();
}

void PostprocessPipeline::releaseScreenQuad()
{
  glDeleteBuffers(1, &screenQuadVBO_);
  glDeleteVertexArrays(1, &screenQuadVAO_);
}
}  // namespace Simple3D
