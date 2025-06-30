#pragma once

#include <stdint.h>

namespace KuchCraft {

	using RendererID = uint32_t;

	using GLenum = unsigned int;
	using GLuint = unsigned int;
	using GLint  = int;

	inline constexpr float ortho_near = -1.0f;
	inline constexpr float ortho_far  =  1.0f;
}