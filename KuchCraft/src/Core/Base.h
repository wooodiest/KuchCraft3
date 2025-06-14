#pragma once

namespace KuchCraft {
	void InitializeCore();
	void ShutdownCore();
}

#define KC_VERSION "2025.0.0"

#if defined(KC_DEBUG)
	#define KC_BUILD_CONFIG_NAME "Debug"
#elif defined(KC_RELEASE)
	#define KC_BUILD_CONFIG_NAME "Release"
#elif defined(KC_DIST)
	#define KC_BUILD_CONFIG_NAME "Dist"
#else
	#error Undefined configuration?
#endif

#if defined(KC_PLATFORM_WINDOWS)
	#define KC_BUILD_PLATFORM_NAME "Windows x64"
#else
	#define KC_BUILD_PLATFORM_NAME "Unknown"
#endif

#define KC_VERSION_LONG "KuchCraft " KC_VERSION " (" KC_BUILD_PLATFORM_NAME " " KC_BUILD_CONFIG_NAME ")"

#if !defined(KC_PLATFORM_WINDOWS)
	#error Unknown or unsupported platform.
#endif

#if defined(__GNUC__)
	#if defined(__clang__)
		#define KC_COMPILER_CLANG
	#else
		#define KC_COMPILER_GCC
	#endif
#elif defined(_MSC_VER)
	#define KC_COMPILER_MSVC
#endif

#ifdef KC_COMPILER_MSVC
	#define KC_FORCE_INLINE __forceinline
	#define KC_EXPLICIT_STATIC static
#elif defined(__GNUC__)
	#define KC_FORCE_INLINE __attribute__((always_inline)) inline
	#define KC_EXPLICIT_STATIC
#else
	#define KC_FORCE_INLINE inline
	#define KC_EXPLICIT_STATIC
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
	#define KC_TODO(msg) __pragma(message("[TODO] " __FILE__ "(" KC_STRINGIFY(__LINE__) "): " msg))
#else
	#define KC_TODO(msg)
#endif

#define KC_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define KC_BIND_STATIC_EVENT_FN(fn) [](auto&&... args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }

#define KC_EXPAND_MACRO(x) x

#define KC_UNUSED(x) (void)(x)

#define KC_STRINGIFY_INTERNAL(x) #x
#define KC_STRINGIFY(x) KC_STRINGIFY_INTERNAL(x)

#define BIT(x) (1u << x)

#define KC_ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))

#define KC_ENUM_FLAG_OPERATORS(Enum) \
	inline Enum operator|(Enum a, Enum b) { return static_cast<Enum>(static_cast<int>(a) | static_cast<int>(b)); } \
	inline Enum operator&(Enum a, Enum b) { return static_cast<Enum>(static_cast<int>(a) & static_cast<int>(b)); } \
	inline Enum& operator|=(Enum& a, Enum b) { return a = a | b; } \
	inline Enum& operator&=(Enum& a, Enum b) { return a = a & b; }

#define KC_CONCAT_INTERNAL(a, b) a##b
#define KC_CONCAT(a, b) KC_CONCAT_INTERNAL(a, b)

#define KC_DISALLOW_COPY(Type) \
	Type(const Type&) = delete; \
	Type& operator=(const Type&) = delete

#define KC_DISALLOW_MOVE(Type) \
	Type(Type&&) = delete; \
	Type& operator=(Type&&) = delete

#define KC_DISALLOW_CREATE(Type) \
	Type() = delete; \
	~Type() = delete
