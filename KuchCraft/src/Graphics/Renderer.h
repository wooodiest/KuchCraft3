#pragma once

#include "Core/Config.h"

#include "Graphics/Core/IndexBuffer.h"
#include "Graphics/Core/VertexBuffer.h"
#include "Graphics/Core/VertexArray.h"
#include "Graphics/Core/Shader.h"
#include "Graphics/Core/ShaderLibrary.h"
#include "Graphics/Core/FrameBuffer.h"
#include "Graphics/Core/Texture.h"
#include "Graphics/Core/UniformBuffer.h"

namespace KuchCraft {

	class Renderer
	{
	public:	
		~Renderer();

		static Ref<Renderer> Create(Config config);

		void NewFrame();
		void EndFrame();

	public:
		void DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		void DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		void DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), glm::vec2 uv0 = glm::vec2(0.0f), glm::vec2 uv1 = glm::vec2(1.0f));
		void DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), glm::vec2 uv0 = glm::vec2(0.0f), glm::vec2 uv1 = glm::vec2(1.0f));

		void DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		void DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		
		void DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), glm::vec2 uv0 = glm::vec2(0.0f), glm::vec2 uv1 = glm::vec2(1.0f));
		void DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), glm::vec2 uv0 = glm::vec2(0.0f), glm::vec2 uv1 = glm::vec2(1.0f));

	public:
		/// Sets the current rendering layer index (used as implicit Z-depth).
		/// This approach allows layering UI, game objects, and overlays without relying on manual Z-positioning.
		void SetLayerIndex(int layerIndex);

		/// Should be called by application when window is resized
		void OnWindowResize(int width, int height);

		Ref<Texture2D> GetWhiteTexture() const { return m_WhiteTexture; }
		Ref<Texture2D> GetBlackTexture() const { return m_BlackTexture; }

	private:
		void CheckExtensions();
		void SetupLogging();
		void SetGlobalSubstitutions();

		void ClearDefaultFrameBuffer();
		void SetRenderTargetToDefault();

	private:
		Config           m_Config;
		ShaderLibrary    m_ShaderLibrary;
		Ref<FrameBuffer> m_OffscreenRenderTarget;

		int   m_CurrentLayerIndex = 0;
		float m_CurrentOrthoDepth = 1.0f;

		Ref<Texture2D> m_WhiteTexture;
		Ref<Texture2D> m_BlackTexture;

		struct {
			glm::mat4 ViewProjection;
			glm::mat4 OrthoProjection;
		} m_EnvironmentUniformBufferData;

		Ref<UniformBuffer> m_EnvironmentUniformBuffer;

		/// Quads 2D
		struct {
			uint32_t MaxQuadsInBatch = 10'000;
			uint32_t MaxIndices  = MaxQuadsInBatch * quad_index_count;
			uint32_t MaxVertices = MaxQuadsInBatch * quad_vertex_count;

			std::vector<VertexQuad2D> Vertices;
			std::vector<RendererID>   Textures;
			size_t CurrentTextureSlot = 1; /// 0 is reserved for a default white texture

			uint32_t CurrentIndexCount = 0;
			size_t   VertexOffset      = 0;

			Ref<VertexArray>  VertexArray;
			Ref<VertexBuffer> VertexBuffer;
			Ref<IndexBuffer>  IndexBuffer;
			Ref<Shader>       Shader;
		} m_Quads2D;

		void InitQuads2D();
		void RenderQuads2D();
		void StartBatchQuads2D();
		void NextBatchQuads2D();
		void FlushQuads2D();

	private:
		Renderer(Config config);

		KC_DISALLOW_COPY(Renderer);
		KC_DISALLOW_MOVE(Renderer);
	};

}