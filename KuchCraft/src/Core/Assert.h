#pragma once

#include "Core/Base.h"
#include "Core/Log.h"

#if defined(KC_PLATFORM_WINDOWS)
	#include <intrin.h>
	#define KC_DEBUGBREAK() __debugbreak()
#else
	#include <csignal>
	#define KC_DEBUGBREAK() raise(SIGTRAP)
#endif

#if defined(KC_DEBUG) || defined(KC_RELEASE)
	#define KC_ENABLE_ASSERTS
#endif

#define KC_INTERNAL_ASSERT_IMPL(type, check, ...) \
	do { \
		if (!(check)) { \
			type##ERROR(__VA_ARGS__); \
			KC_DEBUGBREAK(); \
		} \
	} while (0)


#define KC_INTERNAL_ASSERT_NO_MSG_IMPL(type, check) \
	do { \
		if (!(check)) { \
			type##ERROR("Assertion failed: {}", #check); \
			KC_DEBUGBREAK(); \
		} \
	} while (0)

#ifdef  KC_ENABLE_ASSERTS
	#define KC_CORE_ASSERT(condition, ...)    KC_INTERNAL_ASSERT_IMPL(KC_CORE_, condition, __VA_ARGS__)
	#define KC_CORE_ASSERT_NO_MSG(condition)  KC_INTERNAL_ASSERT_NO_MSG_IMPL(KC_CORE_, condition)
	
	#define KC_ASSERT(condition, ...)         KC_INTERNAL_ASSERT_IMPL(KC_, condition, __VA_ARGS__)
	#define KC_ASSERT_NO_MSG(condition)       KC_INTERNAL_ASSERT_NO_MSG_IMPL(KC_, condition)
#else
	#define KC_CORE_ASSERT(condition, ...)    
	#define KC_CORE_ASSERT_NO_MSG(condition)  
	
	#define KC_ASSERT(condition, ...)         
	#define KC_ASSERT_NO_MSG(condition)       
#endif

#define KC_VERIFY(condition, ...) KC_INTERNAL_ASSERT_IMPL(KC_, condition, __VA_ARGS__)
#define KC_VERIFY_NO_MSG(condition) KC_INTERNAL_ASSERT_NO_MSG_IMPL(KC_, condition)

#define KC_NOT_IMPLEMENTED() \
	KC_CORE_ERROR("Not implemented: {}:{}:{}", __FILE__, __LINE__, __FUNCTION__); \
	KC_DEBUGBREAK()

