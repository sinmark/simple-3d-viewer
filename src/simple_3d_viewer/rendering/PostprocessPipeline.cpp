#include <glad/glad.h>

#include "simple_3d_viewer/utils/StringHeterogeneousLookup.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <fmt/core.h>
#include <iostream>
#include <range/v3/algorithm/remove.hpp>
#include <simple_3d_viewer/rendering/PostprocessPipeline.hpp>
#include <simple_3d_viewer/utils/factories.hpp>
#include <unordered_map>
#include <utility>

namespace Simple3D
{

namespace
{

constexpr GLuint kScreenTextureSlot = 4;

StringHeterogeneousLookupUnorderedMap<Postprocess> createIDToPostprocessMap(
    const std::vector<PostprocessID>& postprocessIDs)
{
  StringHeterogeneousLookupUnorderedMap<Postprocess> idToPostprocess;
  const std::string vertexShaderFilename = "postprocess";
  for (const auto& id : postprocessIDs)
  {
    auto lowerCaseId = id;
    std::ranges::transform(
        lowerCaseId,
        begin(lowerCaseId),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    idToPostprocess.try_emplace(
        id, createProgram(vertexShaderFilename, lowerCaseId), false);
  }

  return idToPostprocess;
}

Framebuffers createFramebuffers(
    const size_t framebuffersCount,
    const Size framebufferSize)
{
  Framebuffers framebuffers(framebuffersCount);
  glGenFramebuffers(
      static_cast<GLsizei>(framebuffersCount),
      framebuffers.framebuffers.data());
  glGenTextures(
      static_cast<GLsizei>(framebuffersCount),
      framebuffers.colorBuffers.data());
  glGenRenderbuffers(
      static_cast<GLsizei>(framebuffersCount),
      framebuffers.renderBuffers.data());

  for (auto i = decltype(framebuffersCount){}; i < framebuffersCount; ++i)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.framebuffers[i]);

    glBindTexture(GL_TEXTURE_2D, framebuffers.colorBuffers[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        framebufferSize.width,
        framebufferSize.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        framebuffers.colorBuffers[i],
        0);

    glBindRenderbuffer(GL_RENDERBUFFER, framebuffers.renderBuffers[i]);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        framebufferSize.width,
        framebufferSize.height);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        framebuffers.renderBuffers[i]);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      fmt::println(stderr, "Framebuffer is not complete");
    }
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return framebuffers;
}

void updatePostprocesses(
    const std::vector<PostprocessID>& postprocessesToUpdate,
    StringHeterogeneousLookupUnorderedMap<Postprocess>& idToPostprocess,
    const Size framebufferSize)
{
  for (const auto& id : postprocessesToUpdate)
  {
    auto& program = idToPostprocess.at(id).program;
    program.doOperations(
        [framebufferSize](Program& it)
        {
          it.setVec3f(
              "inverseScreenSize",
              { 1.0f / static_cast<float>(framebufferSize.width),
                1.0f / static_cast<float>(framebufferSize.height),
                1.0f });
        });
  }
}

std::vector<PostprocessID> initPostprocesses(
    StringHeterogeneousLookupUnorderedMap<Postprocess>& idToPostprocess,
    const Size framebufferSize)
{
  std::vector<PostprocessID> postprocessesToUpdate;

  for (auto& [id, postprocess] : idToPostprocess)
  {
    if (postprocess.program.hasUniform("inverseScreenSize"))
    {
      postprocessesToUpdate.push_back(id);
    }
    postprocess.program.doOperations(
        [](Program& program)
        { program.setInt("screenTexture", kScreenTextureSlot); });
  }

  updatePostprocesses(postprocessesToUpdate, idToPostprocess, framebufferSize);

  return postprocessesToUpdate;
}

void resizeFramebuffersAttachments(
    Framebuffers& framebuffers,
    const Size framebufferSize)
{
  const auto size = framebuffers.colorBuffers.size();
  for (auto i = decltype(size){}; i < size; ++i)
  {
    glBindTexture(GL_TEXTURE_2D, framebuffers.colorBuffers[i]);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        framebufferSize.width,
        framebufferSize.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, framebuffers.renderBuffers[i]);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        framebufferSize.width,
        framebufferSize.height);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

ScreenQuad createScreenQuad()
{
  static constexpr std::array screenQuadVertices = {
    -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f
  };

  ScreenQuad screenQuad{};
  glGenVertexArrays(1, &screenQuad.vao);
  glGenBuffers(1, &screenQuad.vbo);
  glBindVertexArray(screenQuad.vao);
  glBindBuffer(GL_ARRAY_BUFFER, screenQuad.vbo);
  glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(screenQuadVertices),
      &screenQuadVertices,
      GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return screenQuad;
}

}  // namespace

PostprocessPipeline::PostprocessPipeline(
    const std::vector<std::string>& postprocessIDs,
    const Size framebufferSize)
    : idToPostprocess_(createIDToPostprocessMap(postprocessIDs)),
      postprocessesToUpdate_(
          initPostprocesses(idToPostprocess_, framebufferSize)),
      framebuffers_(createFramebuffers(postprocessIDs.size(), framebufferSize)),
      screenQuad_(createScreenQuad())
{
}

void PostprocessPipeline::start()
{
  if (postprocessesOrder_.empty())
  {
    return;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffers_.framebuffers.front());
}

void PostprocessPipeline::finalize()
{
  if (postprocessesOrder_.empty())
  {
    return;
  }

  glBindVertexArray(screenQuad_.vao);
  glDisable(GL_DEPTH_TEST);
  glActiveTexture(GL_TEXTURE0 + kScreenTextureSlot);

  size_t colorBufferToUse = 0;
  const auto postprocessesCount = postprocessesOrder_.size();
  for (auto i = decltype(postprocessesCount){}; i < postprocessesCount - 1; ++i)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers_.framebuffers[i + 1]);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, framebuffers_.colorBuffers[i]);

    auto& program = idToPostprocess_.at(postprocessesOrder_[i]).program;
    program.doOperations([](Program& /*unused*/)
                         { glDrawArrays(GL_TRIANGLES, 0, 6); });

    ++colorBufferToUse;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, framebuffers_.colorBuffers[colorBufferToUse]);

  auto& program = idToPostprocess_.at(postprocessesOrder_.back()).program;
  program.doOperations([](Program& /*unused*/)
                       { glDrawArrays(GL_TRIANGLES, 0, 6); });

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void PostprocessPipeline::resize(Size framebufferSize)
{
  updatePostprocesses(
      postprocessesToUpdate_, idToPostprocess_, framebufferSize);

  resizeFramebuffersAttachments(framebuffers_, framebufferSize);
}

namespace
{
void releaseFramebuffers(Framebuffers& framebuffers)
{
  const auto framebuffersCount =
      static_cast<GLsizei>(framebuffers.framebuffers.size());
  glDeleteFramebuffers(framebuffersCount, framebuffers.framebuffers.data());
  glDeleteTextures(framebuffersCount, framebuffers.colorBuffers.data());
  glDeleteRenderbuffers(framebuffersCount, framebuffers.renderBuffers.data());

  framebuffers.framebuffers.clear();
  framebuffers.colorBuffers.clear();
  framebuffers.renderBuffers.clear();
}

void releaseScreenQuad(ScreenQuad& screenQuad)
{
  glDeleteBuffers(1, &screenQuad.vbo);
  glDeleteVertexArrays(1, &screenQuad.vao);
  screenQuad.vbo = 0;
  screenQuad.vao = 0;
}

}  // namespace

void PostprocessPipeline::release()
{
  releaseFramebuffers(framebuffers_);
  releaseScreenQuad(screenQuad_);
}

void PostprocessPipeline::setPostprocessActiveFlag(
    const PostprocessID& id,
    bool active)
{
  auto& postprocess = idToPostprocess_.at(id);
  if (postprocess.active == active)
  {
    return;
  }
  postprocess.active = active;

  if (postprocess.active)
  {
    postprocessesOrder_.push_back(id);
    return;
  }

  // std::ranges::remove seems to not work correctly so until it is fixed this
  // is the way
  postprocessesOrder_.erase(
      ranges::remove(postprocessesOrder_, id), end(postprocessesOrder_));
}

}  // namespace Simple3D
