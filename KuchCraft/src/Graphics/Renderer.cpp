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
		InitializeRendererState();

		m_ShaderLibrary.SetPath(m_Config.Renderer.ShadersPath);

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

		InitQuads2D();
		InitPlanes();
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
		ResetStats();

		m_EnvironmentUniformBufferData.ViewProjection  = m_Camera ? m_Camera->GetViewProjection() : glm::mat4(1.0f);
		m_EnvironmentUniformBufferData.OrthoProjection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
		m_EnvironmentUniformBuffer->SetData(&m_EnvironmentUniformBufferData, sizeof(m_EnvironmentUniformBufferData));
	}

	void Renderer::EndFrame()
	{
		m_OffscreenRenderTarget->Bind();
		m_OffscreenRenderTarget->ClearAttachments();

		RenderQuads2D();
		RenderPlanes();

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

	void Renderer::DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad2D({ position.x, position.y, m_DepthFromZIndex }, size, color);
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
		DrawQuad2D({ position.x, position.y, m_DepthFromZIndex }, size, texture, tilingFactor, tintColor, uv0, uv1);
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
		DrawRotatedQuad2D({ position.x, position.y, m_DepthFromZIndex }, size, rotation, color);
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
		DrawRotatedQuad2D({ position.x, position.y, m_DepthFromZIndex }, size, rotation, texture, tilingFactor, tintColor, uv0, uv1);
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

	void Renderer::DrawQuad2D(const glm::mat4& transform, const glm::vec4& color)
	{
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

	void Renderer::DrawQuad2D(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, glm::vec2 uv0, glm::vec2 uv1)
	{
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

	void Renderer::DrawPlane(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::toMat4(glm::quat(rotation))
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
		glm::vec3 normal = glm::normalize(normalMatrix * plane_normal);

		int textureID = 0; /// White texture
		for (uint32_t i = 0; i < plane_vertex_count; i++)
		{
			m_Planes.Vertices.emplace_back(
				glm::vec3(transform * plane_vertex_positions[i]),
				normal,
				color,
				plane_vertex_texture_coords[i],
				textureID
			);
		}
	}

	void Renderer::DrawPlane(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, glm::vec2 uv0, glm::vec2 uv1)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::toMat4(glm::quat(rotation))
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
		glm::vec3 normal = glm::normalize(normalMatrix * plane_normal);

		int textureID = texture->GetRendererID();
		for (uint32_t i = 0; i < plane_vertex_count; i++)
		{
			glm::vec2 baseUV = plane_vertex_texture_coords[i];
			glm::vec2 uv = glm::mix(uv0, uv1, baseUV) * tilingFactor;

			m_Planes.Vertices.emplace_back(
				glm::vec3(transform * plane_vertex_positions[i]),
				normal,
				tintColor,
				uv,
				textureID
			);
		}
	}

	void Renderer::DrawPlane(const glm::mat4& transform, const glm::vec4& color)
	{
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
		glm::vec3 normal = glm::normalize(normalMatrix * plane_normal);

		int textureID = 0; /// White texture
		for (uint32_t i = 0; i < plane_vertex_count; i++)
		{
			m_Planes.Vertices.emplace_back(
				glm::vec3(transform * plane_vertex_positions[i]),
				normal,
				color,
				plane_vertex_texture_coords[i],
				textureID
			);
		}
	}

	void Renderer::DrawPlane(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, glm::vec2 uv0, glm::vec2 uv1)
	{
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
		glm::vec3 normal = glm::normalize(normalMatrix * plane_normal);

		int textureID = texture->GetRendererID();
		for (uint32_t i = 0; i < plane_vertex_count; i++)
		{
			glm::vec2 baseUV = plane_vertex_texture_coords[i];
			glm::vec2 uv = glm::mix(uv0, uv1, baseUV) * tilingFactor;

			m_Planes.Vertices.emplace_back(
				glm::vec3(transform * plane_vertex_positions[i]),
				normal,
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
		constexpr glm::vec4 clear_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Renderer::SetRenderTargetToDefault()
	{
		auto [width, height] = Application::Get().GetWindow()->GetSize();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
	}

	void Renderer::InitializeRendererState()
	{
		m_RendererState.ForceSet = true; /// Force initial state to be set

		SetPolygonMode(m_RendererState.PolygonMode);
		SetFrontFaceWinding(m_RendererState.FrontFaceWinding);

		SetDepthTest(m_RendererState.DepthTestEnabled);
		SetDepthFunc(m_RendererState.DepthFunc);

		SetBlend(m_RendererState.BlendEnabled);
		SetBlendFunc(m_RendererState.SrcBlendFunc, m_RendererState.DstBlendFunc);

		SetCullFace(m_RendererState.CullFaceEnabled);
		SetCullMode(m_RendererState.CullFaceMode);

		SetPolygonOffset(m_RendererState.PolygonOffsetEnabled, m_RendererState.PolygonOffsetFactor, m_RendererState.PolygonOffsetUnits);

		m_RendererState.ForceSet = false;
	}

	void Renderer::SetPolygonMode(PolygonMode mode)
	{
		if (!m_RendererState.ForceSet && m_RendererState.PolygonMode == mode)
			return;

		m_RendererState.PolygonMode = mode;
		switch (mode)
		{
			case PolygonMode::Fill:   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   break;
			case PolygonMode::Line:   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   break;
			case PolygonMode::Point:  glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);  break;
			default:
			{
				KC_CORE_ERROR("Invalid PolygonMode: {}", (int)mode);
				SetPolygonMode(PolygonMode::Fill);
				break;
			}
		}
	}

	void Renderer::SetFrontFaceWinding(FaceWinding mode)
	{
		if (!m_RendererState.ForceSet && m_RendererState.FrontFaceWinding == mode)
			return;

		m_RendererState.FrontFaceWinding = mode;
		switch (mode)
		{
			case FaceWinding::CounterClockwise: glFrontFace(GL_CCW); break;
			case FaceWinding::Clockwise:        glFrontFace(GL_CW);  break;
			default : 
			{
				KC_CORE_ERROR("Invalid FrontFace mode: {}", (int)mode);
				SetFrontFaceWinding(FaceWinding::CounterClockwise);
				break;
			}
		}
	}

	void Renderer::SetDepthTest(bool enabled)
	{
		if (!m_RendererState.ForceSet && m_RendererState.DepthTestEnabled == enabled)
			return;

		m_RendererState.DepthTestEnabled = enabled;
		if (enabled)
		{
			glEnable(GL_DEPTH_TEST);
			SetDepthFunc(m_RendererState.DepthFunc);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	void Renderer::SetDepthFunc(DepthFunc func)
	{
		if (!m_RendererState.ForceSet && m_RendererState.DepthFunc == func)
			return;

		m_RendererState.DepthFunc = func;
		switch (func)
		{
			case DepthFunc::Never:        glDepthFunc(GL_NEVER);    break;
			case DepthFunc::Less:         glDepthFunc(GL_LESS);     break;
			case DepthFunc::Equal:        glDepthFunc(GL_EQUAL);    break;
			case DepthFunc::LessEqual:    glDepthFunc(GL_LEQUAL);   break;
			case DepthFunc::Greater:      glDepthFunc(GL_GREATER);  break;
			case DepthFunc::NotEqual:     glDepthFunc(GL_NOTEQUAL); break;
			case DepthFunc::GreaterEqual: glDepthFunc(GL_GEQUAL);   break;
			case DepthFunc::Always:       glDepthFunc(GL_ALWAYS);   break;
			default:
			{
				KC_CORE_ERROR("Invalid DepthFunc mode: {}", (int)func);
				SetDepthFunc(DepthFunc::LessEqual);
				break;
			}
		}
	}

	void Renderer::SetBlend(bool enabled)
	{
		if (!m_RendererState.ForceSet && m_RendererState.BlendEnabled == enabled)
			return;

		m_RendererState.BlendEnabled = enabled;
		if (enabled)
		{
			glEnable(GL_BLEND);
			SetBlendFunc(m_RendererState.SrcBlendFunc, m_RendererState.DstBlendFunc);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	GLenum BlendFuncToGLenum(BlendFunc func)
	{
		switch (func)
		{
			case BlendFunc::Zero:             return GL_ZERO;
			case BlendFunc::One:              return GL_ONE;
			case BlendFunc::SrcAlpha:         return GL_SRC_ALPHA;
			case BlendFunc::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
		}

		return GL_ONE;
	}

	void Renderer::SetBlendFunc(BlendFunc src, BlendFunc dst)
	{
		if (!m_RendererState.ForceSet && m_RendererState.SrcBlendFunc == src && m_RendererState.DstBlendFunc == dst)
			return;

		m_RendererState.SrcBlendFunc = src;
		m_RendererState.DstBlendFunc = dst;

		if (src == BlendFunc::None || dst == BlendFunc::None)
		{
			KC_CORE_ERROR("Invalid BlendFunc combination: Src = {}, Dst = {}", (int)src, (int)dst);
			SetBlendFunc(BlendFunc::SrcAlpha, BlendFunc::OneMinusSrcAlpha);
			return;
		}

		glBlendFunc(BlendFuncToGLenum(src), BlendFuncToGLenum(dst));
		glBlendEquation(GL_FUNC_ADD);
	}

	void Renderer::SetCullFace(bool enabled)
	{
		if (!m_RendererState.ForceSet && m_RendererState.CullFaceEnabled == enabled)
			return;

		m_RendererState.CullFaceEnabled = enabled;
		if (enabled)
		{
			glEnable(GL_CULL_FACE);
			SetCullMode(m_RendererState.CullFaceMode);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

	void Renderer::SetCullMode(CullMode mode)
	{
		if (!m_RendererState.ForceSet && m_RendererState.CullFaceMode == mode)
			return;

		m_RendererState.CullFaceMode = mode;
		switch (mode)
		{
			case CullMode::Front:        glCullFace(GL_FRONT);          break;
			case CullMode::Back:         glCullFace(GL_BACK);           break;
			case CullMode::FrontAndBack: glCullFace(GL_FRONT_AND_BACK); break;
			default:
			{
				KC_CORE_ERROR("Invalid CullMode: {}", (int)mode);
				SetCullMode(CullMode::Back);
				break;
			}
		}
	}

	void Renderer::SetPolygonOffset(bool enabled, float factor, float units)
	{
		if (!m_RendererState.ForceSet && m_RendererState.PolygonOffsetEnabled == enabled && 
			m_RendererState.PolygonOffsetFactor == factor && m_RendererState.PolygonOffsetUnits == units)
			return;

		m_RendererState.PolygonOffsetEnabled = enabled;
		m_RendererState.PolygonOffsetFactor  = factor;
		m_RendererState.PolygonOffsetUnits   = units;

		if (enabled)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(factor, units);
		}
		else
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	inline GLenum ToOpenGLPrimitive(PrimitiveTopology topology)
	{
		switch (topology)
		{
			case PrimitiveTopology::Points:             return GL_POINTS;
			case PrimitiveTopology::Lines:              return GL_LINES;
			case PrimitiveTopology::LineStrip:          return GL_LINE_STRIP;
			case PrimitiveTopology::Triangles:          return GL_TRIANGLES;
			case PrimitiveTopology::TriangleStrip:      return GL_TRIANGLE_STRIP;
			case PrimitiveTopology::TriangleFan:        return GL_TRIANGLE_FAN;
			case PrimitiveTopology::LinesAdjacency:     return GL_LINES_ADJACENCY;
			case PrimitiveTopology::TrianglesAdjacency: return GL_TRIANGLES_ADJACENCY;
			case PrimitiveTopology::Patches:            return GL_PATCHES;
			default:
				KC_CORE_ERROR("Unknown PrimitiveTopology: {}", static_cast<int>(topology));
				return GL_TRIANGLES;
		}
	}

	void Renderer::DrawArrays(PrimitiveTopology topology, uint32_t firstVertex, uint32_t vertexCount)
	{
		glDrawArrays(ToOpenGLPrimitive(topology), firstVertex, vertexCount);

		m_Stats.DrawCalls++;
		m_Stats.Primitives += GetPrimitiveCount(topology, vertexCount);
	}

	void Renderer::DrawArraysInstanced(PrimitiveTopology topology, uint32_t firstVertex, uint32_t vertexCount, uint32_t instanceCount)
	{
		glDrawArraysInstanced(ToOpenGLPrimitive(topology), firstVertex, vertexCount, instanceCount);

		m_Stats.DrawCalls++;
		m_Stats.Primitives += GetPrimitiveCount(topology, vertexCount) * instanceCount;
	}

	void Renderer::DrawElements(PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex)
	{
		glDrawElements(ToOpenGLPrimitive(topology), indexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * firstIndex));

		m_Stats.DrawCalls++;
		m_Stats.Primitives += GetPrimitiveCount(topology, indexCount);
	}

	void Renderer::DrawElementsInstanced(PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount)
	{
		glDrawElementsInstanced(ToOpenGLPrimitive(topology), indexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * firstIndex), instanceCount);

		m_Stats.DrawCalls++;
		m_Stats.Primitives += GetPrimitiveCount(topology, indexCount) * instanceCount;
	}

	void Renderer::DrawElementsBaseVertex(PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex, int32_t baseVertex)
	{
		glDrawElementsBaseVertex(ToOpenGLPrimitive(topology), indexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * firstIndex), baseVertex);

		m_Stats.DrawCalls++;
		m_Stats.Primitives += GetPrimitiveCount(topology, indexCount);
	}

	void Renderer::DrawElementsInstancedBaseVertex(PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, int32_t baseVertex)
	{
		glDrawElementsInstancedBaseVertex(ToOpenGLPrimitive(topology), indexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * firstIndex), instanceCount, baseVertex);

		m_Stats.DrawCalls++;
		m_Stats.Primitives += GetPrimitiveCount(topology, indexCount) * instanceCount;
	}

	void Renderer::DrawElementsInstancedBaseVertexBaseInstance(PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, int32_t baseVertex, uint32_t baseInstance)
	{
		glDrawElementsInstancedBaseVertexBaseInstance(ToOpenGLPrimitive(topology), indexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * firstIndex), instanceCount, baseVertex, baseInstance);

		m_Stats.DrawCalls++;
		m_Stats.Primitives += GetPrimitiveCount(topology, indexCount) * instanceCount;
	}

	void Renderer::DrawRangeElements(PrimitiveTopology topology, uint32_t start, uint32_t end, uint32_t count)
	{
		glDrawRangeElements(ToOpenGLPrimitive(topology), start, end, count, GL_UNSIGNED_INT, nullptr);

		m_Stats.DrawCalls++;
		m_Stats.Primitives += GetPrimitiveCount(topology, count);
	}

	void Renderer::MultiDrawArrays(PrimitiveTopology topology, const std::vector<int>& firsts, const std::vector<int>& counts)
	{
		glMultiDrawArrays(ToOpenGLPrimitive(topology), firsts.data(), counts.data(), static_cast<GLsizei>(counts.size()));

		m_Stats.DrawCalls++;
		for (auto count : counts)
			m_Stats.Primitives += GetPrimitiveCount(topology, count);
	}

	void Renderer::MultiDrawElements(PrimitiveTopology topology, const std::vector<GLsizei>& counts, const std::vector<void*>& offsets)
	{
		glMultiDrawElements(ToOpenGLPrimitive(topology), counts.data(), GL_UNSIGNED_INT, offsets.data(), static_cast<GLsizei>(counts.size()));

		m_Stats.DrawCalls++;
		for (auto count : counts)
			m_Stats.Primitives += GetPrimitiveCount(topology, count);
	}

	void Renderer::DrawArraysIndirect(PrimitiveTopology topology, const void* indirect)
	{
		glDrawArraysIndirect(ToOpenGLPrimitive(topology), indirect);

		m_Stats.DrawCalls++;
	}

	void Renderer::DrawElementsIndirect(PrimitiveTopology topology, const void* indirect)
	{
		glDrawElementsIndirect(ToOpenGLPrimitive(topology), GL_UNSIGNED_INT, indirect);

		m_Stats.DrawCalls++;
	}


	void Renderer::ResetStats()
	{
		m_Stats.DrawCalls  = 0;
		m_Stats.Primitives = 0;
	}

	void Renderer::InitQuads2D()
	{
		/// Graphics
		m_Quads2D.MaxQuadsInBatch = m_Config.Renderer.MaxQuadsInBatch;
		m_Quads2D.MaxIndices  = m_Quads2D.MaxQuadsInBatch * quad_index_count;
		m_Quads2D.MaxVertices = m_Quads2D.MaxQuadsInBatch * quad_vertex_count;

		m_Quads2D.Shader = m_ShaderLibrary.Load(std::filesystem::path("Quads2D.glsl"));
		m_Quads2D.Shader->Bind();

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

		auto setupTexturesSamplers = [this](Shader* shader) {
			std::vector<int> samplers;
			samplers.reserve(m_Config.Renderer.MaxCombinedTextureSlots);
			for (int i = 0; i < m_Config.Renderer.MaxCombinedTextureSlots; i++)
				samplers.push_back(i);

			shader->SetIntArray("u_Textures", samplers.data(), m_Config.Renderer.MaxCombinedTextureSlots);
		};	

		setupTexturesSamplers(m_Quads2D.Shader.get());
		m_Quads2D.Shader->AddReloadCallback(setupTexturesSamplers);

		/// Internal
		m_Quads2D.Textures.resize(m_Config.Renderer.MaxCombinedTextureSlots, 0);
		m_Quads2D.Textures[0] = m_WhiteTexture->GetRendererID();

		m_Quads2D.Vertices.reserve(m_Quads2D.MaxVertices);
	}

	void Renderer::RenderQuads2D()
	{
		if (m_Quads2D.Vertices.empty())
			return;

		SetBlend(true);
		SetBlendFunc(BlendFunc::SrcAlpha, BlendFunc::OneMinusSrcAlpha);
		SetCullFace(false);
		SetDepthTest(true);
		SetDepthFunc(DepthFunc::LessEqual);
		SetPolygonMode(PolygonMode::Fill);
		SetPolygonOffset(false);

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
		
		DrawElements(PrimitiveTopology::Triangles, m_Quads2D.CurrentIndexCount, 0);
	}

	void Renderer::InitPlanes()
	{
		/// Graphics
		m_Planes.MaxPlanesInBatch = m_Config.Renderer.MaxPlanesInBatch;
		m_Planes.MaxIndices  = m_Planes.MaxPlanesInBatch * plane_index_count;
		m_Planes.MaxVertices = m_Planes.MaxPlanesInBatch * plane_vertex_count;

		m_Planes.Shader = m_ShaderLibrary.Load(std::filesystem::path("Plane.glsl"));
		m_Planes.Shader->Bind();

		m_Planes.VertexArray = VertexArray::Create();
		m_Planes.VertexArray->Bind();
		m_Planes.VertexArray->SetDebugName("Planes_VAO");

		m_Planes.VertexBuffer = VertexBuffer::Create(VertexBufferDataUsage::Dynamic, m_Planes.MaxVertices * sizeof(VertexPlane));
		m_Planes.VertexBuffer->SetDebugName("Planes_VBO");
		m_Planes.VertexBuffer->SetLayout(m_Planes.Shader->GetVertexInputLayout());
		m_Planes.VertexArray->AddVertexBuffer(m_Planes.VertexBuffer);

		std::vector<uint32_t> indices;
		indices.reserve(m_Planes.MaxIndices);
		uint32_t offset = 0;
		for (uint32_t i = 0; i < m_Planes.MaxIndices; i += plane_index_count)
		{
			indices.push_back(offset + 0);
			indices.push_back(offset + 1);
			indices.push_back(offset + 2);
			indices.push_back(offset + 2);
			indices.push_back(offset + 3);
			indices.push_back(offset + 0);

			offset += quad_vertex_count;
		}

		m_Planes.IndexBuffer = IndexBuffer::Create(indices.data(), m_Planes.MaxIndices);
		m_Planes.IndexBuffer->SetDebugName("Planes_IBO");
		m_Planes.VertexArray->SetIndexBuffer(m_Planes.IndexBuffer);

		auto setupTexturesSamplers = [this](Shader* shader) {
			std::vector<int> samplers;
			samplers.reserve(m_Config.Renderer.MaxCombinedTextureSlots);
			for (int i = 0; i < m_Config.Renderer.MaxCombinedTextureSlots; i++)
				samplers.push_back(i);

			shader->SetIntArray("u_Textures", samplers.data(), m_Config.Renderer.MaxCombinedTextureSlots);
		};

		setupTexturesSamplers(m_Planes.Shader.get());
		m_Planes.Shader->AddReloadCallback(setupTexturesSamplers);

		/// Internal
		m_Planes.Textures.resize(m_Config.Renderer.MaxCombinedTextureSlots, 0);
		m_Planes.Textures[0] = m_WhiteTexture->GetRendererID();

		m_Planes.Vertices.reserve(m_Planes.MaxVertices);
	}

	void Renderer::RenderPlanes()
	{
		if (m_Planes.Vertices.empty())
			return;

		SetBlend(true);
		SetBlendFunc(BlendFunc::SrcAlpha, BlendFunc::OneMinusSrcAlpha);
		SetCullFace(false);
		SetDepthTest(true);
		SetDepthFunc(DepthFunc::LessEqual);
		SetPolygonMode(PolygonMode::Fill);
		SetPolygonOffset(false);

		m_Planes.Shader     ->Bind();
		m_WhiteTexture      ->Bind(0);
		m_Planes.VertexArray->Bind();

		m_Planes.VertexOffset = 0;

		StartBatchPlanes();
		for (size_t i = 0; i < m_Planes.Vertices.size(); i += plane_vertex_count)
		{
			if (m_Planes.CurrentIndexCount == m_Planes.MaxIndices)
				NextBatchPlanes();

			/// TextureSlot temporarily holds the texture rendererID
			if (m_Planes.Vertices[i].TextureSlot == 0)
			{
				/// Just color, white texture is bound to slot 0
				m_Planes.CurrentIndexCount += plane_index_count;
			}
			else
			{
				/// Do we already have assigned slot to that texture?
				int textureSlot = 0;
				for (size_t j = 1; j < m_Planes.CurrentTextureSlot; j++)
				{
					if (m_Planes.Textures[j] == (RendererID)m_Planes.Vertices[i].TextureSlot) /// TextureSlot temporarily holds the texture rendererID
					{
						textureSlot = (int)j;
						break;
					}
				}

				/// If not, do it
				if (textureSlot == 0)
				{
					if (m_Planes.CurrentTextureSlot >= m_Config.Renderer.MaxCombinedTextureSlots)
						NextBatchPlanes();

					textureSlot = (int)m_Planes.CurrentTextureSlot;
					m_Planes.Textures[m_Planes.CurrentTextureSlot] = (RendererID)m_Planes.Vertices[i].TextureSlot;
					m_Planes.CurrentTextureSlot++;
				}

				m_Planes.Vertices[i + 0].TextureSlot = textureSlot;
				m_Planes.Vertices[i + 1].TextureSlot = textureSlot;
				m_Planes.Vertices[i + 2].TextureSlot = textureSlot;
				m_Planes.Vertices[i + 3].TextureSlot = textureSlot;

				m_Planes.CurrentIndexCount += plane_index_count;
			}
		}

		FlushPlanes();

		m_Planes.Vertices.clear();
	}

	void Renderer::StartBatchPlanes()
	{
		m_Planes.CurrentIndexCount = 0;
		m_Planes.CurrentTextureSlot = 1; /// 0 is reserved for a default white texture
	}

	void Renderer::NextBatchPlanes()
	{
		FlushPlanes();
		StartBatchPlanes();
	}

	void Renderer::FlushPlanes()
	{
		if (m_Planes.CurrentIndexCount == 0)
			return;

		uint32_t vertexCount = m_Planes.CurrentIndexCount / plane_index_count * plane_vertex_count;
		m_Planes.VertexBuffer->SetData(&m_Planes.Vertices[m_Planes.VertexOffset], vertexCount * sizeof(VertexPlane));

		m_Planes.VertexOffset += vertexCount;
		for (int slot = 1; slot < (int)m_Planes.CurrentTextureSlot; slot++)
			Texture::Bind(slot, m_Planes.Textures[slot]);

		DrawElements(PrimitiveTopology::Triangles, m_Planes.CurrentIndexCount, 0);
	}

}
