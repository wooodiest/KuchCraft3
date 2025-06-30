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
		void InitSimpleTriangleData();
		void RenderSimpleTriangle();

	private:
		Config m_Config;
		ShaderLibrary m_ShaderLibrary;
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

		struct {
			Ref<VertexArray>  VertexArray;
			Ref<VertexBuffer> VertexBuffer;
			Ref<Shader>       Shader;
			Ref<Texture2D>	  Texture;
		} m_SimpleTriangleData;

	private:
		Renderer(Config config);

		KC_DISALLOW_COPY(Renderer);
		KC_DISALLOW_MOVE(Renderer);
	};

}