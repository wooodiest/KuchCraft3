#pragma once

#include "Graphics/Core/TextureSpecification.h"

namespace KuchCraft {
	namespace Utils {

		GLenum GetGLInternalFormat(TextureFormat format);

		GLenum GetGLFormat(TextureFormat format);

		GLenum GetGLType(TextureFormat format);

		GLenum GetGlTextureWrap(TextureWrap wrap);

		GLenum GetGlTextureFilter(TextureFilter filter);

		inline bool IsDepthFormat(TextureFormat format)
		{
			return format == TextureFormat::DEPTH32FSTENCIL8UINT ||
				format == TextureFormat::DEPTH32F ||
				format == TextureFormat::DEPTH24STENCIL8;
		}

		inline bool IsStencilFormat(TextureFormat format)
		{
			return format == TextureFormat::STENCIL8;
		}

		inline bool IsDepthStencilFormat(TextureFormat format)
		{
			return format == TextureFormat::DEPTH24STENCIL8 ||
				format == TextureFormat::DEPTH32FSTENCIL8UINT;
		}

		inline bool IsColorFormat(TextureFormat format)
		{
			return format != TextureFormat::None && !IsDepthFormat(format) && !IsStencilFormat(format);
		}

		inline bool IsSignedIntegerFormat(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RED8I:
				case TextureFormat::RED16I:
				case TextureFormat::RED32I:
					return true;	
			}

			return false;
		}

		inline bool IsUnsignedIntegerFormat(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RED8UI:
				case TextureFormat::RED16UI:
				case TextureFormat::RED32UI:
					return true;
			}

			return false;
		}

		inline bool IsFloatFormat(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RED8UN:
				case TextureFormat::RGBA:
				case TextureFormat::RGB:
				case TextureFormat::SRGB:
				case TextureFormat::SRGBA:
				case TextureFormat::RED32F:
				case TextureFormat::RG16F:
				case TextureFormat::RG32F:
				case TextureFormat::RGBA16F:
				case TextureFormat::RGBA32F:
				case TextureFormat::DEPTH32F:
				case TextureFormat::DEPTH32FSTENCIL8UINT:
					return true;
			}

			return false;
		}

		inline uint32_t GetTextureFormatSize(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RED8I:    return 1;
				case TextureFormat::RED16I:   return 2;
				case TextureFormat::RED32I:   return 4;
				case TextureFormat::RED8UN:   return 1;
				case TextureFormat::RED8UI:   return 1;
				case TextureFormat::RED16UI:  return 2;
				case TextureFormat::RED32UI:  return 4;
				case TextureFormat::RED32F:   return 4;
				case TextureFormat::RG8:      return 2;
				case TextureFormat::RG16F:    return 4;
				case TextureFormat::RG32F:    return 8;
				case TextureFormat::RGB:      return 3;
				case TextureFormat::RGBA:     return 4;
				case TextureFormat::RGBA16F:  return 8;
				case TextureFormat::RGBA32F:  return 16;
				case TextureFormat::SRGB:     return 3;
				case TextureFormat::SRGBA:    return 4;
				case TextureFormat::DEPTH32FSTENCIL8UINT:
					return 8; // Depth + Stencil
				case TextureFormat::DEPTH32F:
					return 4; // Depth only
				case TextureFormat::DEPTH24STENCIL8:
					return 4; // Depth + Stencil
				case TextureFormat::STENCIL8: return 1;
			}

			KC_CORE_ASSERT(false, "Unknown TextureFormat!");
			return 0;
		}

		inline uint32_t GetTextureFormatChannelCount(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RED8I:    return 1;
				case TextureFormat::RED16I:   return 1;
				case TextureFormat::RED32I:   return 1;
				case TextureFormat::RED8UN:   return 1;
				case TextureFormat::RED8UI:   return 1;
				case TextureFormat::RED16UI:  return 1;
				case TextureFormat::RED32UI:  return 1;
				case TextureFormat::RED32F:   return 1;
				case TextureFormat::RG8:      return 2;
				case TextureFormat::RG16F:    return 2;
				case TextureFormat::RG32F:    return 2;
				case TextureFormat::RGB:      return 3;
				case TextureFormat::RGBA:     return 4;
				case TextureFormat::RGBA16F:  return 4;
				case TextureFormat::RGBA32F:  return 4;
				case TextureFormat::SRGB:     return 3;
				case TextureFormat::SRGBA:    return 4;
				case TextureFormat::DEPTH32FSTENCIL8UINT:
					return 2; // Depth + Stencil
				case TextureFormat::DEPTH32F:
					return 1; // Depth only
				case TextureFormat::DEPTH24STENCIL8:
					return 2; // Depth + Stenci
				case TextureFormat::STENCIL8: return 1;
			}
			KC_CORE_ASSERT(false, "Unknown TextureFormat!");
			return 0;
		}

		inline uint32_t GetMemorySize(TextureFormat format, uint32_t width, uint32_t height = 1)
		{
			return width * height * GetTextureFormatSize(format);
		}

		inline uint32_t GetMemorySize(TextureFormat format, const glm::ivec2& size)
		{
			return GetMemorySize(format, size.x, size.y);
		}

	}
}