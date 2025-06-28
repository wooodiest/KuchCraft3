### Vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

layout (std140, binding = 0) uniform UniformCameraData
{
	mat4 u_ViewProjection;
	mat4 u_OrthoProjection;
};

out vec3 v_Color;

uniform mat4 u_ProjectionMatrix;

void main()
{
v_Color = a_Color;
    gl_Position = vec4(a_Position, 1.0);
}

### Fragment
#version 460 core

layout(location = 0) out vec4 o_Color;

uniform vec4 u_Color;

in vec3 v_Color;

void main()
{
    o_Color = vec4(v_Color, 1.0);
}