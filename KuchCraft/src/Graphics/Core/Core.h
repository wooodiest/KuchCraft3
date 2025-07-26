#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

namespace KuchCraft {

	using RendererID  = uint32_t;

	using GLenum  = unsigned int;
	using GLuint  = unsigned int;
	using GLint   = int;
	using GLsizei = int;

	inline constexpr float ortho_near = -1.0f;
	inline constexpr float ortho_far  =  1.0f;

	constexpr inline uint32_t quad_vertex_count = 4;
	constexpr inline uint32_t quad_index_count = 6;

	constexpr inline glm::vec4 quad_vertex_positions[quad_vertex_count] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f }, ///< Bottom-left corner.
		{  0.5f, -0.5f, 0.0f, 1.0f }, ///< Bottom-right corner.
		{  0.5f,  0.5f, 0.0f, 1.0f }, ///< Top-right corner.
		{ -0.5f,  0.5f, 0.0f, 1.0f }  ///< Top-left corner.
	};

	constexpr inline glm::vec2 quad_vertex_texture_coords[quad_vertex_count] = {
		{ 0.0f, 0.0f }, ///< Bottom-left corner.
		{ 1.0f, 0.0f }, ///< Bottom-right corner
		{ 1.0f, 1.0f }, ///< Top-right corner.
		{ 0.0f, 1.0f }  ///< Top-left corner.
	};

	struct VertexQuad2D
	{
		glm::vec3 Position     = { 0.0f, 0.0f, 0.0f };
		glm::vec4 Color        = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec2 TextureCoord = { 0.0f, 0.0f };
		int       TextureSlot  = 0;
	};

	enum class FaceWinding
	{
		CounterClockwise = 0,
		Clockwise,
		None
	};

	enum class DepthFunc
	{
		Never = 0,
		Less ,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,
		None
	};

	enum class BlendFunc
	{
		Zero = 0,
		One,
		SrcAlpha,
		OneMinusSrcAlpha,
		None
	};

	enum class CullMode 
	{
		Back = 0,
		Front,
		FrontAndBack,
		None
	};

	enum class PolygonMode
	{
		Fill = 0,
		Line,
		Point,
		None
	};

	enum class PrimitiveTopology
	{
		Points = 0,
		Lines,
		LineStrip,
		Triangles,
		TriangleStrip,
		TriangleFan,
		LinesAdjacency,
		TrianglesAdjacency,
		Patches
	};

	constexpr uint32_t GetVerticesPerPrimitive(PrimitiveTopology topology)
	{
		switch (topology)
		{
			case PrimitiveTopology::Points:         return 1;
			case PrimitiveTopology::Lines:          return 2;
			case PrimitiveTopology::Triangles:      return 3;
			case PrimitiveTopology::LinesAdjacency: return 4;
			case PrimitiveTopology::TrianglesAdjacency: return 6;
			default: return 1;
		}
	}

	constexpr uint32_t GetPrimitiveCount(PrimitiveTopology topology, uint32_t vertexCount)
	{
		switch (topology)
		{
			case PrimitiveTopology::Points:         return vertexCount;
			case PrimitiveTopology::Lines:          return vertexCount / 2;
			case PrimitiveTopology::LineStrip:      return vertexCount >= 2 ? vertexCount - 1 : 0;
			case PrimitiveTopology::Triangles:      return vertexCount / 3;
			case PrimitiveTopology::TriangleStrip:  return vertexCount >= 3 ? vertexCount - 2 : 0;
			case PrimitiveTopology::TriangleFan:    return vertexCount >= 3 ? vertexCount - 2 : 0;
			default: return 0;
		}
	}

}