#version 330 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec3 iColor;
layout (location = 3) in vec2 iTexCoord0;
layout (location = 4) in vec2 iTexCoord1;
layout (location = 5) in vec2 iTexCoord2;
layout (location = 6) in vec2 iTexCoord3;
layout (location = 7) in vec2 iTexCoord4;

out vec3 FragPosition;
out vec3 Normal;
out vec3 Color;
out vec2 TexCoord0;
out vec2 TexCoord1;
out vec2 TexCoord2;
out vec2 TexCoord3;
out vec2 TexCoord4;

uniform mat4 model;
uniform mat4 pv;

void main()
{
  FragPosition = vec3(model * vec4(iPos, 1.0));
  Normal = mat3(transpose(inverse(model))) * iNormal;
  Color = iColor;
  TexCoord0 = iTexCoord0;    
  TexCoord1 = iTexCoord1;    
  TexCoord2 = iTexCoord2;    
  TexCoord3 = iTexCoord3;    
  TexCoord4 = iTexCoord4;    
  gl_Position = pv * model * vec4(iPos, 1.0);
}