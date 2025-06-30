#include "kcpch.h"
#include "Core/LayerStack.h"

namespace KuchCraft {

	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		Clear();
	}

	void LayerStack::Clear()
	{
		for (const auto& layer : m_Layers)
			layer->OnDetach();

		m_Layers.clear();
	}

	void LayerStack::AddLayer(Ref<Layer> layer)
	{
		KC_CORE_ASSERT(layer, "Layer cannot be null");

		if (Size() >= s_MaxLayers)
		{
			KC_CORE_ERROR("LayerStack has maximum layer size");
			return;
		}

		const std::string& name = layer->GetName();
		auto named = m_NamedLayers.find(name);
		if (named != m_NamedLayers.end())
		{
			KC_CORE_WARN("Layer with name '{}' already exists. Overwriting.", name);
			ReplaceLayer(named->second, layer);
			return;
		}

		int priority = static_cast<int>(layer->GetType());
		auto it = std::find_if(m_Layers.begin(), m_Layers.end(), [&](const Ref<Layer>& existing) {
			return GetPriority(existing->GetType()) > priority;
		});

		m_Layers.insert(it, layer);
		m_NamedLayers[name] = layer;

		layer->OnAttach();
	}

	void LayerStack::RemoveLayer(Ref<Layer> layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			(*it)->OnDetach();
			m_NamedLayers.erase(layer->GetName());
			m_Layers.erase(it);
		}
		else
		{
			KC_CORE_WARN("Layer not found in stack");
		}
	}

	void LayerStack::ReplaceLayer(Ref<Layer> oldLayer, Ref<Layer> newLayer)
	{
		KC_CORE_ASSERT(oldLayer && newLayer, "Layers cannot be null");

		auto it = std::find(m_Layers.begin(), m_Layers.end(), oldLayer);
		if (it != m_Layers.end())
		{
			const std::string name = oldLayer->GetName();

			oldLayer->OnDetach();
			*it = newLayer;
			newLayer->OnAttach();

			m_NamedLayers[name] = newLayer; 
		}
		else
		{
			KC_CORE_WARN("Old layer not found, adding new layer instead");
			AddLayer(newLayer);
		}
	}

	Ref<Layer> LayerStack::operator[](size_t index)
	{
		KC_CORE_ASSERT(index < m_Layers.size(), "Layer index out of bounds");
		return m_Layers[index];
	}

	Ref<Layer> LayerStack::operator[](const std::string& name)
	{
		auto it = m_NamedLayers.find(name);
		if (it != m_NamedLayers.end())
			return it->second;

		KC_CORE_ERROR("Layer with name '{}' not found", name);
		return nullptr;
	}

	bool LayerStack::HasLayer(const Ref<Layer>& layer) const
	{
		return std::find(m_Layers.begin(), m_Layers.end(), layer) != m_Layers.end();
	}

	bool LayerStack::HasLayer(const std::string& name) const
	{
		return m_NamedLayers.contains(name);
	}

	Ref<Layer> LayerStack::GetLayerByName(const std::string& name) const
	{
		for (const auto& layer : m_Layers)
		{
			if (layer->GetName() == name)
				return layer;
		}

		KC_CORE_WARN("Layer with name '{0}' not found in stack", name);
		return nullptr;
	}

	size_t  LayerStack::GetPriority(LayerType type) const
	{
		return static_cast<size_t>(type);
	}

}
