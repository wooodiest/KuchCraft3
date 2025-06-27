#pragma once

namespace KuchCraft {

#pragma region DataTypes 

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Uint, Int
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return 1 * sizeof(float);
			case ShaderDataType::Float2:   return 2 * sizeof(float);
			case ShaderDataType::Float3:   return 3 * sizeof(float);
			case ShaderDataType::Float4:   return 4 * sizeof(float);
			case ShaderDataType::Uint:     return 1 * sizeof(uint32_t);
			case ShaderDataType::Int:      return 1 * sizeof(int32_t);
		}

		KC_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	static uint32_t ShaderDataTypeCount(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return 1;
			case ShaderDataType::Float2:   return 2;
			case ShaderDataType::Float3:   return 3;
			case ShaderDataType::Float4:   return 4;
			case ShaderDataType::Uint:     return 1;
			case ShaderDataType::Int:      return 1;
		}

		KC_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

#pragma endregion 
#pragma region BufferLayout
	struct BufferElement
	{
		std::string    Name;
		ShaderDataType Type       = ShaderDataType::None;
		uint32_t       Offset     = 0;
		bool           Normalized = false;

		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Normalized(normalized) {}

		uint32_t GetSize()  const { return ShaderDataTypeSize(Type); }
		uint32_t GetCount() const { return ShaderDataTypeCount(Type); }
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements);

		void AddElement(const BufferElement& element);

		inline [[nodiscard]] std::vector<BufferElement>::iterator begin() { return m_BufferElements.begin(); }
		inline [[nodiscard]] std::vector<BufferElement>::iterator end()   { return m_BufferElements.end();   }
		inline [[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return m_BufferElements.begin(); }
		inline [[nodiscard]] std::vector<BufferElement>::const_iterator end()   const { return m_BufferElements.end();   }

	private:
		std::vector<BufferElement> m_BufferElements;
		uint32_t m_Stride        = 0;
		uint32_t m_CurrentOffset = 0;
	};

#pragma endregion 

	enum class VertexBufferDataUsage
	{
		Static  = 0,
		Dynamic = 1
	};

	class VertexBuffer
	{
	public:
		~VertexBuffer();

		Ref<VertexBuffer> Create(VertexBufferDataUsage usage, size_t size, const void* data = nullptr);

		void SetData(const void* data, size_t size, size_t offset = 0);

		void Bind()   const;
		void Unbind() const;

		bool IsValid() const { return m_RendererID != 0; }	

		RendererID GetRendererID() const { return m_RendererID; }
		size_t     GetSize()       const { return m_Size;       }

		void SetLayout(const BufferLayout& layout);
		const BufferLayout& GetLayout() const { return m_Layout; }

		void SetDebugName(const std::string& name);
		const std::string& GetDebugName() const { return m_DebugName; }

	private:
		std::string  m_DebugName  = "UnnamedVertexBuffer";
		RendererID   m_RendererID = 0;
		size_t       m_Size       = 0;
		BufferLayout m_Layout;
		VertexBufferDataUsage m_Usage = VertexBufferDataUsage::Static;

		VertexBuffer(VertexBufferDataUsage usage, size_t size, const void* data = nullptr);

		KC_DISALLOW_MOVE(VertexBuffer);
		KC_DISALLOW_COPY(VertexBuffer);
	};

}