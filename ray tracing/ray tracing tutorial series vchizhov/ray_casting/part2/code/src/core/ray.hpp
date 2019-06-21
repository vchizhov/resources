#pragma once
#include "math/vec.hpp"


namespace HydrogenCG
{
	/*!
		\brief A basic ray class.

		Note that it does not perform any checks as to whether a point is actually on the ray (t>=0).
	*/
	struct Ray
	{
		vec3 o;		//!< The origin of the ray
		vec3 d;		//!< The direction of the ray, ideally kept unit length

		Ray() {}
		Ray(const vec3& o, const vec3& d) : o(o), d(d) {}

		/*Ray& operator=(const Ray& ray)
		{
			o = ray.o;
			d = ray.d;
		}*/

		//! Convenience method returning the point on the ray at distance t from its origin
		inline vec3 at(float t) const { return o + t * d; }
		//! Convenience method returning the point on the ray at distance t from its origin
		inline vec3 operator()(float t) const { return at(t); }
	};

	// free function variant, mirrors the shadertoy code
	vec3 at(const Ray& ray, float t)
	{
		return ray.at(t);
	}
}