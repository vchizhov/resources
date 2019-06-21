#pragma once
#include "math/vec.hpp"

namespace HydrogenCG
{
	//! A structure containing intersection data
	struct Intersection
	{
		float dist;		//!< distance from the ray origin to the intersection
		vec3 pos;		//!< position of the intersection point
		vec3 normal;	//!< surface normal at the intersection
		vec3 color;		//!< surface color at the intersection

		Intersection() : dist(INFINITY), pos(0), normal(0), color(0) {}
		Intersection(float dist, const vec3& pos, const vec3& normal, const vec3& color)
			: dist(dist), pos(pos), normal(normal), color(color) {}

		operator bool() const 
		{
			return dist < INFINITY;
		}
	};

	// convenience function returning an invalid intersection
	Intersection noIntersection()
	{
		return Intersection();
	}

	// convenience function checking whether an intersection is valid
	bool valid(const Intersection& intersection)
	{
		return static_cast<bool>(intersection);
	}

}