#include "kcpch.h"
#include "Graphics/Core/VertexBuffer.h"

#include <glad/glad.h>

namespace KuchCraft {

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
	{
		for (const auto& element : elements)
			AddElement(element);
	}

	void BufferLayout::AddElement(const BufferElement& element)
	{
		KC_CORE_ASSERT(element.Type != ShaderDataType::None, "BufferElement type cannot be None!");

		BufferElement newElement = element;
		newElement.Offset = m_CurrentOffset;

		m_Stride        += element.GetSize();
		m_CurrentOffset += element.GetSize();

		m_BufferElements.push_back(newElement);
	}

	VertexBuffer::~VertexBuffer()
	{
		if (IsValid())
			glDeleteBuffers(1, &m_RendererID);
	}

	VertexBuffer::VertexBuffer(VertexBufferDataUsage usage, size_t size, const void* data)
		: m_Usage(usage), m_Size(size)
	{
		KC_CORE_ASSERT(size > 0, "VertexBuffer size must be greater than 0.");

		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, data, (usage == VertexBufferDataUsage::Static) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

		KC_CORE_ASSERT(IsValid(), "Failed to create VertexBuffer!");
	}

	Ref<VertexBuffer> VertexBuffer::Create(VertexBufferDataUsage usage, size_t size, const void* data)
	{
		return Ref<VertexBuffer>(new VertexBuffer(usage, size, data));
	}

	void VertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		KC_CORE_ASSERT(IsValid(), "VertexBuffer is not valid.");
		KC_CORE_ASSERT(data != nullptr, "VertexBuffer data must not be null.");
		KC_CORE_ASSERT(size > 0, "VertexBuffer size must be greater than 0.");

		size_t currentSize  = m_Size;
		size_t requiredSize = size; 
		size_t byteOffset   = offset;
		KC_CORE_ASSERT(byteOffset + requiredSize <= currentSize,
			"VertexBuffer data exceeds buffer size. Required: {}, Allocated: {}", byteOffset + requiredSize, currentSize);

		glNamedBufferSubData(m_RendererID, byteOffset, size, data);
	}

	void VertexBuffer::SetLayout(const BufferLayout& layout)
	{
		KC_CORE_ASSERT(IsValid(), "VertexBuffer is not valid.");

		m_Layout = layout;
	}

	void VertexBuffer::SetDebugName(const std::string& name)
	{
		KC_CORE_ASSERT(IsValid(), "VertexBuffer is not valid.");
		KC_CORE_ASSERT(!name.empty(), "VertexBuffer debug name must not be empty.");

		m_DebugName = name;
		if (GLAD_GL_KHR_debug)
			glObjectLabel(GL_BUFFER, m_RendererID, static_cast<GLsizei>(name.length()), name.c_str());
	}

}