#pragma once

#include "Core/Layer.h"
#include "Core/Base.h"

namespace KuchCraft {

	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void Clear();

		void AddLayer    (Ref<Layer> layer);
		void RemoveLayer (Ref<Layer> layer);
		void ReplaceLayer(Ref<Layer> oldLayer, Ref<Layer> newLayer);

		Ref<Layer> GetLayerByName(const std::string& name) const;
		Ref<Layer> operator[](size_t index);
		Ref<Layer> operator[](const std::string& name);

		bool HasLayer(const Ref<Layer>& layer) const;
		bool HasLayer(const std::string& name) const;

		size_t Size() const { return m_Layers.size(); }
		std::vector<Ref<Layer>>::iterator         begin()  { return m_Layers.begin();  }
		std::vector<Ref<Layer>>::iterator         end()    { return m_Layers.end();    }
		std::vector<Ref<Layer>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<Ref<Layer>>::reverse_iterator rend()   { return m_Layers.rend();   }

		inline static int s_MaxLayers = 1000;

	private:
		size_t GetPriority(LayerType type) const;

	private:
		std::vector<Ref<Layer>> m_Layers;
		std::unordered_map<std::string, Ref<Layer>> m_NamedLayers;

	};

}