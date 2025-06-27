#include "kcpch.h"
#include "Graphics/Core/IndexBuffer.h"

#include <glad/glad.h>

namespace KuchCraft {

	IndexBuffer::~IndexBuffer()
	{
		if (IsValid())
			glDeleteBuffers(1, &m_RendererID);
	}

	IndexBuffer::IndexBuffer(uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
	}

	IndexBuffer::IndexBuffer(uint32_t* data, uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t count)
	{
		KC_CORE_ASSERT(count > 0, "IndexBuffer count must be greater than 0.");

		return Ref<IndexBuffer>(new IndexBuffer(count));
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* data, uint32_t count)
	{
		KC_CORE_ASSERT(data != nullptr, "IndexBuffer data must not be null.");
		KC_CORE_ASSERT(count > 0, "IndexBuffer count must be greater than 0.");

		return Ref<IndexBuffer>(new IndexBuffer(data, count));
	}

	void IndexBuffer::SetData(uint32_t* data, uint32_t count, uint32_t offset)
	{
		KC_CORE_ASSERT(data != nullptr, "IndexBuffer data must not be null.");
		KC_CORE_ASSERT(count > 0, "IndexBuffer count must be greater than 0.");
		KC_CORE_ASSERT(IsValid(), "IndexBuffer is not valid.");

		uint32_t currentSize  = m_Count * sizeof(uint32_t);
		uint32_t requiredSize = count   * sizeof(uint32_t);
		uint32_t byteOffset   = offset  * sizeof(uint32_t);
		KC_CORE_ASSERT(byteOffset + requiredSize <= currentSize,
			"IndexBuffer data exceeds buffer size. Required: {}, Allocated: {}", byteOffset + requiredSize, currentSize);

		m_Count = count;
		glNamedBufferSubData(m_RendererID, byteOffset, count * sizeof(uint32_t), data);
	}

	void IndexBuffer::Bind() const
	{
		KC_CORE_ASSERT(IsValid(), "IndexBuffer is not valid.");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::Unbind() const
	{
		KC_CORE_ASSERT(IsValid(), "IndexBuffer is not valid.");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void IndexBuffer::SetDebugName(const std::string& name)
	{
		KC_CORE_ASSERT(!name.empty(), "IndexBuffer debug name must not be empty.");
		KC_CORE_ASSERT(IsValid(), "IndexBuffer is not valid.");

		m_DebugName = name;
		if (GLAD_GL_KHR_debug)
		{
			glObjectLabel(GL_BUFFER, m_RendererID, static_cast<GLsizei>(name.length()), name.c_str());
		}
		else
		{
			KC_CORE_WARN("Tried to set debug name on IndexBuffer, but GL_KHR_debug is not supported.");
		}
	}

}
