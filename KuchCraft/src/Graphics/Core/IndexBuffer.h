#pragma once

namespace KuchCraft {

	class IndexBuffer 
	{
	public:
		~IndexBuffer();

		static Ref<IndexBuffer> Create(uint32_t count);
		static Ref<IndexBuffer> Create(uint32_t* data, uint32_t count);

		void SetData(uint32_t* data, uint32_t count, uint32_t offset = 0);

		void Bind()   const;
		void Unbind() const;

		bool IsValid() const { return m_RendererID != 0; }

		RendererID GetRendererID() const { return m_RendererID;               }
		uint32_t   GetCount()      const { return m_Count;                    }
		size_t     GetSize()       const { return m_Count * sizeof(uint32_t); }

		void SetDebugName(const std::string& name);
		const std::string& GetDebugName() const { return m_DebugName; }

	private:
		std::string m_DebugName  = "UnnamedIndexBuffer";
		RendererID  m_RendererID = 0;
		uint32_t    m_Count      = 0;

		IndexBuffer(uint32_t count);
		IndexBuffer(uint32_t* data, uint32_t count);

		KC_DISALLOW_MOVE(IndexBuffer);
		KC_DISALLOW_COPY(IndexBuffer);
	};


}