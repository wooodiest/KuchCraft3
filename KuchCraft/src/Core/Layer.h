#pragma once

#include "Core/Timestep.h"
#include "Core/ApplicationEvent.h"

#include <string>

namespace KuchCraft {

	enum class LayerType : uint8_t
	{
		None	= 0,
		Game    = 1,
		UI      = 2,
		Overlay = 3,
		Debug   = 4,
	};

	enum class LayerState : uint8_t
	{
		Active,       // Fully active: receives updates, events, and is rendered
		Visible,      // Rendered only, no updates or events
		NotVisible,   // Updates and receives events, but not rendered
		Disabled      // Completely inactive: no rendering, updates, or events
	};

	class Layer
	{
	public:
		Layer(const std::string& name, LayerType type)
			: m_Name(name), m_Type(type) {}
		virtual ~Layer() = default;
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnTick(const Timestep ts) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
		virtual void OnApplicationEvent(ApplicationEvent& e) {}

		inline const std::string& GetName() const { return m_Name; }
		inline LayerType          GetType() const { return m_Type; }

		void SetState(LayerState state) { m_State = state; }
		LayerState GetState() const { return m_State; }

		bool ShouldRender() const { return m_State == LayerState::Active || m_State == LayerState::Visible; }
		bool ShouldUpdate() const { return m_State == LayerState::Active || m_State == LayerState::NotVisible; }
		bool ShouldHandleEvents() const { return m_State == LayerState::Active || m_State == LayerState::NotVisible; }

		void  SetZIndex(float z) { m_ZIndex = z; }
		float GetZIndex() const  { return m_ZIndex; }

	protected:
		std::string m_Name;
		LayerState  m_State  = LayerState::Active;
		LayerType   m_Type   = LayerType::None;

		float m_ZIndex = 0.0f;

	};
}