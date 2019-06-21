#pragma once
#include "scene.hpp"
#include "image.hpp"
#include "camera.hpp"

namespace HydrogenCG
{
	/*!
		\brief	Base class for all integrators.

		The purpose of this class and inheriting classes are to render a scene from
		the perspective of a specific camera into an image.
		It also defines a function that returns the light energy (radiance) arriving at a ray's
		origin from the direction of the ray.
	*/
	class Integrator
	{
	public:
		virtual ~Integrator() {}

		/*!
			\brief				Renders the scene from the camera's perspective into the image.

			\param[out]			image Image object to render into.
			\param[in]			camera Camera from which to generate the rays for rendering the scene
			\param[in]			scene The scene to be rendered

			\return				A ray passing through the point corresponding to (u,v) on the virtual film
		*/
		virtual void render(Image& image, const Camera& camera, const Scene& scene) const
		{
			float aspectRatio = (float)image.w() / (float)image.h();
			for (uint32_t y = 0; y < image.h(); ++y)
			{
				for (uint32_t x = 0; x < image.w(); ++x)
				{
					/*
						This would be correspond to mainImage(...) in the shadertoy code, with:
						fragCoord = (x,y)
						iResolution = (image.w(), image.h())
					*/

					// map [0,width]x[0,height] to [-aspectRatio,aspectRatio] x [1,-1]
					// multiply by the aspect ratio to non-uniformly stretch/squeeze the virtual film size to match the screen's aspect ratio
					vec2 uv;
					uv.x = aspectRatio * (2.0f * ((float)x + 0.5f) / (float)image.w() - 1.0f);
					uv.y = -2.0f * ((float)y + 0.5f) / (float)image.h() + 1.0f;

					// generate ray corresponding to the normalized screen coordinates
					Ray ray = camera(uv);

					// evaluate radiance arriving along the ray
					image(x, y) = radiance(scene, ray);
				}
			}
		}

		/*!
			\brief	Computes the radiance arriving from the scene along the ray direction.
		*/
		virtual vec3 radiance(const Scene& scene, const Ray& ray) const = 0;
	};


	class BinaryIntegrator : public Integrator
	{
	public:
		//! returns white/black for intersection/no intersection
		vec3 radiance(const Scene& scene, const Ray& ray) const final
		{
			bool doesIntersect = valid(scene(ray));
			return vec3(static_cast<float>(doesIntersect));
		}
	};
	
	class ColorIntegrator : public Integrator
	{
	public:
		//! returns the color of objects 
		vec3 radiance(const Scene& scene, const Ray& ray) const final
		{
			return scene(ray).color;
		}
	};

	class InverseDistanceIntegrator : public Integrator
	{
	public:
		//! returns a greyscale color based on the reciprocal distance to intersections
		vec3 radiance(const Scene& scene, const Ray& ray) const final
		{
			return vec3(1.0f/scene(ray).dist);
		}
	};

	class NormalIntegrator : public Integrator
	{
	public:
		//! returns an rgb color by mapping the normals from [-1,1]^3 to [0,1]^3
		vec3 radiance(const Scene& scene, const Ray& ray) const final
		{
			Intersection intersection = intersect(scene, ray, 0.0f, INFINITY);

			/*
				Find whether the normal is facing towards the camera or away from it
				A normal is facing towards the camera if the angle between the ray direction
				and the normal is greater than 90 degrees (if we set the ray direction origin
				at the intersection point).

				We use the property of the dot product:
				dot(u,v) = length(u) * length(v) * cos(angle(u,v))
				Since length(ray.d) = 1 and length(intersection.normal) = 1
				We get:
				cos(angle(ray.d, intersection.normal)) = dot(ray.d, intersection.normal)
			*/
			float cosRayNormal = dot(ray.d, intersection.normal);

			/*
				If the normal is facing away from the camera (for example if the ray
				origin is inside a sphere), we need to flip it to get the correct
				facing normal, since we treat our objects as 2-sided.

				We use the fact that the cosine of an angle greater than 90 degrees is negative.
			*/
			vec3 normal = (cosRayNormal < 0.0f) ? intersection.normal : -intersection.normal;

			/*
				Return black if there is no intersection (valid(result) = 0 for no intersection)
				Otherwise return a color resulting from mapping the normal coordinates from
				[-1,1]^3 to [0,1]^3
				With our conventions we have:

				Pink for right facing normals: (1,0,0) -> (1,0.5,0.5)
				Cyan for left facing normals: (-1,0,0) -> (0,0.5,0.5)
				Light green for up facing normals: (0,1,0) -> (0.5,1,0.5)
				Purple for down facing normals: (0,-1,0) -> (0.5,0,0.5)
				Light blue for forward facing normals: (0,0,1) -> (0.5,0.5,1)
				Yellow/Orange for back facing normals: (0,0,-1) -> (0.5,0.5,0)

			*/
			return float(valid(intersection))*(0.5f*normal + vec3(0.5f));
		}
	};


	class TransparencyIntegrator : public Integrator
	{
	public:
		/*
			\brief Treats all objects as transparent, with transparency defined by their color

			This is the first integrator that spawns more than 1 ray, and it does so in succession.
			We set the background to white in this case (in order for the transparency to be visible
			with the backdrop being empty space).

			Note that a valid optimization is finding all valid intersections just once, rather than the
			closest one and attenuating based on that, however this requires modifying the intersect function
			for the scene, or writing a custom loop here, which we avoid for simplicity. Albeit ineffcient,
			the method that we use here will be used as a foundation to understand the algotihm we will
			use for constructive solid geometry (CSG).
		*/
		vec3 radiance(const Scene& scene, const Ray& r) const final
		{
			/*
				we set the initial color to 1 and attenuate based on the color
				of the intersected surfaces
			*/
			vec3 color = vec3(1);

			Ray ray = r;

			/*
				We allow at most 11 iterations, which should be enough for 5 spheres (since each
				can be intersected in at most 2 places by the same ray), the last iteration is there 
				in order to allow for the ray to intersect the background
			*/
			for (int i = 0; i < 11; ++i)
			{
				// at each iteration find the new intersection
				Intersection intersection = intersect(scene, ray, 0.0, INFINITY);
				// if we intersect nothing return the current color
				if (!valid(intersection)) return color;

				// flip the normal of the closest intersection to face in the correct direction
				float cosRayNormal = dot(ray.d, intersection.normal);
				vec3 normal = (cosRayNormal < 0.0f) ? intersection.normal : -intersection.normal;

				// if we intersect an object, attenuate with its color:
				color *= intersection.color;

				/*
					The ray below is the ray that will be used for the next intersection (it
					starts off at the previous intersection and continues along the same direction).

					Similar to the diffuse direct illumination integrator we perform an offset
					to avoid self-intersection, however, this time around we offset the
					intersection position to the other side of the surface, since we want to
					continue our ray on the opposite side.
				*/
				ray = Ray(intersection.pos - EPSILON * normal, ray.d);
			}
			// if we run out of iterations return black
			return vec3(0);
		}
	};

	class DiffuseLocalDirectIlluminationIntegrator : public Integrator
	{
	public:
		/*
			\brief Computes the local (no shadows) direct illumination treating all objects as diffuse

			Iterates over all light sources to compute the shading at the first ray-scene intersection.
			Objects cannot cast shadows, since we consider each object as if it was situated in a scene
			with no other objects. This is what rasterization graphics does (unless the visibility term
			is approximated by a shadow map/shadow volume).

			We consider only direct illumination - that means that we ignore effects such as indirect
			illumination (some light bouncing off some other to illuminate our shading point). We will
			consider some of those in an integrator introduced in the Whitted style ray tracing chapter,
			and most (if not all) of those in the path tracing chapter.
		*/
		vec3 radiance(const Scene& scene, const Ray& ray) const final
		{
			// compute the primary ray intersection with the scene
			Intersection intersection = intersect(scene, ray, 0.0f, INFINITY);

			// if there's no intersection return black
			if (!valid(intersection)) return vec3(0);

			/*
				We will accumulate the final color here (an estimation of the flux through a film pixel)

				In the chapters on distributed ray tracing and path tracing we will elaborate more on the
				virtual film, our camera model, and why it is precisely the flux that we are interested in.
			*/
			vec3 color = vec3(0);

			// the position of the point being shaded
			vec3 pos = intersection.pos;
			/*
				the INV_PI is so that the color of a material can be given in the range [0,1] and be
				energy conserving (we'll use this property later on with ray and path tracing, so that
				we can guarantee convergence with energy conserving materials)
			*/
			vec3 albedo = intersection.color * INV_PI;

			/*
				Similarly to the normal integrator we want to find the
				correct-facing normal, so that even if the camera is inside
				a sphere we get correct shading.

				Note that a light source may also be present inside of an object.
			*/
			float cosRayNormal = dot(ray.d, intersection.normal);
			vec3 normal = (cosRayNormal < 0.0f) ? intersection.normal : -intersection.normal;


			/*
				We always add the ambient contribution since it's "omnipresent" in the scene.
				The PI term comes from summing up all ambient contributions from all possible directions
				on the upper hemisphere around the normal of the intersection point.

				This will become clear in the context of the rendering equation in the path tracing chapter.
			*/
			color += PI * albedo * sampleRadiance(scene.ambientLight, pos).radiance;

			// iterate over all point light sources and accumulate their contribution
			for (int i = 0; i < scene.pointLights.size(); ++i)
			{
				// find the radiance travelling from the light to the intersection point
				LightSample lightSample = sampleRadiance(scene.pointLights[i], pos);
				vec3 radiance = lightSample.radiance;
				/*
					the (negative) direction along which light travels from our sample to the
					intersection point (it points from the intersection point to the light).
				*/
				vec3 direction = lightSample.direction;

				/*
					The foreshortening term due to Lambert's cosine law.
					We limit it to non-negative values, since (with our assumptions
					about opaque surfaces) light cannot arrive from the lower hemisphere
					around the normal of the intersection point. So we set the contribution
					to 0 in those cases.
				*/
				float cosLambert = max(0.0f, dot(normal, direction));

				/*
					In the real world a light ray may intersect another object before reaching
					a specific point - meaning that if there is an object between our light source
					and the point being shaded, we should NOT add the light contribution. This allows
					for shadows to be cast by objects.

					However, for this integrator we consider all objects locally - as if there were
					no other objects present in the scene. So we set the visibility term to 1.
				*/
				float visibility = 1.0f;

				/*
					Here we add the light contribution from point light i:
					(this is simply the integrand of the rendering equation)
					We multiply by the visibility only to emphasize that this term
					should actually be present in the general case (when we consider shadows).
				*/
				color += albedo * radiance * cosLambert * visibility;
			}

			// iterate over all directional light sources and accumulate their contribution
			for (int i = 0; i < scene.directionalLights.size(); ++i)
			{
				LightSample lightSample = sampleRadiance(scene.directionalLights[i], pos);
				vec3 radiance = lightSample.radiance;
				vec3 direction = lightSample.direction;

				float cosLambert = max(0.0f, dot(normal, direction));
				float visibility = 1.0f;
				color += albedo * radiance * cosLambert * visibility;
			}

			// iterate over all cone light sources and accumulate their contribution
			for (int i = 0; i < scene.coneLights.size(); ++i)
			{
				LightSample lightSample = sampleRadiance(scene.coneLights[i], pos);
				vec3 radiance = lightSample.radiance;
				vec3 direction = lightSample.direction;

				float cosLambert = max(0.0f, dot(normal, direction));
				float visibility = 1.0f;
				color += albedo * radiance * cosLambert * visibility;
			}

			// iterate over all cylinder light sources and accumulate their contribution
			for (int i = 0; i < scene.cylinderLights.size(); ++i)
			{
				LightSample lightSample = sampleRadiance(scene.cylinderLights[i], pos);
				vec3 radiance = lightSample.radiance;
				vec3 direction = lightSample.direction;

				float cosLambert = max(0.0f, dot(normal, direction));
				float visibility = 1.0f;
				color += albedo * radiance * cosLambert * visibility;
			}

			// return the accumulated color (flux)
			return color;
		}
	};

	class DiffuseDirectIlluminationIntegrator : public Integrator 
	{
	public:
		/*
			\brief Computes the direct illumination treating all objects as diffuse

			Unlike the local direct illumination integrator, this integrator considers visibility
			by shooting shadow rays. Thus objects can cast shadows in this case. It still does not
			model indirect illumination effects, hence the name.
		*/
		vec3 radiance(const Scene& scene, const Ray& ray) const final
		{
			Intersection intersection = intersect(scene, ray, 0.0f, INFINITY);
			if (!valid(intersection)) return vec3(0);

			vec3 color = vec3(0);

			// normalize the material color
			vec3 albedo = intersection.color * INV_PI;

			// flip the normal to face in the correct direction
			float cosRayNormal = dot(ray.d, intersection.normal);
			vec3 normal = (cosRayNormal < 0.0f) ? intersection.normal : -intersection.normal;

			/*
				In order to avoid self-intersection when shooting shadow rays, we offset the
				intersection point along the normal. This is necessary if one uses floating point
				numbers, since round-off error may cause an intersection to end up on the "wrong side"
				of an object's surface. And if that happens, when shooting a shadow ray it will intersect
				the surface we are trying to shade and it would be erroneously classified to be in shadow.

				A similar issue arises in rasterization graphics causing shadow acne. We encourage the reader
				to set EPSILON to 0, to see the self-intersection artifacts.

				We use the normal since the direction in which the distance to the object sruface is
				shortest at this point is precisely the normal.

				Note that this is an issue inherent to floating point numbers and it does not arise
				when one uses fixed-point or rational numbers. For more information see:
				http://iquilezles.org/www/articles/floatingbar/floatingbar.htm
			*/
			vec3 pos = intersection.pos + EPSILON * normal;

			// add ambient light contribution
			color += PI * albedo * sampleRadiance(scene.ambientLight, intersection.pos).radiance;

			// iterate over all point light sources and accumulate their contribution
			for (int i = 0; i < scene.pointLights.size(); ++i)
			{
				LightSample lightSample = sampleRadiance(scene.pointLights[i], pos);
				vec3 radiance = lightSample.radiance;
				vec3 direction = lightSample.direction;

				// The term due to Lambert's cosine law
				float cosLambert = max(0.0f, dot(normal, direction));

				/*
					We trace a ray segment from the (offset) intersection point until the light.
					If we intersect any object in that range, this means that there is an object
					occluding the light source, so we set the visibility to 0.
				*/
				Ray shadowRay = Ray(pos, direction);
				float distanceToLight = lightSample.distanceToLight;
				float visibility = float(!intersectAny(scene, shadowRay, 0.0f, distanceToLight));

				color += albedo * radiance * cosLambert * visibility;
			}

			// iterate over all directional light sources and accumulate their contribution
			for (int i = 0; i < scene.directionalLights.size(); ++i)
			{
				LightSample lightSample = sampleRadiance(scene.directionalLights[i], pos);
				vec3 radiance = lightSample.radiance;
				vec3 direction = lightSample.direction;

				float cosLambert = max(0.0f, dot(normal, direction));

				Ray shadowRay = Ray(pos, direction);
				float distanceToLight = lightSample.distanceToLight;
				float visibility = float(!intersectAny(scene, shadowRay, 0.0f, distanceToLight));

				color += albedo * radiance * cosLambert * visibility;
			}

			// iterate over all cone light sources and accumulate their contribution
			for (int i = 0; i < scene.coneLights.size(); ++i)
			{
				LightSample lightSample = sampleRadiance(scene.coneLights[i], pos);
				vec3 radiance = lightSample.radiance;
				vec3 direction = lightSample.direction;

				float cosLambert = max(0.0f, dot(normal, direction));

				Ray shadowRay = Ray(pos, direction);
				float distanceToLight = lightSample.distanceToLight;
				float visibility = float(!intersectAny(scene, shadowRay, 0.0f, distanceToLight));

				color += albedo * radiance * cosLambert * visibility;
			}

			// iterate over all cylinder light sources and accumulate their contribution
			for (int i = 0; i < scene.cylinderLights.size(); ++i)
			{
				LightSample lightSample = sampleRadiance(scene.cylinderLights[i], pos);
				vec3 radiance = lightSample.radiance;
				vec3 direction = lightSample.direction;

				float cosLambert = max(0.0f, dot(normal, direction));

				Ray shadowRay = Ray(pos, direction);
				float distanceToLight = lightSample.distanceToLight;
				float visibility = float(!intersectAny(scene, shadowRay, 0.0f, distanceToLight));

				color += albedo * radiance * cosLambert * visibility;
			}

			// return the accumulated color (flux)
			return color;
		}
	};
}