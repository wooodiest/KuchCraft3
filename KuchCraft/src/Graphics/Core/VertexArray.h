#pragma once

#include "Graphics/Core/IndexBuffer.h"
#include "Graphics/Core/VertexBuffer.h"

namespace KuchCraft {

	class VertexArray
	{
	public:
		~VertexArray();

		static Ref<VertexArray> Create();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

		bool IsValid() const { return m_RendererID != 0; }

		RendererID GetRendererID() const { return m_RendererID; }

		void SetDebugName(const std::string& name);
		const std::string& GetDebugName() const { return m_DebugName; }

	private:
		std::string m_DebugName  = "UnnamedVertexArray";
		RendererID  m_RendererID = 0;

		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer>               m_IndexBuffer;

		uint32_t m_AttributeIndex        = 0;
		uint32_t m_AttributeBindingIndex = 0;

		VertexArray();

		KC_DISALLOW_COPY(VertexArray);
		KC_DISALLOW_MOVE(VertexArray);
	};

}