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

		m_WhiteTexture = Texture2D::Create(TextureSpecification{ .Width = 1, .Height = 1 });
		uint32_t whiteColor = 0xffffffff;
		m_WhiteTexture->SetData(&whiteColor, sizeof(whiteColor));

		m_BlackTexture = Texture2D::Create(TextureSpecification{ .Width = 1, .Height = 1 });
		uint32_t blackColor = 0x000000;
		m_BlackTexture->SetData(&blackColor, sizeof(blackColor));

		FrameBufferSpecification fbSpec;
		fbSpec.Name   = "Offscreen Frame Buffer";
		fbSpec.Width  = Application::Get().GetWindow()->GetWidth();
		fbSpec.Height = Application::Get().GetWindow()->GetHeight();
		fbSpec.Attachments.Attachments = {
			{ "ColorAttachment", TextureFormat::RGBA     },
			{ "DepthAttachment", TextureFormat::DEPTH32F }
		};

		m_OffscreenRenderTarget = FrameBuffer::Create(fbSpec);

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
		ClearDefaultFrameBuffer();
		m_CurrentLayerIndex = 0;
	}

	void Renderer::EndFrame()
	{
		m_OffscreenRenderTarget->Bind();
		m_OffscreenRenderTarget->ClearAttachments();

		RenderSimpleTriangle();

		m_OffscreenRenderTarget->Unbind();
		SetRenderTargetToDefault();
		m_OffscreenRenderTarget->BlitToDefault(FrameBufferBlitMask::Color, TextureFilter::Linear);
	}

	void Renderer::OnWindowResize(int width, int height)
	{
		if (width <= 0 || height <= 0)
			return;

		m_OffscreenRenderTarget->Resize(width, height);
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
		/// Maybe reload shaders??
	}

	void Renderer::ClearDefaultFrameBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		const auto& color = m_Config.Renderer.ClearColor;
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Renderer::SetRenderTargetToDefault()
	{
		auto [width, height] = Application::Get().GetWindow()->GetSize();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
	}

	void Renderer::InitSimpleTriangleData()
	{
		float triangleVertices[] = {
		//	  x     y     z     r  g  b       u     v
			 0.0f, 0.5f, 0.0f,  1, 0, 0,     0.5f, 1.0f,
			-0.5f,-0.5f, 0.0f,  0, 1, 0,     0.0f, 0.0f,
			 0.5f,-0.5f, 0.0f,  0, 0, 1,     1.0f, 0.0f,
		};

		m_SimpleTriangleData.Shader = m_ShaderLibrary.Load(std::filesystem::path("assets/shaders/SimpleTriangle.glsl"), "SimpleTriangle");

		m_SimpleTriangleData.VertexArray  = VertexArray::Create();
		m_SimpleTriangleData.VertexArray->SetDebugName("SimpleTriangleVAO");

		m_SimpleTriangleData.VertexBuffer = VertexBuffer::Create(VertexBufferDataUsage::Static, sizeof(triangleVertices), triangleVertices);
		m_SimpleTriangleData.VertexBuffer->SetDebugName("SimpleTriangleVBO");
		m_SimpleTriangleData.VertexBuffer->SetLayout(m_SimpleTriangleData.Shader->GetVertexInputLayout());
		m_SimpleTriangleData.VertexArray->AddVertexBuffer(m_SimpleTriangleData.VertexBuffer);

		m_SimpleTriangleData.Texture = Texture2D::Create(std::filesystem::path("assets/textures/grid.png"));

		m_SimpleTriangleData.Shader->Bind();
		m_SimpleTriangleData.Shader->SetFloat4("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_SimpleTriangleData.Shader->SetInt("u_Texture", 0);
	}

	void Renderer::RenderSimpleTriangle()
	{
		m_SimpleTriangleData.Shader->Bind();
		m_SimpleTriangleData.Texture->Bind();
		m_SimpleTriangleData.VertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}
