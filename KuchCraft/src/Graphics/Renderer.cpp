#include "kcpch.h"
#include "Graphics/Renderer.h"

#include "Core/Application.h"
#include "Core/LayerStack.h"

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
		m_WhiteTexture->SetDebugName("WhiteTexture");

		m_BlackTexture = Texture2D::Create(TextureSpecification{ .Width = 1, .Height = 1 });
		uint32_t blackColor = 0x000000;
		m_BlackTexture->SetData(&blackColor, sizeof(blackColor));
		m_BlackTexture->SetDebugName("BlackTexture");

		m_EnvironmentUniformBuffer = UniformBuffer::Create(sizeof(m_EnvironmentUniformBufferData));
		m_EnvironmentUniformBuffer->SetDebugName("EnvironmentUniformBuffer");
		m_ShaderLibrary.SetGlobalSubstitution("ENVIRONMENT_UNIFORM_BUFFER_BINDING", std::to_string(m_EnvironmentUniformBuffer->GetBinding()));

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
		InitQuads2D();
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
		SetLayerIndex(0);

		m_EnvironmentUniformBufferData.ViewProjection  = glm::mat4(1.0f);
		m_EnvironmentUniformBufferData.OrthoProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
		m_EnvironmentUniformBuffer->SetData(&m_EnvironmentUniformBufferData, sizeof(m_EnvironmentUniformBufferData));
	}

	void Renderer::EndFrame()
	{
		m_OffscreenRenderTarget->Bind();
		m_OffscreenRenderTarget->ClearAttachments();

		DrawQuad2D({ 900.0f, 500.0f }, { 100.0f, 100.0f }, m_SimpleTriangleData.Texture);

		RenderQuads2D();

		m_OffscreenRenderTarget->Unbind();
		SetRenderTargetToDefault();
		m_OffscreenRenderTarget->BlitToDefault(FrameBufferBlitMask::Color, TextureFilter::Linear);
	}

	void Renderer::SetLayerIndex(int layerIndex)
	{
		m_CurrentLayerIndex = layerIndex;
		m_CurrentOrthoDepth = ortho_near + (ortho_far - ortho_near) * (1.0f - (float)m_CurrentLayerIndex / (float)LayerStack::s_MaxLayers);
	}

	void Renderer::OnWindowResize(int width, int height)
	{
		if (width <= 0 || height <= 0)
			return;

		m_OffscreenRenderTarget->Resize(width, height);
	}

	void Renderer::DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad2D({ position.x, position.y, m_CurrentOrthoDepth }, size, color);
	}

	void Renderer::DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		int textureID = 0; /// White texture
		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			m_Quads2D.Vertices.emplace_back(
				glm::vec3(transform * quad_vertex_positions[i]),
				color,
				quad_vertex_texture_coords[i],
				textureID
			);
		}
	}

	void Renderer::DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, glm::vec2 uv0, glm::vec2 uv1)
	{
		DrawQuad2D({ position.x, position.y, m_CurrentOrthoDepth }, size, texture, tilingFactor, tintColor, uv0, uv1);
	}

	void Renderer::DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, glm::vec2 uv0, glm::vec2 uv1)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		int textureID = texture->GetRendererID();
		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			glm::vec2 baseUV = quad_vertex_texture_coords[i];
			glm::vec2 uv     = glm::mix(uv0, uv1, baseUV) * tilingFactor;

			m_Quads2D.Vertices.emplace_back(
				glm::vec3(transform * quad_vertex_positions[i]),
				tintColor,                                      
				uv,                                            
				textureID                                      
			);
		}
	}

	void Renderer::DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad2D({ position.x, position.y, m_CurrentOrthoDepth }, size, rotation, color);
	}

	void Renderer::DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		int textureID = 0; /// White texture
		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			m_Quads2D.Vertices.emplace_back(
				glm::vec3(transform * quad_vertex_positions[i]),
				color,
				quad_vertex_texture_coords[i],
				textureID
			);
		}
	}

	void Renderer::DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, glm::vec2 uv0, glm::vec2 uv1)
	{
		DrawRotatedQuad2D({ position.x, position.y, m_CurrentOrthoDepth }, size, rotation, texture, tilingFactor, tintColor, uv0, uv1);
	}

	void Renderer::DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, glm::vec2 uv0, glm::vec2 uv1)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		int textureID = texture->GetRendererID();
		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			glm::vec2 baseUV = quad_vertex_texture_coords[i];
			glm::vec2 uv = glm::mix(uv0, uv1, baseUV) * tilingFactor;

			m_Quads2D.Vertices.emplace_back(
				glm::vec3(transform * quad_vertex_positions[i]),
				tintColor,
				uv,
				textureID
			);
		}
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
		m_ShaderLibrary.SetGlobalSubstitution("MAX_TEXTURE_SLOTS",          std::to_string(m_Config.Renderer.MaxTextureSlots));
		m_ShaderLibrary.SetGlobalSubstitution("MAX_COMBINED_TEXTURE_SLOTS", std::to_string(m_Config.Renderer.MaxCombinedTextureSlots));
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

	void Renderer::InitQuads2D()
	{
		/// Graphics
		m_Quads2D.MaxQuadsInBatch = m_Config.Renderer.MaxQuadsInBatch;
		m_Quads2D.MaxIndices  = m_Quads2D.MaxQuadsInBatch * quad_index_count;
		m_Quads2D.MaxVertices = m_Quads2D.MaxQuadsInBatch * quad_vertex_count;

		m_Quads2D.Shader = m_ShaderLibrary.Load(std::filesystem::path("assets/shaders/Quads2D.glsl"));
		m_Quads2D.Shader->Bind();

		m_Quads2D.Shader->LogLayout();

		m_Quads2D.VertexArray = VertexArray::Create();
		m_Quads2D.VertexArray->Bind();
		m_Quads2D.VertexArray->SetDebugName("Quads2D_VAO");

		m_Quads2D.VertexBuffer = VertexBuffer::Create(VertexBufferDataUsage::Dynamic, m_Quads2D.MaxVertices * sizeof(VertexQuad2D));
		m_Quads2D.VertexBuffer->SetDebugName("Quads2D_VBO");
		m_Quads2D.VertexBuffer->SetLayout(m_Quads2D.Shader->GetVertexInputLayout());
		m_Quads2D.VertexArray ->AddVertexBuffer(m_Quads2D.VertexBuffer);

		std::vector<uint32_t> indices;
		indices.reserve(m_Quads2D.MaxIndices);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < m_Quads2D.MaxIndices; i += quad_index_count)
		{
			indices.push_back(offset + 0);
			indices.push_back(offset + 1);
			indices.push_back(offset + 2);
			indices.push_back(offset + 2);
			indices.push_back(offset + 3);
			indices.push_back(offset + 0);

			offset += quad_vertex_count;
		}

		m_Quads2D.IndexBuffer = IndexBuffer::Create(indices.data(), m_Quads2D.MaxIndices);
		m_Quads2D.IndexBuffer->SetDebugName("Quads2D_IBO");
		m_Quads2D.VertexArray->SetIndexBuffer(m_Quads2D.IndexBuffer);

		KC_TODO("After reloading shader set this again");
		std::vector<int> samplers;
		indices.reserve(m_Config.Renderer.MaxCombinedTextureSlots);
		for (int i = 0; i < m_Config.Renderer.MaxCombinedTextureSlots; i++)
			samplers.push_back(i);

		m_Quads2D.Shader->SetIntArray("u_Textures", samplers.data(), m_Config.Renderer.MaxCombinedTextureSlots);

		/// Internal
		m_Quads2D.Textures.resize(m_Config.Renderer.MaxCombinedTextureSlots, 0);
		m_Quads2D.Textures[0] = m_WhiteTexture->GetRendererID();

		m_Quads2D.Vertices.reserve(m_Quads2D.MaxVertices);
	}

	void Renderer::RenderQuads2D()
	{
		if (m_Quads2D.Vertices.empty())
			return;

		m_Quads2D.Shader     ->Bind();
		m_WhiteTexture       ->Bind(0);
		m_Quads2D.VertexArray->Bind();

		m_Quads2D.VertexOffset = 0;

		StartBatchQuads2D();
		for (size_t i = 0; i < m_Quads2D.Vertices.size(); i += quad_vertex_count)
		{
			if (m_Quads2D.CurrentIndexCount == m_Quads2D.MaxIndices)
				NextBatchQuads2D();

			/// TextureSlot temporarily holds the texture rendererID
			if (m_Quads2D.Vertices[i].TextureSlot == 0)
			{
				/// Just color, white texture is bound to slot 0
				m_Quads2D.CurrentIndexCount += quad_index_count;
			}
			else
			{
				/// Do we already have assigned slot to that texture?
				int textureSlot = 0;
				for (size_t j = 1; j < m_Quads2D.CurrentTextureSlot; j++)
				{
					if (m_Quads2D.Textures[j] == (RendererID)m_Quads2D.Vertices[i].TextureSlot) /// TextureSlot temporarily holds the texture rendererID
					{
						textureSlot = (int)j;
						break;
					}
				}

				/// If not, do it
				if (textureSlot == 0)
				{
					if (m_Quads2D.CurrentTextureSlot >= m_Config.Renderer.MaxCombinedTextureSlots)
						NextBatchQuads2D();

					textureSlot = (int)m_Quads2D.CurrentTextureSlot;
					m_Quads2D.Textures[m_Quads2D.CurrentTextureSlot] = (RendererID)m_Quads2D.Vertices[i].TextureSlot;
					m_Quads2D.CurrentTextureSlot++;
				}

				m_Quads2D.Vertices[i + 0].TextureSlot = textureSlot;
				m_Quads2D.Vertices[i + 1].TextureSlot = textureSlot;
				m_Quads2D.Vertices[i + 2].TextureSlot = textureSlot;
				m_Quads2D.Vertices[i + 3].TextureSlot = textureSlot;

				m_Quads2D.CurrentIndexCount += quad_index_count;
			}
		}

		FlushQuads2D();

		m_Quads2D.Vertices.clear();
	}

	void Renderer::StartBatchQuads2D()
	{
		m_Quads2D.CurrentIndexCount  = 0;
		m_Quads2D.CurrentTextureSlot = 1; /// 0 is reserved for a default white texture
	}

	void Renderer::NextBatchQuads2D()
	{
		FlushQuads2D();
		StartBatchQuads2D();
	}

	void Renderer::FlushQuads2D()
	{
		if (m_Quads2D.CurrentIndexCount == 0)
			return;

		uint32_t vertexCount = m_Quads2D.CurrentIndexCount / quad_index_count * quad_vertex_count;
		m_Quads2D.VertexBuffer->SetData(&m_Quads2D.Vertices[m_Quads2D.VertexOffset], vertexCount * sizeof(VertexQuad2D));

		m_Quads2D.VertexOffset += vertexCount;
		for (int slot = 1; slot < (int)m_Quads2D.CurrentTextureSlot; slot++)
			Texture::Bind(slot, m_Quads2D.Textures[slot]);
		
		glDrawElements(GL_TRIANGLES, m_Quads2D.CurrentIndexCount, GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::InitSimpleTriangleData()
	{
		m_SimpleTriangleData.Shader = m_ShaderLibrary.Load(std::filesystem::path("assets/shaders/SimpleTriangle.glsl"), "SimpleTriangle");
		m_SimpleTriangleData.Shader->Bind();

		m_SimpleTriangleData.VertexArray  = VertexArray::Create();
		m_SimpleTriangleData.VertexArray->Bind();
		m_SimpleTriangleData.VertexArray->SetDebugName("SimpleTriangleVAO");

		float triangleVertices[] = {
		//	       x       y     z     r  g  b       u     v
				600.0f, 540.0f, 0.0f,  1, 0, 0,     0.5f, 1.0f,
				300.0f, 180.0f, 0.0f,  0, 1, 0,     0.0f, 0.0f,
				900.0f, 180.0f, 0.0f,  0, 0, 1,     1.0f, 0.0f,
		};

		m_SimpleTriangleData.VertexBuffer = VertexBuffer::Create(VertexBufferDataUsage::Static, sizeof(triangleVertices), triangleVertices);
		m_SimpleTriangleData.VertexBuffer->SetDebugName("SimpleTriangleVBO");
		m_SimpleTriangleData.VertexBuffer->SetLayout(m_SimpleTriangleData.Shader->GetVertexInputLayout());
		m_SimpleTriangleData.VertexArray->AddVertexBuffer(m_SimpleTriangleData.VertexBuffer);

		m_SimpleTriangleData.Texture = Texture2D::Create(std::filesystem::path("assets/textures/grid.png"));

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
