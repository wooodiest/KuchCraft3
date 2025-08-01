### Vertex
#value(SHADER_VERSION_LONG)

layout (location = 0) in uint a_BlockDataLowerBits;
layout (location = 1) in uint a_BlockDataUpperBits;

#include "assets/shaders/CommonBindings.glsl"

uniform vec3 u_GlobalPosition;

out flat uint v_Layer;
out vec2 v_TexCoord;
out vec3 v_Normal;

const float uvWidth  = 1.0 / #value(BLOCK_FACE_COUNT);
const float uvHeight = 1.0;

const int blockFaceCount       = #value(BLOCK_FACE_COUNT);
const int blockVerticesPerFace = #value(BLOCK_VERTICES_PER_FACE);

const vec2 blockFaceUV[blockFaceCount][blockVerticesPerFace] = vec2[blockFaceCount][blockVerticesPerFace](
    vec2[](vec2(0.0,           0.0), vec2(uvWidth,       0.0), vec2(uvWidth,       uvHeight), vec2(0.0,           uvHeight)), /// Front
    vec2[](vec2(uvWidth,       0.0), vec2(2.0 * uvWidth, 0.0), vec2(2.0 * uvWidth, uvHeight), vec2(uvWidth,       uvHeight)), /// Left
    vec2[](vec2(2.0 * uvWidth, 0.0), vec2(3.0 * uvWidth, 0.0), vec2(3.0 * uvWidth, uvHeight), vec2(2.0 * uvWidth, uvHeight)), /// Back
    vec2[](vec2(3.0 * uvWidth, 0.0), vec2(4.0 * uvWidth, 0.0), vec2(4.0 * uvWidth, uvHeight), vec2(3.0 * uvWidth, uvHeight)), /// Right
    vec2[](vec2(4.0 * uvWidth, 0.0), vec2(5.0 * uvWidth, 0.0), vec2(5.0 * uvWidth, uvHeight), vec2(4.0 * uvWidth, uvHeight)), /// Top
    vec2[](vec2(5.0 * uvWidth, 0.0), vec2(6.0 * uvWidth, 0.0), vec2(6.0 * uvWidth, uvHeight), vec2(5.0 * uvWidth, uvHeight))  /// Bottom
);

const vec3 blockFaceNormals[blockFaceCount] = vec3[]( 
    vec3( 0.0,  0.0,  1.0), /// Front
    vec3(-1.0,  0.0,  0.0), /// Left
    vec3( 0.0,  0.0, -1.0), /// Back
    vec3( 1.0,  0.0,  0.0), /// Right
    vec3( 0.0,  1.0,  0.0), /// Top
    vec3( 0.0, -1.0,  0.0)  /// Bottom
);

const vec3 blockFacePositions[blockFaceCount][blockVerticesPerFace] = vec3[blockFaceCount][blockVerticesPerFace](
    vec3[](vec3(-0.5, -0.5,  0.5), vec3( 0.5, -0.5,  0.5), vec3( 0.5,  0.5,  0.5), vec3(-0.5,  0.5,  0.5)), /// Front
    vec3[](vec3(-0.5, -0.5, -0.5), vec3(-0.5, -0.5,  0.5), vec3(-0.5,  0.5,  0.5), vec3(-0.5,  0.5, -0.5)), /// Left
    vec3[](vec3( 0.5, -0.5, -0.5), vec3(-0.5, -0.5, -0.5), vec3(-0.5,  0.5, -0.5), vec3( 0.5,  0.5, -0.5)), /// Back
    vec3[](vec3( 0.5, -0.5,  0.5), vec3( 0.5, -0.5, -0.5), vec3( 0.5,  0.5, -0.5), vec3( 0.5,  0.5,  0.5)), /// Right
    vec3[](vec3(-0.5,  0.5,  0.5), vec3( 0.5,  0.5,  0.5), vec3( 0.5,  0.5, -0.5), vec3(-0.5,  0.5, -0.5)), /// Top
    vec3[](vec3(-0.5, -0.5, -0.5), vec3( 0.5, -0.5, -0.5), vec3( 0.5, -0.5,  0.5), vec3(-0.5, -0.5,  0.5))  /// Bottom
);

/// TODO: Do it in compiler time
uint UnpackBits(uint lower, uint upper, uint shift, uint bits)
{
    if (shift + bits <= 32)
    {
        return (lower >> shift) & ((1u << bits) - 1u);
    }
    else if (shift >= 32u)
    {
        uint upperShift = shift - 32u;
        return (upper >> upperShift) & ((1u << bits) - 1u);
    }
    else
    {
        uint lowPartBits  = 32u  - shift;
        uint highPartBits = bits - lowPartBits;

        uint lowPart  = (lower >> shift) & ((1u << lowPartBits) - 1u);
        uint highPart = upper & ((1u << highPartBits) - 1u);

        return (highPart << lowPartBits) | lowPart;
    }
}

void main()
{
    /// Extracting block data
    uint positionX = UnpackBits(a_BlockDataLowerBits, a_BlockDataUpperBits, #value(BLOCK_MESH_SHIFT_POSITION_X), #value(BLOCK_MESH_BITS_FOR_POSITION_X));
    uint positionY = UnpackBits(a_BlockDataLowerBits, a_BlockDataUpperBits, #value(BLOCK_MESH_SHIFT_POSITION_Y), #value(BLOCK_MESH_BITS_FOR_POSITION_Y));
    uint positionZ = UnpackBits(a_BlockDataLowerBits, a_BlockDataUpperBits, #value(BLOCK_MESH_SHIFT_POSITION_Z), #value(BLOCK_MESH_BITS_FOR_POSITION_Z));
    uint rotation  = UnpackBits(a_BlockDataLowerBits, a_BlockDataUpperBits, #value(BLOCK_MESH_SHIFT_ROTATION), #value(BLOCK_MESH_BITS_FOR_ROTATION));

    uint face        = UnpackBits(a_BlockDataLowerBits, a_BlockDataUpperBits, #value(BLOCK_MESH_SHIFT_FACE), #value(BLOCK_MESH_BITS_FOR_FACE));
    uint vertexIndex = UnpackBits(a_BlockDataLowerBits, a_BlockDataUpperBits, #value(BLOCK_MESH_SHIFT_VERTEX_INDEX), #value(BLOCK_MESH_BITS_FOR_VERTEX_INDEX));
    uint layer       = UnpackBits(a_BlockDataLowerBits, a_BlockDataUpperBits, #value(BLOCK_MESH_SHIFT_LAYER), #value(BLOCK_MESH_BITS_FOR_LAYER));

    /// Block data
    vec3 position = u_GlobalPosition + vec3(positionX, positionY, positionZ);

    if (face == #value(BLOCK_MESH_FACE_TOP)) 
        v_TexCoord = blockFaceUV[face][(vertexIndex - rotation + blockFaceCount) % blockFaceCount];
    else if (face == #value(BLOCK_MESH_FACE_BOTTOM))
        v_TexCoord = blockFaceUV[face][(vertexIndex + rotation) % blockFaceCount];
    else 
        v_TexCoord = blockFaceUV[(face + rotation) % blockFaceCount][vertexIndex];

    v_Normal = blockFaceNormals[face];
    v_Layer  = layer;

    gl_Position = u_ViewProjection * vec4(position + blockFacePositions[face][vertexIndex], 1.0);
}

### Fragment
#value(SHADER_VERSION_LONG)

layout (location = 0) out vec4 o_Color;
layout (location = 1) out vec4 o_Normal;

uniform sampler2DArray u_Textures;

in flat uint v_Layer;
in vec2 v_TexCoord;
in vec3 v_Normal;

void main()
{
    vec4 color = texture(u_Textures, vec3(v_TexCoord, v_Layer));
    if (color.a < 0.1)
        discard;

    o_Color = color;
    o_Normal = vec4(v_Normal, 1.0);
}