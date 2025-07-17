#pragma once

#include <nlohmann_json/json.hpp>
#include <glm/glm.hpp>
#include "Core/UUID.h"

namespace nlohmann {

	// glm::vec2
	template <>
	struct adl_serializer<glm::vec2> {
		static void to_json(json& j, const glm::vec2& v) {
			j = json::array({ v.x, v.y });
		}
		static void from_json(const json& j, glm::vec2& v) {
			v.x = j.at(0).get<float>();
			v.y = j.at(1).get<float>();
		}
	};

	// glm::vec3
	template <>
	struct adl_serializer<glm::vec3> {
		static void to_json(json& j, const glm::vec3& v) {
			j = json::array({ v.x, v.y, v.z });
		}
		static void from_json(const json& j, glm::vec3& v) {
			v.x = j.at(0).get<float>();
			v.y = j.at(1).get<float>();
			v.z = j.at(2).get<float>();
		}
	};

	// glm::vec4
	template <>
	struct adl_serializer<glm::vec4> {
		static void to_json(json& j, const glm::vec4& v) {
			j = json::array({ v.x, v.y, v.z, v.w });
		}
		static void from_json(const json& j, glm::vec4& v) {
			v.x = j.at(0).get<float>();
			v.y = j.at(1).get<float>();
			v.z = j.at(2).get<float>();
			v.w = j.at(3).get<float>();
		}
	};

	// KuchCraft::UUID
	template <>
	struct adl_serializer<KuchCraft::UUID> {
		static void to_json(json& j, const KuchCraft::UUID& uuid) {
			j = static_cast<uint64_t>(uuid);
		}
		static void from_json(const json& j, KuchCraft::UUID& uuid) {
			uuid = KuchCraft::UUID(j.get<uint64_t>());
		}
	};

}
