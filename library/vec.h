#pragma once

/* GLM_GTX_transform is an experimental extension now, apparently */
#define GLM_ENABLE_EXPERIMENTAL

#define GLM_FORCE_RADIANS
#include <fmt/format.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace OpenApoc
{
#if (GLM_VERSION_MAJOR == 0 && GLM_VERSION_MINOR <= 9 && GLM_VERSION_PATCH <= 5)
#error GLM 0.9.6 or above is required
#else
template <typename T> using Vec3 = glm::vec<3, T, glm::highp>;

template <typename T> using Vec2 = glm::vec<2, T, glm::highp>;
#endif

static inline float mix(float a, float b, float factor) { return a * (1.0f - factor) + b * factor; }

template <typename T> static inline T clamp(const T &v, const T &min, const T &max)
{
	if (v < min)
		return min;
	if (v > max)
		return max;
	return v;
}

} // namespace OpenApoc

namespace glm
{
// Required for storing in std::map

template <typename T> bool operator<(const glm::vec<3, T, highp> &a, const vec<3, T, highp> &b)
{
	if (a.x < b.x)
		return true;
	else if (a.x > b.x)
		return false;
	else if (a.y < b.y)
		return true;
	else if (a.y > b.y)
		return false;
	else if (a.z < b.z)
		return true;
	else
		return false;
}

template <typename T> bool operator<(const glm::vec<2, T, highp> &a, const glm::vec<2, T, highp> &b)
{
	if (a.x < b.x)
		return true;
	else if (a.x > b.x)
		return false;
	else if (a.y < b.y)
		return true;
	else
		return false;
}
} // namespace glm

template <typename T> struct fmt::formatter<OpenApoc::Vec2<T>> : fmt::formatter<string_view>
{
	template <typename FormatContext>
	auto format(const OpenApoc::Vec2<T> &vec, FormatContext &ctx) const -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "\\{{1},{2}\\}", vec.x, vec.y);
	}
};
template <typename T> struct fmt::formatter<OpenApoc::Vec3<T>> : fmt::formatter<string_view>
{
	template <typename FormatContext>
	auto format(const OpenApoc::Vec3<T> &vec, FormatContext &ctx) const -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "\\{{1},{2},{3}\\}", vec.x, vec.y, vec.z);
	}
};