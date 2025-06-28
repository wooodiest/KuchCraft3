### Vertex
#pragma value(SHADER_VERSION_LONG)

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

out vec3 v_Color;

void main()
{
	v_Color = a_Color;
    gl_Position = vec4(a_Position, 1.0);
}

### Fragment
#pragma value(SHADER_VERSION_LONG)

layout(location = 0) out vec4 o_Color;

uniform vec4 u_Color;

in vec3 v_Color;

void main()
{
    o_Color = mix(u_Color, vec4(v_Color, 1.0), 0.5);
}