#include <simple_3d_viewer/linear_algebra/Transform.hpp>
#include <simple_3d_viewer/rendering/Renderer.hpp>

namespace Simple3D
{
void Renderer::render(Scene& scene, const Size framebufferSize)
{
  performCacheChecks(scene, framebufferSize);
  postprocessPipeline_.start();
  glEnable(GL_DEPTH_TEST);
  static constexpr auto clearColor = 0.01f;
  glClearColor(clearColor, clearColor, clearColor, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (scene.model)
    render(scene.model.value(), scene.modelProgram);
  if (drawLight_)
    render(scene.light, scene.lightProgram);
  renderSkybox(scene.skybox, scene.skyboxProgram, scene.skyboxTexture);
  postprocessPipeline_.finalize();
}

void Renderer::performCacheChecks(Scene& scene, const Size framebufferSize)
{
  auto& modelProgramUniformsCache = cache_.modelProgramUniformsCache;
  const auto projection = calculateProjectionTransform(framebufferSize);
  const auto view = scene.camera.getViewTransform();
  modelProgramUniformsCache.modelProgramID.update(
      scene.modelProgram.getId(),
      [&modelProgramUniformsCache]() { modelProgramUniformsCache.clear(); });
  cache_.framebufferSize.update(
      framebufferSize,
      [&modelProgramUniformsCache,
       framebufferSize,
       &postprocessPipeline = postprocessPipeline_]()
      {
        modelProgramUniformsCache.clear();
        glViewport(0, 0, framebufferSize.width, framebufferSize.height);
        postprocessPipeline.resize(framebufferSize);
      });
  modelProgramUniformsCache.projectionViewTransform.update(
      { projection, view },
      [&projection, &view, &scene]()
      {
        const auto projectionView = projection * view;
        scene.modelProgram.doOperations(
            [&projectionView](Program& program)
            { program.setMat4f("pv", projectionView); });
        scene.lightProgram.doOperations(
            [&projectionView](Program& program)
            { program.setMat4f("pv", projectionView); });
        const auto viewWithoutTranslation = glm::mat4(glm::mat3(view));
        const auto projectionModifiedView = projection * viewWithoutTranslation;
        scene.skyboxProgram.doOperations(
            [&projectionModifiedView](Program& program)
            { program.setMat4f("pv", projectionModifiedView); });
      });
  modelProgramUniformsCache.cameraPosition.update(
      scene.camera.getPosition(),
      [&scene]()
      {
        scene.modelProgram.doOperations(
            [&cameraPosition = scene.camera.getPosition()](Program& program)
            { program.setVec3f("cameraPosition", cameraPosition); });
      });
  modelProgramUniformsCache.lightPosition.update(
      scene.lightPosition,
      [&scene]()
      {
        scene.modelProgram.doOperations(
            [&lightPosition = scene.lightPosition](Program& program)
            { program.setVec3f("lightPosition", lightPosition); });
        scene.lightProgram.doOperations(
            [&lightPosition = scene.lightPosition](Program& program)
            {
              const auto model =
                  glm::translate(glm::mat4x4(1.0), lightPosition);
              program.setMat4f("model", model);
            });
      });
  if (scene.model)
  {
    modelProgramUniformsCache.modelTransform.update(
        scene.model->transform_,
        [&program = scene.modelProgram, &model = scene.model.value()]()
        {
          program.doOperations(
              [&modelTransform = model.transform_](Program& program)
              { program.setMat4f("model", modelTransform); });
        });
  }
}

void Renderer::render(Model& model, Program& program)
{
  for (auto& mesh : model.meshes_)
  {
    render(mesh, program);
  }
}

void Renderer::renderSkybox(
    Mesh& skybox,
    Program& skyboxProgram,
    Texture& skyboxTexture)
{
  glDepthFunc(GL_LEQUAL);
  skyboxTexture.use(0);
  render(skybox, skyboxProgram);
  glDepthFunc(GL_LESS);
}

namespace
{

void drawPrimitives(const Mesh& mesh)
{
  glBindVertexArray(mesh.vao_);

  if (!mesh.indices_.empty())
  {
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(mesh.indices_.size()),
        GL_UNSIGNED_INT,
        nullptr);
  }
  else
  {
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.vertices_.size()));
  }

  glBindVertexArray(0);
}

}  // namespace

void Renderer::render(Mesh& mesh, Program& program)
{
  if (mesh.material_ == nullptr)
  {
    program.doOperations([&mesh](const Program& /*program*/)
                         { drawPrimitives(mesh); });
    return;
  }

  cache_.modelProgramUniformsCache.materialID.update(
      mesh.material_->getId(),
      [&mesh, &program]() { mesh.material_->use(program); });
  program.doOperations([&mesh](const Program& /*program*/)
                       { drawPrimitives(mesh); });
}

}  // namespace Simple3D