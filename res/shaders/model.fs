#version 330 core
out vec4 FragColor;

in vec3 FragPosition;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord0;
in vec2 TexCoord1;
in vec2 TexCoord2;
in vec2 TexCoord3;
in vec2 TexCoord4;

uniform sampler2D diffuseTexture0;
uniform sampler2D specularTexture0;
uniform sampler2D emissiveTexture0;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 emissiveColor;
uniform int diffuseTexturesCount;
uniform int emissiveTexturesCount;
uniform int diffuseUVChannel0;
uniform int emissiveUVChannel0;
uniform int specularUVChannel0;

void main()
{    
  vec4 baseColor = vec4(emissiveColor, 1.0);
  if (diffuseTexturesCount > 0)
  {
    vec4 diffuseSample;
    if(diffuseUVChannel0 == 0) diffuseSample = texture(diffuseTexture0, TexCoord0);
    else if(diffuseUVChannel0 == 1) diffuseSample = texture(diffuseTexture0, TexCoord1);
    else if(diffuseUVChannel0 == 2) diffuseSample = texture(diffuseTexture0, TexCoord2);
    else if(diffuseUVChannel0 == 3) diffuseSample = texture(diffuseTexture0, TexCoord3);
    else if(diffuseUVChannel0 == 4) diffuseSample = texture(diffuseTexture0, TexCoord4);
    baseColor = baseColor + diffuseSample;
  }
  if (emissiveTexturesCount > 0)
  {
    vec4 emissiveSample;
    if(emissiveUVChannel0 == 0) emissiveSample = texture(emissiveTexture0, TexCoord0);
    else if(emissiveUVChannel0 == 1) emissiveSample = texture(emissiveTexture0, TexCoord1);
    else if(emissiveUVChannel0 == 2) emissiveSample = texture(emissiveTexture0, TexCoord2);
    else if(emissiveUVChannel0 == 3) emissiveSample = texture(emissiveTexture0, TexCoord3);
    else if(emissiveUVChannel0 == 4) emissiveSample = texture(emissiveTexture0, TexCoord4);
    baseColor = baseColor + emissiveSample;
  }
  FragColor = baseColor;
}