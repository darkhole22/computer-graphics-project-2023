#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace vulture {

using i8 = char;
using i16 = short;
using i32 = int;
using i64 = long long int;

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long int;

using f32 = float;
using f64 = double;

// @brief Has 'true' value if the type in the template is a signed type.
template<class T> constexpr bool isSignedType = static_cast<T>(-1) < static_cast<T>(0);

static_assert(sizeof(i8 ) == 1 && isSignedType<i8 >, "Type i8 expected to be 1 byte in size and signed.");
static_assert(sizeof(i16) == 2 && isSignedType<i16>, "Type i16 expected to be 2 byte in size and signed.");
static_assert(sizeof(i32) == 4 && isSignedType<i32>, "Type i32 expected to be 4 byte in size and signed.");
static_assert(sizeof(i64) == 8 && isSignedType<i64>, "Type i64 expected to be 8 byte in size and signed.");

static_assert(sizeof(u8 ) == 1 && !isSignedType<u8 >, "Type u8 expected to be 1 byte in size and unsigned.");
static_assert(sizeof(u16) == 2 && !isSignedType<u16>, "Type u16 expected to be 2 byte in size and unsigned.");
static_assert(sizeof(u32) == 4 && !isSignedType<u32>, "Type u32 expected to be 4 byte in size and unsigned.");
static_assert(sizeof(u64) == 8 && !isSignedType<u64>, "Type u64 expected to be 8 byte in size and unsigned.");

static_assert(sizeof(f32) == 4, "Type f32 expected to be 4 byte in size.");
static_assert(sizeof(f64) == 8, "Type f64 expected to be 8 byte in size.");

} // namespace vulture
