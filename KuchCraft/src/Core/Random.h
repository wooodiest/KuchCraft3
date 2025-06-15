#pragma once

#include "Core/Base.h"

#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

namespace KuchCraft {

	class Random
	{
	public:
		/// Initializes the random number generator for the current thread.
		/// This method seeds the random engine with a random value obtained from a random device.
		static void Init()
		{
			s_RandomEngine.seed(std::random_device()());
		}

	public:
		static inline [[nodiscard]] uint32_t UInt()
		{
			return s_Distribution(s_RandomEngine);
		}

		static inline [[nodiscard]] uint32_t UInt(uint32_t min, uint32_t max)
		{
			return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
		}
		static inline [[nodiscard]] int Int()
		{
			return static_cast<int>(UInt());
		}

		static inline [[nodiscard]] int Int(int min, int max)
		{
			return static_cast<int>(UInt(static_cast<uint32_t>(min), static_cast<uint32_t>(max)));
		}

		static inline [[nodiscard]] float Float()
		{
			return static_cast<float>(s_Distribution(s_RandomEngine)) / static_cast<float>(std::numeric_limits<uint32_t>::max());
		}

		static inline [[nodiscard]] float Float(float min, float max)
		{
			return min + (max - min) * Float();
		}

		static inline [[nodiscard]] glm::vec3 ColorRGB()
		{
			return glm::vec3(Float(0.0f, 1.0f), Float(0.0f, 1.0f), Float(0.0f, 1.0f));
		}

		static inline [[nodiscard]] glm::vec4 ColorRGBA()
		{
			return glm::vec4(Float(0.0f, 1.0f), Float(0.0f, 1.0f), Float(0.0f, 1.0f), 1.0f);
		}

		static inline [[nodiscard]] glm::vec3 Position(const glm::vec3& min, const glm::vec3& max)
		{
			return glm::vec3(Float(min.x, max.x), Float(min.y, max.y), Float(min.z, max.z));
		}

		static inline [[nodiscard]] float Angle()
		{
			return Float(0.0f, glm::two_pi<float>());
		}
		static inline [[nodiscard]] glm::vec3 Direction()
		{
			float theta = Angle();
			float phi = glm::acos(2.0f * Float(0.0f, 1.0f) - 1.0f);
			return glm::vec3(glm::sin(phi) * glm::cos(theta), glm::sin(phi) * glm::sin(theta), glm::cos(phi));
		}

		static inline [[nodiscard]] glm::quat Quaternion()
		{
			float u1 = Float(0.0f, 1.0f);
			float u2 = Float(0.0f, 1.0f);
			float u3 = Float(0.0f, 1.0f);
			float sqrt1u1 = sqrt(1 - u1);
			return glm::quat(sqrt1u1 * sin(2 * glm::pi<float>() * u2),
				sqrt1u1 * cos(2 * glm::pi<float>() * u2),
				sqrt(u1) * sin(2 * glm::pi<float>() * u3),
				sqrt(u1) * cos(2 * glm::pi<float>() * u3));
		}

		static inline [[nodiscard]] glm::vec3 NormalizedVector()
		{
			return glm::normalize(Vec3());
		}

		static inline [[nodiscard]] bool Bool()
		{
			return UInt(0, 1) == 1;
		}

		static inline [[nodiscard]] glm::vec2 Vec2()
		{
			return glm::vec2(Float(), Float());
		}

		static inline [[nodiscard]] glm::vec3 Vec3()
		{
			return glm::vec3(Float(), Float(), Float());
		}

		static inline [[nodiscard]] glm::vec4 Vec4()
		{
			return glm::vec4(Float(), Float(), Float(), Float());
		}

		static inline [[nodiscard]] glm::vec2 Vec2(float min, float max)
		{
			return glm::vec2(Float(min, max), Float(min, max));
		}

		static inline [[nodiscard]] glm::vec3 Vec3(float min, float max)
		{
			return glm::vec3(Float(min, max), Float(min, max), Float(min, max));
		}

		static inline [[nodiscard]] glm::vec4 Vec4(float min, float max)
		{
			return glm::vec4(Float(min, max), Float(min, max), Float(min, max), Float(min, max));
		}

		static inline [[nodiscard]] glm::ivec2 IVec2()
		{
			return glm::ivec2(Int(), Int());
		}

		static inline [[nodiscard]] glm::ivec3 IVec3()
		{
			return glm::ivec3(Int(), Int(), Int());
		}

		static inline [[nodiscard]] glm::ivec4 IVec4()
		{
			return glm::ivec4(Int(), Int(), Int(), Int());
		}

		static inline [[nodiscard]] glm::ivec2 IVec2(int min, int max)
		{
			return glm::ivec2(Int(min, max), Int(min, max));
		}

		static inline [[nodiscard]] glm::ivec3 IVec3(int min, int max)
		{
			return glm::ivec3(Int(min, max), Int(min, max), Int(min, max));
		}

		static inline [[nodiscard]] glm::ivec4 IVec4(int min, int max)
		{
			return glm::ivec4(Int(min, max), Int(min, max), Int(min, max), Int(min, max));
		}

	private:
		/// Thread-local random number generator for each thread
		static thread_local inline std::mt19937 s_RandomEngine;

		/// Thread-local distribution used for generating random numbers
		static thread_local inline std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;

	private:
		KC_DISALLOW_COPY(Random);
		KC_DISALLOW_MOVE(Random);
		KC_DISALLOW_CREATE(Random);
	};

}