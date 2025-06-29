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

	union TextureClearValue
	{
		glm::vec4  FloatValues;
		glm::ivec4 IntValues;
		glm::uvec4 UIntValues;
	};

}