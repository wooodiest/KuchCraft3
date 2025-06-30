#pragma once

#include "Core/Config.h"

#include "Graphics/Core/IndexBuffer.h"
#include "Graphics/Core/VertexBuffer.h"
#include "Graphics/Core/VertexArray.h"
#include "Graphics/Core/Shader.h"
#include "Graphics/Core/ShaderLibrary.h"
#include "Graphics/Core/FrameBuffer.h"
#include "Graphics/Core/Texture.h"

namespace KuchCraft {

	class Renderer
	{
	public:	
		~Renderer();

		static Ref<Renderer> Create(Config config);

		void NewFrame();
		void EndFrame();

		/// Sets the current rendering layer index (used as implicit Z-depth).
		/// This approach allows layering UI, game objects, and overlays without relying on manual Z-positioning.
		void SetLayerIndex(int layerIndex) { m_CurrentLayerIndex = layerIndex; }

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

		int m_CurrentLayerIndex = 0;

		struct {
			Ref<VertexArray>  VertexArray;
			Ref<VertexBuffer> VertexBuffer;
			Ref<Shader>       Shader;
			Ref<Texture2D>	  Texture;
		} m_SimpleTriangleData;

		Ref<Texture2D> m_WhiteTexture;
		Ref<Texture2D> m_BlackTexture;

	private:
		Renderer(Config config);

		KC_DISALLOW_COPY(Renderer);
		KC_DISALLOW_MOVE(Renderer);
	};

}