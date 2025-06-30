#include "kcpch.h"
#include "Graphics/Core/VertexArray.h"

#include <glad/glad.h>

namespace KuchCraft {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:  return GL_FLOAT;
			case ShaderDataType::Float2: return GL_FLOAT;
			case ShaderDataType::Float3: return GL_FLOAT;
			case ShaderDataType::Float4: return GL_FLOAT;
			case ShaderDataType::Int:    return GL_INT;
			case ShaderDataType::Uint:   return GL_UNSIGNED_INT;	
		}

		KC_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	VertexArray::~VertexArray()
	{
		if (IsValid())
			glDeleteVertexArrays(1, &m_RendererID);
	}

	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);

		KC_CORE_ASSERT(IsValid(), "Failed to create VertexArray!");
	}

	Ref<VertexArray> VertexArray::Create()
	{
		return Ref<VertexArray>(new VertexArray());
	}

	void VertexArray::Bind() const
	{
		KC_CORE_ASSERT(IsValid(), "VertexArray is not valid.");

		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		KC_CORE_ASSERT(IsValid(), "VertexArray is not valid.");

		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		KC_CORE_ASSERT(vertexBuffer, "VertexBuffer cannot be null!");

		const BufferLayout& layout = vertexBuffer->GetLayout();

		glVertexArrayVertexBuffer(m_RendererID, m_AttributeBindingIndex, vertexBuffer->GetRendererID(), 0, layout.GetStride());
		for (const auto& element : layout)
		{
			glEnableVertexArrayAttrib(m_RendererID, m_AttributeIndex);
			if (element.Type == ShaderDataType::Int || element.Type == ShaderDataType::Uint)
			{
				glVertexArrayAttribIFormat(m_RendererID, m_AttributeIndex, element.GetCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type), element.Offset);
			}
			else
			{
				glVertexArrayAttribFormat(m_RendererID, m_AttributeIndex, element.GetCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, element.Offset);
			}

			glVertexArrayAttribBinding(m_RendererID, m_AttributeIndex, m_AttributeBindingIndex);
			m_AttributeIndex++;
		}

		m_AttributeBindingIndex++;
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		KC_CORE_ASSERT(indexBuffer, "IndexBuffer cannot be null!");

		glVertexArrayElementBuffer(m_RendererID, indexBuffer->GetRendererID());
		m_IndexBuffer = indexBuffer;
	}
	void VertexArray::SetDebugName(const std::string& name)
	{
		KC_CORE_ASSERT(!name.empty(), "VertexArray debug name must not be empty.");
		KC_CORE_ASSERT(IsValid(), "VertexArray is not valid.");

		m_DebugName = name;
		if (GLAD_GL_KHR_debug)
			glObjectLabel(GL_VERTEX_ARRAY, m_RendererID, static_cast<GLsizei>(name.length()), name.c_str());
	}
}