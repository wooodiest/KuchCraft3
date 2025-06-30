#pragma once

#include "Graphics/Core/TextureTypes.h"

namespace KuchCraft {

	struct TextureSpecification
	{
		TextureFormat Format = TextureFormat::RGBA;
		TextureFilter Filter = TextureFilter::Linear;
		TextureWrap   Wrap   = TextureWrap::Clamp;
		int Width  = 0;
		int Height = 0;
		bool GenerateMips  = false;
		uint32_t MipLevels = 0;      /// 0 = auto-calculate if GenerateMips == true
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void Bind(int slot = 0) const = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual bool IsValid() const = 0;

		virtual TextureFormat GetFormat() const = 0;
		virtual TextureType   GetType()   const = 0;

		virtual int GetWidth()  const = 0;
		virtual int GetHeight() const = 0;
		virtual std::pair<int, int> GetSize() const = 0;

		virtual void SetDebugName(const std::string& name) = 0;
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D();

		static Ref<Texture2D> Create(const TextureSpecification& spec);
		static Ref<Texture2D> Create(const std::filesystem::path& path, const TextureSpecification& spec = {});

		virtual void Bind(int slot = 0) const override;

		virtual RendererID GetRendererID() const override { return m_RendererID; }
		virtual bool IsValid() const override { return m_RendererID != 0; }

		void SetData(const void* data, size_t size);

		virtual TextureFormat GetFormat() const override { return m_Specification.Format; }
		virtual TextureType   GetType()   const override { return TextureType::Texture2D; }

		virtual int GetWidth()  const override { return m_Specification.Width; }
		virtual int GetHeight() const override { return m_Specification.Height; }
		virtual std::pair<int, int> GetSize() const override { return{ m_Specification.Width, m_Specification.Height };}

		virtual void SetDebugName(const std::string& name) override;

	private:
		RendererID m_RendererID = 0;
		TextureSpecification  m_Specification;
		std::string m_DebugName;

		Texture2D(const TextureSpecification& spec, const std::filesystem::path& path = std::filesystem::path());

		KC_DISALLOW_MOVE(Texture2D);
		KC_DISALLOW_COPY(Texture2D);
	};

	class Texture2DArray : public Texture
	{
	public:
		virtual ~Texture2DArray();

		static Ref<Texture2DArray> Create(const TextureSpecification& spec, int LayerCount);
		static Ref<Texture2DArray> Create(const std::vector<std::filesystem::path>& paths, const TextureSpecification& spec = {});

		virtual void Bind(int slot = 0) const override;

		virtual RendererID GetRendererID() const override { return m_RendererID; }
		virtual bool IsValid() const override { return m_RendererID != 0; }

		void SetLayerData(const void* data, size_t size, int layer);

		virtual TextureFormat GetFormat() const override { return m_Specification.Format; }
		virtual TextureType   GetType()   const override { return TextureType::Texture2DArray; }
		int GetLayerCount() const { return m_LayerCount; }

		virtual int GetWidth()  const override { return m_Specification.Width; }
		virtual int GetHeight() const override { return m_Specification.Height; }
		virtual std::pair<int, int> GetSize() const override { return{ m_Specification.Width, m_Specification.Height }; }

		virtual void SetDebugName(const std::string& name) override;

	private:
		RendererID m_RendererID = 0;
		TextureSpecification  m_Specification;
		std::string m_DebugName;
		int m_LayerCount = 0;

		Texture2DArray(int LayerCount, const TextureSpecification& spec, const std::vector<std::filesystem::path>& paths);

		KC_DISALLOW_MOVE(Texture2DArray);
		KC_DISALLOW_COPY(Texture2DArray);
	};

	class TextureCube : public Texture
	{
	public:
		virtual ~TextureCube();

		static Ref<TextureCube> Create(const TextureSpecification& spec);
		static Ref<TextureCube> Create(const std::array<std::filesystem::path, 6>& facePaths, const TextureSpecification& spec = {});

		virtual void Bind(int slot = 0) const override;

		virtual RendererID GetRendererID() const override { return m_RendererID; }
		virtual bool IsValid() const override { return m_RendererID != 0; }

		void SetFaceData(const void* data, size_t size, CubeTextureFaces face);

		virtual TextureFormat GetFormat() const override { return m_Specification.Format; }
		virtual TextureType   GetType()   const override { return TextureType::TextureCube; }

		virtual int GetWidth()  const override { return m_Specification.Width; }
		virtual int GetHeight() const override { return m_Specification.Height; }
		virtual std::pair<int, int> GetSize() const override { return { m_Specification.Width, m_Specification.Height }; }

		virtual void SetDebugName(const std::string& name) override;

	private:
		TextureCube(const TextureSpecification& spec, const std::array<std::filesystem::path, 6>& facePaths = {});

		RendererID m_RendererID = 0;
		TextureSpecification m_Specification;
		std::string m_DebugName;

		KC_DISALLOW_MOVE(TextureCube);
		KC_DISALLOW_COPY(TextureCube);
	};


}