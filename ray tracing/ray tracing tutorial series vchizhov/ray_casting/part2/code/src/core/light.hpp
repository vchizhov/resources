#pragma once
#include "math/vec.hpp"

namespace HydrogenCG
{

	/*
		\brief Holds sampled data from a light source, used for shading computations
	*/
	struct LightSample
	{
		vec3 radiance;			//!< radiance traveling toward the point being shaded

		// used for Lambert's cosine term and brdf evaluation
		vec3 direction;			//!< direction from the point being shaded to the light source sample

		// used for shadow rays tests
		float distanceToLight;	//!< distance to the light source sample along the direction

		LightSample() : radiance(0), direction(0), distanceToLight(INFINITY) {}
		LightSample(const vec3& radiance, const vec3& direction, float distanceToLight)
			: radiance(radiance), direction(direction), distanceToLight(distanceToLight) {}
	};

	/*
		\brief A light that emits constant light (radiance) from every point in the scene in every direction.

		The ambient light aims to compensate for the lack of indirect illumination in local
		rendering methods (similar to rasterization or the two diffuse integrators we have here).
		For example it can make very dark regions of the image brighter, however, it makes ALL
		shaded points brighter, so it is simply like increasing the brightness.
	*/
	struct AmbientLight
	{
		vec3 radiance;	//!< The color and strength of the light

		AmbientLight(const vec3& radiance = vec3(0)) : radiance(radiance) {}


		/*
			\brief Derives data from the light source and the position of the point to be shaded necessary for shading

			pos is the position of the point for which we called sampleRadiance
			in order to shade it
		*/
		LightSample sampleRadiance(const vec3& pos) const
		{
			return LightSample(radiance, vec3(0), 0.0);
		}
	};

	/*
		\brief A light with no area, defined only though its position and (isotropic) intensity

		The point light aims to model very small light sources. Small light sources still
		have area, which requires sampling. On the other hand, a point light has no area
		so it allows for an efficient implementation since no sampling is required.
		Obviously there's a trade-off between accuracy and efficiency, as point lights
		are not physical and do not produce soft shadows - infinite energy is concentrated
		in a single point. However point lights obey the inverse-square law, so the light energy
		diminishes with the inverse square of the distance from the light source.

		Note: isotropic intensity means that the the light emits equal energy in all directions.
		In comparison a textured light would be anisotropic.
	*/
	struct PointLight
	{
		vec3 intensity;		//!< The color and strength of the light
		vec3 origin;		//!< The position of the light

		PointLight() : intensity(0), origin(0) {}
		PointLight(const vec3& intensity, const vec3& origin) : intensity(intensity), origin(origin) {}

		LightSample sampleRadiance(const vec3& pos) const
		{
			LightSample lightSample;

			// the vector pointing from the intersection to the light source
			vec3 posToLight = origin - pos;
			/*
				we also need to normalize this vector and compute the square of its
				distance to account for the inverse-square light falloff
			*/
			float distanceToLight = length(posToLight);
			posToLight /= distanceToLight;
			float squaredDistanceToLight = distanceToLight * distanceToLight;

			// the negated direction from which the light comes from
			lightSample.direction = posToLight;
			// the radiance from the light along that direction
			lightSample.radiance = intensity / squaredDistanceToLight;

			// return the distance to the light for shadow ray tests
			lightSample.distanceToLight = distanceToLight;

			return lightSample;
		}
	};

	/*
		\brief An infinitely distant light source emitting in a single direction

		It aims to model far away light sources, so that the rays arriving at the scene are close
		to parallel. Examples of such emitters are the sun or the moon (as a reflector).
		This light source is also not physical since it usually spans an infinite area (this can be
		changed) and each point emits light only in a single direction. A less intuitive and more
		accurate analogy would be an array of (infinitely many infinitely small) lasers oriented in
		the exact same direction.

		Note that the directional light we implement is homogeneous with regards to position - it
		emits the exact same amount of light from all of its points.
		In comparison a textured directional light would be non-homogeneous.
	*/
	struct DirectionalLight
	{
		vec3 radiosity;		//!< The strength and color of the light
		vec3 direction;		//!< The direction of the light

		DirectionalLight() : radiosity(0), direction(0) {}
		DirectionalLight(const vec3& radiosity, const vec3& direction)
			: radiosity(radiosity), direction(direction) {}

		LightSample sampleRadiance(const vec3& pos) const
		{
			LightSample lightSample;

			lightSample.direction = -direction;
			lightSample.radiance = radiosity;

			/*
				we consider the directional light to be situated at an infinite distance
				from the point being shaded
			*/
			lightSample.distanceToLight = INFINITY;

			return lightSample;
		}
	};

	/*
		\brief An extension of the (isotropic) point light defined above

		We relax the isotropy of the point light defined above and
		make it emit light only in a cone of angle phi around the direction it's
		pointing at. The attenuation from the center to the outward angles is
		modeled through a smoothstep and we apply a concentric texture.
	*/
	struct ConeLight
	{
		vec3 intensity; //!< The color and strength of the light
		vec3 origin;	//!< The position of the light
		vec3 direction;	//!< The direction of the light (the height vector of the cone)
		float cosPhi;	//!< The cosine of the maximum angle beyond which it emits no light

		ConeLight() : intensity(0), origin(0), direction(0), cosPhi(0) {}
		ConeLight(const vec3& intensity, const vec3& origin, const vec3& direction, float cosPhi)
			: intensity(intensity), origin(origin), direction(direction), cosPhi(cosPhi) {}

		LightSample sampleRadiance(const vec3& pos) const
		{
			// sample the cone light like you would a point light
			PointLight pointLight = PointLight(intensity, origin);
			LightSample lightSample = pointLight.sampleRadiance(pos);

			// apply attenuation based on angle:
			float cosLightCone = -dot(lightSample.direction, direction);
			/*
				use a smoothstep to attenuate based on the angle from the direction vector
				beyond the user defined phi angle there should be no contribution
			*/
			float att = smoothstep(cosPhi, 1.0f, cosLightCone);

			// add a concentric texture to the light based on the angle
			float tex = 0.5f + 0.5f*sin(200.0f * cosLightCone);
			lightSample.radiance *= att * tex;


			return lightSample;
		}
	};

	

	/*
		\brief An extension of the (homogeneous) directional light defined above

		We relax the homogeneity of the directional light defined above and introduce
		variable emissivity over its surface. We add a smoothstep attenuation from the
		center based on distance so that it is not infinite anymore, and the light rays
		form a cylinder. We additionally add a concentric texture.
	*/
	struct CylinderLight
	{
		vec3 radiosity;		//!< The strength and color of the light
		vec3 origin;		//!< The center of the light (it's actually at infinity, so this is used as a direction to infinity)
		vec3 direction;		//!< The direction of the light
		float radius;		//!< The radius of the light cylinder

		CylinderLight() {}
		CylinderLight(const vec3& radiosity, const vec3& origin, const vec3& direction, float radius)
			: radiosity(radiosity), origin(origin), direction(direction), radius(radius) {}

		LightSample sampleRadiance(const vec3& pos) const
		{
			// sample the cylinder light like you would a directional light
			DirectionalLight directionalLight = DirectionalLight(radiosity, direction);
			LightSample lightSample = directionalLight.sampleRadiance(pos);

			// compute the projection of the current point on the light plane
			vec3 lightToPos = pos - origin;
			vec3 projOnLightDir = dot(lightToPos, direction) * direction;
			vec3 projOnLightPlane = lightToPos - projOnLightDir;

			// use the magnitude of the projected vector to create varying radiance based on position:
			float magnitudeProjectedVector = length(projOnLightPlane);

			// use a smoothstep to attenuate based on the distance from the 
			// "center" point on the light plane (becomes 0 outside of the radius)
			float att = smoothstep(0.0f, 1.0f, radius - magnitudeProjectedVector);

			// add a concentric texture to the light
			float tex = 0.5f + 0.5f*sin(15.0f * magnitudeProjectedVector);

			lightSample.radiance *= tex * att;

			return lightSample;
		}
	};

	// free function variant, mirrors the shadertoy code
	template<typename LightType>
	LightSample sampleRadiance(const LightType& light, const vec3& pos)
	{
		return light.sampleRadiance(pos);
	}

}