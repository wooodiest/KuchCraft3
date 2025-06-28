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

		m_ShaderLibrary.SetGlobalSubstitution("KC_VERSION", KC_VERSION);
		m_ShaderLibrary.SetGlobalSubstitution("KC_VERSION_LONG", KC_VERSION_LONG);
		m_ShaderLibrary.SetGlobalSubstitution("OPENGL_VERSION_MAJOR", std::to_string(config.Renderer.OpenGlMajorVersion));
		m_ShaderLibrary.SetGlobalSubstitution("OPENGL_VERSION_MINOR", std::to_string(config.Renderer.OpenGlMinorVersion));
		m_ShaderLibrary.SetGlobalSubstitution("SHADER_VERSION",config.Renderer.GetOpenGlVersion());
		m_ShaderLibrary.SetGlobalSubstitution("SHADER_VERSION_LONG", "#version " + config.Renderer.GetOpenGlVersion());

		const auto& substitutions = m_ShaderLibrary.GetGlobalSubstitutions();
		KC_CORE_INFO("Global shader substitutions:");
		for (const auto& [name, value] : substitutions)
		{
			KC_CORE_INFO("  {} = {}", name, value);
		}

		/// tmp
		float triangleVertices[] = {
			// x     y     z       r     g     b
			 0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // top 
			-0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // bottom left
			 0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // bottom right
		};

		m_ExampleData.Shader = m_ShaderLibrary.Load(std::filesystem::path("assets/shaders/SimpleTriangle.glsl"), "SimpleTriangle");

		m_ExampleData.VertexArray  = VertexArray::Create();
		m_ExampleData.VertexBuffer = VertexBuffer::Create(VertexBufferDataUsage::Static, sizeof(triangleVertices), triangleVertices);
		m_ExampleData.VertexBuffer->SetLayout(m_ExampleData.Shader->GetVertexInputLayout());
		m_ExampleData.VertexArray->AddVertexBuffer(m_ExampleData.VertexBuffer);

		std::string shader_VertexBufferSource   = m_ExampleData.Shader->GetShaderSources().at(ShaderType::Vertex);
		std::string shader_FragmentBufferSource = m_ExampleData.Shader->GetShaderSources().at(ShaderType::Fragment);
		KC_INFO("VertexBuffer source:\n{}", shader_VertexBufferSource);
		KC_INFO("FragmentBuffer source:\n{}", shader_FragmentBufferSource);


		m_ExampleData.Shader->Bind();
		m_ExampleData.Shader->SetFloat4("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	Renderer::~Renderer()
	{
	}

	Ref<Renderer> Renderer::Create(Config config)
	{
		return Ref<Renderer>(new Renderer(config));
	}
	void Renderer::Render()
	{
		m_ExampleData.Shader->Bind();
		m_ExampleData.VertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}
