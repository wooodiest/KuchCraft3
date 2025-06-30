#pragma once

#include <glm/glm.hpp>
#include <stdint.h>

namespace KuchCraft {

	enum class TextureFormat : uint8_t
	{
		None = 0,

		RED8I,
		RED16I,
		RED32I,

		RED8UN,
		RED8UI,
		RED16UI,
		RED32UI,

		RED32F,
		RG8,
		RG16F,
		RG32F,
		RGB,
		RGBA,
		RGBA16F,
		RGBA32F,

		SRGB,
		SRGBA,

		STENCIL8,
		DEPTH32FSTENCIL8UINT,
		DEPTH32F,
		DEPTH24STENCIL8
	};

	enum class TextureWrap : uint8_t
	{
		None = 0,
		Clamp,
		Repeat
	};

	enum class TextureFilter : uint8_t
	{
		None = 0,
		Linear,
		Nearest
	};

	enum class TextureType : uint8_t
	{
		None = 0,
		Texture2D,
		TextureCube,
		Texture2DArray,
	};

	enum class CubeTextureFaces : uint8_t
	{
		Right  = 0, /// +X
		Left   = 1, /// -X
		Top    = 2, /// +Y
		Bottom = 3, /// -Y
		Front  = 4, /// +Z
		Back   = 5  /// -Z
	};

	union TextureClearValue
	{
		glm::vec4  FloatValues;
		glm::ivec4 IntValues;
		glm::uvec4 UIntValues;
	};

}