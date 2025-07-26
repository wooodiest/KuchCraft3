#pragma once

#include "Core/Config.h"
#include "Core/LayerStack.h"

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

		/// Should be called by application when window is resized
		void OnWindowResize(int width, int height);

#pragma region DrawCommands
	public:
		void DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		void DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		void DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), glm::vec2 uv0 = glm::vec2(0.0f), glm::vec2 uv1 = glm::vec2(1.0f));
		void DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), glm::vec2 uv0 = glm::vec2(0.0f), glm::vec2 uv1 = glm::vec2(1.0f));

		void DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		void DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		
		void DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), glm::vec2 uv0 = glm::vec2(0.0f), glm::vec2 uv1 = glm::vec2(1.0f));
		void DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), glm::vec2 uv0 = glm::vec2(0.0f), glm::vec2 uv1 = glm::vec2(1.0f));

		void DrawQuad2D(const glm::mat4& transform, const glm::vec4& color);
		void DrawQuad2D(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), glm::vec2 uv0 = glm::vec2(0.0f), glm::vec2 uv1 = glm::vec2(1.0f));

#pragma endregion

#pragma region ZIndex
	public:
		/// Updates the object's Z index and maps it to a normalized depth value used for depth buffering.
		void SetZIndex(float zIndex) {
			m_ZIndex = zIndex;
			m_DepthFromZIndex = 1.0f - m_ZIndex / max_layers;
		}

	private:
		float m_ZIndex = 0.0f;
		float m_DepthFromZIndex = 0.0f;

#pragma endregion

#pragma region RendererInitialization
	private:
		void CheckExtensions();
		void SetupLogging();

#pragma endregion

#pragma region FrameBuffers
	private:
		void ClearDefaultFrameBuffer();
		void SetRenderTargetToDefault();

		Ref<FrameBuffer> m_OffscreenRenderTarget;

#pragma endregion

#pragma region Shaders
	private:
		void SetGlobalSubstitutions();

		ShaderLibrary m_ShaderLibrary;

#pragma endregion

#pragma region UniformBuffers
	private:
		struct {
			glm::mat4 ViewProjection;
			glm::mat4 OrthoProjection;
		} m_EnvironmentUniformBufferData;

		Ref<UniformBuffer> m_EnvironmentUniformBuffer;

#pragma endregion

#pragma region Textures
	public:
		Ref<Texture2D> GetWhiteTexture() const { return m_WhiteTexture; }
		Ref<Texture2D> GetBlackTexture() const { return m_BlackTexture; }

	private:	
		Ref<Texture2D> m_WhiteTexture;
		Ref<Texture2D> m_BlackTexture;

#pragma endregion

#pragma region RendererState
	private:
		struct {
			bool ForceSet = false;

			PolygonMode PolygonMode = PolygonMode::Fill;
			FaceWinding FrontFaceWinding = FaceWinding::CounterClockwise;

			bool DepthTestEnabled = true;
			DepthFunc DepthFunc = DepthFunc::LessEqual;

			bool BlendEnabled = true;
			BlendFunc SrcBlendFunc = BlendFunc::SrcAlpha;
			BlendFunc DstBlendFunc = BlendFunc::OneMinusSrcAlpha;

			bool CullFaceEnabled = false;
			CullMode CullFaceMode = CullMode::Back;

			bool  PolygonOffsetEnabled = false;
			float PolygonOffsetFactor  = 0.0f;
			float PolygonOffsetUnits   = 0.0f;

		} m_RendererState;

		void InitializeRendererState();
		void SetPolygonMode(PolygonMode mode);
		void SetDepthTest(bool enabled);
		void SetDepthFunc(DepthFunc func);
		void SetBlend(bool enabled);
		void SetBlendFunc(BlendFunc src, BlendFunc dst);
		void SetCullFace(bool enabled);
		void SetFrontFaceWinding(FaceWinding mode);
		void SetCullMode(CullMode mode);
		void SetPolygonOffset(bool enabled, float factor = 0.0f, float units = 0.0f);
	
#pragma endregion

#pragma region RendererDrawCommands
	private:
		void DrawArrays(PrimitiveTopology topology, uint32_t firstVertex, uint32_t vertexCount);
		void DrawArraysInstanced(PrimitiveTopology topology, uint32_t firstVertex, uint32_t vertexCount, uint32_t instanceCount);

		void DrawElements(PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex);
		void DrawElementsInstanced(PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount);
		void DrawElementsBaseVertex(PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex, int32_t baseVertex);

		void DrawElementsInstancedBaseVertex(PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, int32_t baseVertex);
		void DrawElementsInstancedBaseVertexBaseInstance( PrimitiveTopology topology, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, int32_t baseVertex, uint32_t baseInstance);
		
		void DrawRangeElements(PrimitiveTopology topology, uint32_t start, uint32_t end, uint32_t count);

		void MultiDrawArrays(PrimitiveTopology topology, const std::vector<int>& firsts, const std::vector<int>& counts);
		void MultiDrawElements(PrimitiveTopology topology, const std::vector<GLsizei>& counts, const std::vector<void*>& offsets);

		void DrawArraysIndirect(PrimitiveTopology topology, const void* indirect);
		void DrawElementsIndirect(PrimitiveTopology topology, const void* indirect);

#pragma endregion

#pragma region Stats
	private:
		struct {
			uint32_t DrawCalls  = 0;
			uint32_t Primitives = 0;

		} m_Stats;
		
		void ResetStats();

	public:
		const auto& GetStats() const { return m_Stats; }

#pragma endregion

#pragma region Quads2D
	private:
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

#pragma endregion

	private:
		Config m_Config;
		Renderer(Config config);

		friend class RendererLayer;

		KC_DISALLOW_COPY(Renderer);
		KC_DISALLOW_MOVE(Renderer);
	};

}