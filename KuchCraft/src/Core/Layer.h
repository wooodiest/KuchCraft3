#pragma once

#include "Core/Timestep.h"
#include "Core/Event.h"

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
		Active, 
		Visible,
		Disabled
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
		virtual void OnEvent(Event& e) {}

		inline const std::string& GetName() const { return m_Name; }
		inline LayerType          GetType() const { return m_Type; }

		void SetState(LayerState state) { m_State = state; }
		LayerState GetState() const { return m_State; }

		bool IsVisible() const { return m_State == LayerState::Active || m_State == LayerState::Visible; }
		bool IsActive()  const { return m_State == LayerState::Active; }

	protected:
		std::string m_Name;
		LayerState  m_State  = LayerState::Active;
		LayerType   m_Type   = LayerType::None;

	};
}