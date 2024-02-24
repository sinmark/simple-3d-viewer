#version 330 core
 
#define SPANMAX 8.0
#define REDUCEMIN 0.0078125
#define REDUCEMUL 0.125
 
in vec2 TexCoord;
uniform sampler2D screenTexture;
uniform vec3 inverseScreenSize;

out vec4 FragColor;
 
void main()
{
  vec2 texCoordOffset = inverseScreenSize.xy;
  
  vec3 luma = vec3(0.299, 0.587, 0.114);  
  float lumaTL = dot(luma, texture2D(screenTexture, TexCoord.xy + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
  float lumaTR = dot(luma, texture2D(screenTexture, TexCoord.xy + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
  float lumaBL = dot(luma, texture2D(screenTexture, TexCoord.xy + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
  float lumaBR = dot(luma, texture2D(screenTexture, TexCoord.xy + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
  float lumaM  = dot(luma, texture2D(screenTexture, TexCoord.xy).xyz);

  vec2 dir;
  dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
  dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
  
  float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (REDUCEMUL * 0.25), REDUCEMIN);
  float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
  
  dir = min(vec2(SPANMAX, SPANMAX),
    max(vec2(-SPANMAX, -SPANMAX), dir * inverseDirAdjustment)) * texCoordOffset;

  vec3 result1 = (1.0/2.0) * (
    texture2D(screenTexture, TexCoord.xy + (dir * vec2(1.0/3.0 - 0.5))).xyz +
    texture2D(screenTexture, TexCoord.xy + (dir * vec2(2.0/3.0 - 0.5))).xyz);

  vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
    texture2D(screenTexture, TexCoord.xy + (dir * vec2(0.0/3.0 - 0.5))).xyz +
    texture2D(screenTexture, TexCoord.xy + (dir * vec2(3.0/3.0 - 0.5))).xyz);

  float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
  float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
  float lumaResult2 = dot(luma, result2);
  
  if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
    FragColor = vec4(result1, 1.0);
  else
    FragColor = vec4(result2, 1.0);
}
