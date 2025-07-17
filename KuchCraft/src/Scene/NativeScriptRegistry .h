#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include "Scene/Components.h"

namespace KuchCraft {

	class NativeScriptRegistry
	{
	public:
		using BinderFunc = std::function<void(NativeScriptComponent&)>;

		static auto& GetMap() 
		{
			static std::unordered_map<std::string, BinderFunc> registry;
			return registry;
		}

		static void Register(const std::string& name, BinderFunc binder)
		{
			GetMap()[name] = std::move(binder);
		}

		static bool BindScript(const std::string& name, NativeScriptComponent& nsc)
		{
			if (auto it = GetMap().find(name); it != GetMap().end()) {
				it->second(nsc);
				return true;
			}
			return false;
		}

		static std::vector<std::string> GetRegisteredNames()
		{
			std::vector<std::string> names;
			for (auto& [name, _] : GetMap())
				names.push_back(name);
			return names;
		}
	};

	#define REGISTER_NATIVE_SCRIPT(TYPE) \
	struct TYPE##ScriptRegistrar { \
	    inline static const bool registered = [] { \
	        KuchCraft::NativeScriptRegistry::Register(#TYPE, [](KuchCraft::NativeScriptComponent& nsc) { \
	            nsc.Bind<TYPE>(#TYPE); \
	        }); \
	        return true; \
	    }(); \
	};

	
}

