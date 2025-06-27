#include "kcpch.h"
#include "Graphics/Renderer.h"

#include <glad/glad.h>

namespace KuchCraft {

	Renderer::Renderer(Config config)
	{
		if (!GLAD_GL_KHR_debug)
		{
			KC_CORE_WARN("GL_KHR_debug is not supported!");
		}
		if (!GLAD_GL_ARB_direct_state_access)
		{
			KC_CORE_WARN("GL_ARB_direct_state_access is not supported!");
		}
		if (!GLAD_GL_ARB_buffer_storage)
		{
			KC_CORE_WARN("GL_ARB_buffer_storage is not supported!");
		}
		if (!GLAD_GL_ARB_shader_draw_parameters)
		{
			KC_CORE_WARN("GL_ARB_shader_draw_parameters is not supported!");
		}
		if (!GLAD_GL_ARB_multi_draw_indirect)
		{
			KC_CORE_WARN("GL_ARB_multi_draw_indirect is not supported!");
		}
		if (!GLAD_GL_ARB_bindless_texture)
		{
			KC_CORE_WARN("GL_ARB_bindless_texture is not supported!");
		}

		if (config.Renderer.OpenGlLogging)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
				switch (severity)
				{
					case GL_DEBUG_SEVERITY_HIGH:   { KC_CORE_ERROR("OpenGL: {}", message); break; }
					case GL_DEBUG_SEVERITY_MEDIUM: { KC_CORE_WARN ("OpenGL: {}", message); break; }
					case GL_DEBUG_SEVERITY_LOW:    { KC_CORE_INFO ("OpenGL: {}", message); break; }
					default:                       { KC_CORE_TRACE("OpenGL: {}", message); break; }
				}
			}, nullptr);
		}
	}

	Renderer::~Renderer()
	{
	}

	Ref<Renderer> Renderer::Create(Config config)
	{
		return Ref<Renderer>(new Renderer(config));
	}
}
