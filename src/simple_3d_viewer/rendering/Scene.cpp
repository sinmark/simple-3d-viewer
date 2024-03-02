#include <simple_3d_viewer/linear_algebra/meshGeneration.hpp>
#include <simple_3d_viewer/rendering/Scene.hpp>
#include <simple_3d_viewer/utils/constants.hpp>
#include <simple_3d_viewer/utils/factories.hpp>

namespace Simple3D
{

namespace
{

constexpr auto kLightSphereRadius = 0.1f;

}  // namespace

Scene::Scene(Size framebufferSize)
    : camera(framebufferSize),
      light(sphereMesh(kLightSphereRadius)),
      lightProgram(createProgram(kLightShaderCommonName)),
      skybox(skyboxMesh()),
      skyboxProgram(createProgram(kSkyboxShaderCommonName)),
      skyboxTexture(kSkyboxImagesPaths()),
      modelProgram(createProgram(kModelShaderCommonName))
{
}

}  // namespace Simple3D