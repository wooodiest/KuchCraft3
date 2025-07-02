#include "kcpch.h"
#include "Graphics/Core/UniformBuffer.h"

#include <glad/glad.h>

namespace KuchCraft {

	UniformBuffer::UniformBuffer(size_t size)
		: m_Size(size)
	{
		m_Binding = AllocateBinding();

		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_Binding, m_RendererID);
	}

	UniformBuffer::~UniformBuffer()
	{
		if (IsValid())
		{
			glDeleteBuffers(1, &m_RendererID);
			ReleaseBinding(m_Binding);
		}
	}

	Ref<UniformBuffer> UniformBuffer::Create(size_t size)
	{
		return Ref<UniformBuffer>(new UniformBuffer(size));
	}

	void UniformBuffer::SetData(const void* data, size_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

	void UniformBuffer::SetDebugName(const std::string& name)
	{
		KC_CORE_ASSERT(!name.empty(), "UniformBuffer debug name must not be empty.");
		KC_CORE_ASSERT(IsValid(), "UniformBuffer is not valid.");

		m_DebugName = name;
		if (GLAD_GL_KHR_debug)
			glObjectLabel(GL_BUFFER, m_RendererID, static_cast<GLsizei>(name.length()), name.c_str());
	}

	uint32_t UniformBuffer::AllocateBinding()
	{
		if (!s_FreeBindings.empty())
		{
			auto it = s_FreeBindings.begin();
			uint32_t binding = *it;
			s_FreeBindings.erase(it);
			return binding;
		}

		return s_NextAvailableBinding++;
	}

	void UniformBuffer::ReleaseBinding(uint32_t binding)
	{
		s_FreeBindings.insert(binding);
	}
}
