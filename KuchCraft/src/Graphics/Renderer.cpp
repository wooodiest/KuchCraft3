#include "kcpch.h"
#include "Graphics/Renderer.h"

#include "Core/Application.h"

#include <glad/glad.h>

namespace KuchCraft {

	Renderer::Renderer(Config config)
		: m_Config(config)
	{
		CheckExtensions();
		SetupLogging();
		SetGlobalSubstitutions();

		InitSimpleTriangleData();	
	}

	Renderer::~Renderer()
	{
	}

	Ref<Renderer> Renderer::Create(Config config)
	{
		return Ref<Renderer>(new Renderer(config));
	}

	void Renderer::NewFrame()
	{
		auto [width, height] = Application::Get().GetWindow()->GetSize();
		glViewport(0, 0, width, height);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_CurrentLayerIndex = 0;
	}

	void Renderer::EndFrame()
	{
		RenderSimpleTriangle();
	}

	void Renderer::OnWindowResize(int width, int height)
	{
		if (width <= 0 || height <= 0)
			return;
	}

	void Renderer::CheckExtensions()
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
	}

	void Renderer::SetupLogging()
	{
		if (!m_Config.Renderer.OpenGlLogging)
			return;
		
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

	void Renderer::SetGlobalSubstitutions()
	{
		m_ShaderLibrary.SetGlobalSubstitution("KC_VERSION", KC_VERSION);
		m_ShaderLibrary.SetGlobalSubstitution("KC_VERSION_LONG", KC_VERSION_LONG);
		m_ShaderLibrary.SetGlobalSubstitution("OPENGL_VERSION_MAJOR", std::to_string(m_Config.Renderer.OpenGlMajorVersion));
		m_ShaderLibrary.SetGlobalSubstitution("OPENGL_VERSION_MINOR", std::to_string(m_Config.Renderer.OpenGlMinorVersion));
		m_ShaderLibrary.SetGlobalSubstitution("SHADER_VERSION", m_Config.Renderer.GetOpenGlVersion());
		m_ShaderLibrary.SetGlobalSubstitution("SHADER_VERSION_LONG", "#version " + m_Config.Renderer.GetOpenGlVersion());

		const auto& substitutions = m_ShaderLibrary.GetGlobalSubstitutions();
		KC_CORE_INFO("Global shader substitutions:");
		for (const auto& [name, value] : substitutions)
		{
			KC_CORE_INFO("  {} = {}", name, value);
		}

		/// Maybe reload shaders??
	}

	void Renderer::InitSimpleTriangleData()
	{
		float triangleVertices[] = {
			// x     y     z       r     g     b
			 0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // top 
			-0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // bottom left
			 0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // bottom right
		};

		m_SimpleTriangleData.Shader = m_ShaderLibrary.Load(std::filesystem::path("assets/shaders/SimpleTriangle.glsl"), "SimpleTriangle");

		m_SimpleTriangleData.VertexArray  = VertexArray::Create();
		m_SimpleTriangleData.VertexBuffer = VertexBuffer::Create(VertexBufferDataUsage::Static, sizeof(triangleVertices), triangleVertices);
		m_SimpleTriangleData.VertexBuffer->SetLayout(m_SimpleTriangleData.Shader->GetVertexInputLayout());
		m_SimpleTriangleData.VertexArray->AddVertexBuffer(m_SimpleTriangleData.VertexBuffer);

		m_SimpleTriangleData.Shader->Bind();
		m_SimpleTriangleData.Shader->SetFloat4("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	void Renderer::RenderSimpleTriangle()
	{
		m_SimpleTriangleData.Shader->Bind();
		m_SimpleTriangleData.VertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}
