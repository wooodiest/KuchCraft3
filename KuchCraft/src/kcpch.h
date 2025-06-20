#pragma once

#include <iostream>
#include <iomanip>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <random>
#include <filesystem>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <tuple>

#include <chrono>
#include <fstream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <queue>

#include <type_traits>
#include <typeinfo>

#include <cassert>
#include <exception>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <limits>

#include <optional>
#include <variant>
#include <any>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/bitfield.hpp>
#include <glm/gtc/round.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/integer.hpp>

#include "Core/Base.h"
#include "Core/Assert.h"
#include "Core/Log.h"
#include "Core/Random.h"
#include "Core/FastRandom.h"
#include "Core/KeyCodes.h"
#include "Core/Input.h"
#include "Core/Event.h"
#include "Core/Config.h"
#include "Core/Timestep.h"