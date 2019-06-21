#pragma once
#include "intersection.hpp"
#include "ray.hpp"
#include <limits>

namespace HydrogenCG
{
	/*
		\brief A sphere defined through its origin and radius
	*/
	struct Sphere
	{
		vec3 origin; 	//! sphere center
		float radius;	//! sphere radius

		/*
			Currently this represent the diffuse color of the sphere.
			For energy conservation it should be in [0,1] (this will become
			important for the first time in the ray tracing part).
		*/
		vec3 color;		//! the color (albedo) of the sphere


		Sphere(const vec3& origin = vec3(0), float radius = 1.0f, const vec3& color = vec3(1))
			: origin(origin), radius(radius), color(color) {}

		// returns the normal at point p on the sphere
		vec3 normal(const vec3& p) const
		{
			return (p - origin) / radius;
		}

		// Evaluates whether the ray intersects the sphere within the ray segment (minT, maxT)
		// returns INFINITY if there is no intersection, so that a check for intersection can be performed as intersect(...)<INFINITY
		// otherwise returns the distance from the ray origin to the closest intersection
		Intersection intersect(const Ray& ray, float minT, float maxT) const
		{
			/*
				|ray(t) - pos| == r <->
				|ray(t) - pos|^2 == r^2 <->
				<ray(t)-pos,ray(t)-pos> == r^2 <->
				<ray.o-pos + t * ray.d, ray.o-pos + t * ray.d> == r^2 <->
				<ray.d,ray.d> * t^2 - 2 * <ray.d, pos - ray.o> * t + <pos-ray.o,pos-ray.o> - r^2 == 0
				A = <ray.d,ray.d>, but |ray.d|==1 if the direction is normalized -> A = 1
				B = <ray.d, pos - ray.o>
				C = <pos-ray.o,pos-ray.o> - r^2

				D = B^2 - C
				D<0  -> no intersection
				D==0 -> grazing intersection
				D>0  -> 2 intersections

				One can ignore grazing intersections since they are actually numerical error.

				D>0 -> sqrtD = sqrt(D)
				t_1 = B - D
				t_2 = B + D

				If maxT>t_1>minT -> intersection at ray(t_1)
				Else If maxT>t_2>min_t -> intersection at ray(t_2)
				Else -> no intersection
			*/
			vec3 oPos = origin - ray.o;
			float b = dot(ray.d, oPos);
			float c = dot(oPos, oPos) - radius * radius;
			float d = b * b - c;

			// If the discriminant is 0 or negative -> no (actual) intersection
			if (d <= 0.0) return noIntersection();

			float sqrtD = sqrt(d);
			float t1 = b - sqrtD; // closer intersection
			// is it within the defined ray segment by (minT,maxT) ?
			if (t1 > minT && t1 < maxT)
			{
				vec3 pos = at(ray, t1);
				return Intersection(t1, pos, normal(pos), color);
			}

			float t2 = b + sqrtD; // farther intersection
			// is it within the defined ray segment by (minT,maxT) ?
			if (t2 > minT && t2 < maxT)
			{
				vec3 pos = at(ray, t2);
				return Intersection(t2, pos, normal(pos), color);
			}

			return noIntersection();
		}

		//! returns true only if there's an intersection without computing normals or extra intersection info
		bool intersectAny(const Ray& ray, float minT, float maxT) const
		{
			// the code is almost the same
			vec3 oPos = origin - ray.o;
			float b = dot(ray.d, oPos);
			float c = dot(oPos, oPos) - radius * radius;
			float d = b * b - c;

			float sqrtD = sqrt(d);
			float t1 = b - sqrtD;
			float t2 = b + sqrtD;
			return (d > 0.0) && ((t1 > minT && t1 < maxT) || (t2 > minT && t2 < maxT));
		}

	};

	// free function variant, mirrors the shadertoy code
	vec3 normal(const Sphere& s, const vec3& p)
	{
		return s.normal(p);
	}

	// free function variant, mirrors the shadertoy code
	Intersection intersect(const Sphere& s, const Ray& ray, float minT, float maxT)
	{
		return s.intersect(ray, minT, maxT);
	}

	// free function variant, mirrors the shadertoy code
	bool intersectAny(const Sphere& s, const Ray& ray, float minT, float maxT)
	{
		return s.intersectAny(ray, minT, maxT);
	}
}