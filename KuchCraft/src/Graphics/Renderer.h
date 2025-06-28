#pragma once

#include "Core/Config.h"

#include "Graphics/Core/IndexBuffer.h"
#include "Graphics/Core/VertexBuffer.h"
#include "Graphics/Core/VertexArray.h"
#include "Graphics/Core/Shader.h"
#include "Graphics/Core/ShaderLibrary.h"

namespace KuchCraft {

	class Renderer
	{
	public:	
		~Renderer();

		static Ref<Renderer> Create(Config config);

		void NewFrame();
		void EndFrame();
	private:
		void SetGlobalSubstitutions();

		void InitSimpleTriangleData();

	private:
		void RenderSimpleTriangle();

	private:
		Config m_Config;
		ShaderLibrary m_ShaderLibrary;

		struct {
			Ref<VertexArray>  VertexArray;
			Ref<VertexBuffer> VertexBuffer;
			Ref<Shader>       Shader;
		} m_SimpleTriangleData;

	private:
		Renderer(Config config);

		KC_DISALLOW_COPY(Renderer);
		KC_DISALLOW_MOVE(Renderer);
	};

}