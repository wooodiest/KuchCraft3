### Vertex
#value(SHADER_VERSION_LONG)

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;
layout (location = 3) in vec2 a_TextureCoord;
layout (location = 4) in int  a_TextureSlot;

#include "assets/shaders/CommonBindings.glsl"

out vec4 v_Color;
out vec3 v_Normal;
out vec2 v_TextureCoord;
out flat int v_TextureSlot;

void main()
{
	v_Color        = a_Color;
	v_Normal       = a_Normal;
	v_TextureCoord = a_TextureCoord;
	v_TextureSlot  = a_TextureSlot;

    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

### Fragment
#value(SHADER_VERSION_LONG)

layout (location = 0) out vec4 o_Color;
layout (location = 1) out vec4 o_Normal;

uniform sampler2D u_Textures[#value(MAX_COMBINED_TEXTURE_SLOTS)];

in vec4 v_Color;
in vec3 v_Normal;
in vec2 v_TextureCoord;
in flat int v_TextureSlot;

void main()
{
   vec4 color = v_Color * texture(u_Textures[v_TextureSlot], v_TextureCoord);
   if (color.a == 0.0)
		discard;

	o_Color  = color;
	o_Normal = vec4(v_Normal, 1.0);
}