### Vertex
#pragma value(SHADER_VERSION_LONG)

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

#include "assets/shaders/CommonBindings.glsl"

out vec3 v_Color;
out vec2 v_TexCoord;

void main()
{
	v_Color    = a_Color;
    v_TexCoord = a_TexCoord;

    //gl_Position = vec4(a_Position, 1.0);
	gl_Position = u_OrthoProjection * vec4(a_Position, 1.0);
}

### Fragment
#pragma value(SHADER_VERSION_LONG)

layout(location = 0) out vec4 o_Color;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

in vec3 v_Color;
in vec2 v_TexCoord;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord);
	o_Color = mix(u_Color, texColor * vec4(v_Color, 1.0), 0.5);
}