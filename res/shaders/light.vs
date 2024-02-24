#version 330 core
layout (location = 0) in vec3 iPosition;

uniform mat4 model;
uniform mat4 pv;

void main()
{
	gl_Position = pv * model * vec4(iPosition, 1.0);
}