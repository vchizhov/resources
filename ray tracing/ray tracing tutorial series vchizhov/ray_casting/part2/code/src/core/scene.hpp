#pragma once
#include "sphere.hpp"
#include "light.hpp"
#include <vector>

namespace HydrogenCG
{
	
	/*
		\brief A class holding the scene geometry and light sources

		A function is provided to intersect all of the objects
		Additionally integrators accept a scene as an argument
	*/
	struct Scene
	{
		std::vector<Sphere> spheres;

		AmbientLight ambientLight;

		std::vector<PointLight> pointLights;
		std::vector<DirectionalLight> directionalLights;
		std::vector<ConeLight> coneLights;
		std::vector<CylinderLight> cylinderLights;

		/*
			\brief Returns the closest intersection in (minT,maxT), otherwise noIntersection()
		*/
		Intersection intersect(const Ray& ray, float minT, float maxT) const
		{
			Intersection result = noIntersection();
			result.dist = maxT;
			for (int i = 0; i < spheres.size(); ++i)
			{
				Intersection temp = spheres[i].intersect(ray, minT, result.dist);
				if (temp.dist < result.dist) result = temp;
			}
			if (result.dist < maxT)
				return result;
			else
				return noIntersection();
		}

		/*
			\brief Returns true if it intersects any object
		*/
		bool intersectAny( const Ray& ray, float minT, float maxT) const
		{
			for (int i = 0; i < spheres.size(); ++i)
			{
				if (spheres[i].intersectAny(ray, minT, maxT)) return true;
			}
			return false;
		}

		// convenience/brevity
		Intersection operator()(const Ray& ray, float minT = 0.0f, float maxT = INFINITY) const 
		{
			return intersect(ray, minT, maxT);
		}

	};

	// convenience function to add spheres to the scene
	void add(Scene& scene, const Sphere& sphere)
	{
		scene.spheres.push_back(sphere);
	}

	// convenience function to add point lights to the scene
	void add(Scene& scene, const PointLight& light)
	{
		scene.pointLights.push_back(light);
	}

	// convenience function to add directional lights to the scene
	void add(Scene& scene, const DirectionalLight& light)
	{
		scene.directionalLights.push_back(light);
	}

	// convenience function to add cone lights to the scene
	void add(Scene& scene, const ConeLight& light)
	{
		scene.coneLights.push_back(light);
	}

	// convenience function to add cylinder lights to the scene
	void add(Scene& scene, const CylinderLight& light)
	{
		scene.cylinderLights.push_back(light);
	}

	// free function variant, mirrors the shadertoy code
	Intersection intersect(const Scene& scene, const Ray& ray, float minT, float maxT)
	{
		return scene.intersect(ray, minT, maxT);
	}

	// free function variant, mirrors the shadertoy code
	bool intersectAny(const Scene& scene, const Ray& ray, float minT, float maxT)
	{
		return scene.intersectAny(ray, minT, maxT);
	}
}