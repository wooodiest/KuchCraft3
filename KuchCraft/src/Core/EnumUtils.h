#pragma once

#include <string_view>
#include <optional>
#include <type_traits>
#include <stdexcept>
#include <magic_enum.hpp>

namespace KuchCraft {

    template <typename Enum>
    constexpr std::optional<Enum> FromString(std::string_view name)
    {
        static_assert(std::is_enum_v<Enum>, "FromString can only be used with enum types");
        return magic_enum::enum_cast<Enum>(name);
    }

    template <typename Enum>
    constexpr std::string_view ToString(Enum e)
    {
        static_assert(std::is_enum_v<Enum>, "ToString can only be used with enum types");
        if (auto name = magic_enum::enum_name(e); !name.empty()) 
            return name;
        
        return "Unknown";
    }

	KC_TODO("Maybe add more json utility functions for enums");
}