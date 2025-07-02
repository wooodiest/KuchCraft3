#pragma once

namespace KuchCraft {

	class UniformBuffer
	{
	public:
		~UniformBuffer();

		static Ref<UniformBuffer> Create(size_t size);

		void SetData(const void* data, size_t size, uint32_t offset = 0);

		bool IsValid() const { return m_RendererID != 0; }

		size_t GetSize() const { return m_Size; }
		RendererID GetRendererID() const { return m_RendererID; }
		uint32_t GetBinding() const { return m_Binding; }

		void SetDebugName(const std::string& name);
		const std::string& GetDebugName() const { return m_DebugName; }

	private:
		RendererID m_RendererID = 0;
		size_t     m_Size    = 0;
		uint32_t   m_Binding = 0;

		std::string m_DebugName = "UnnamedUniformBuffer";

		UniformBuffer(size_t size);

		static inline std::set<uint32_t> s_FreeBindings;
		static inline uint32_t s_NextAvailableBinding = 0;

		static uint32_t AllocateBinding();
		static void ReleaseBinding(uint32_t binding);

		KC_DISALLOW_COPY(UniformBuffer);
		KC_DISALLOW_MOVE(UniformBuffer);
	};

}