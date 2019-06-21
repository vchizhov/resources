#include <iostream>
#include "core/math/vec.hpp"
#include "core/sphere.hpp"
#include "core/camera.hpp"
#include "core/scene.hpp"
#include "core/integrator.hpp"
#include "core/light.hpp"

/*
	TODO:
	Base class for light - less code dup in integrator, or templates
	comments in main
	rename project to hydrogenCG in cmake (without extra stuff)

*/

using namespace HydrogenCG;

const int renderMode = 5;
const int lightMode = 3;

int main()
{
	Image image;
	image.init(640, 480);
	Camera camera;
	Scene scene;

	scene.ambientLight.radiance = vec3(0.01f);

	// Add some scene geometry
	add(scene, Sphere(vec3(0, 0, 4), 1.0f, vec3(1, 0.5f, 0.1f)));
	add(scene, Sphere(vec3(-1, 0, 2.5f), 1.0f, vec3(0.3f, 1, 0.3f)));

	// a large sphere for the ground
	add(scene, Sphere(vec3(0, -1001, 0), 1000.0f, vec3(0.1f, 0.5f, 1.0f)));

	

	const Integrator* integrator = nullptr;

	if (renderMode == 0)
	{
		integrator = new BinaryIntegrator();
	}
	else if (renderMode == 1)
	{
		integrator = new ColorIntegrator();
	}
	else if (renderMode == 2)
	{
		integrator = new InverseDistanceIntegrator();
	}
	else if (renderMode == 3)
	{
		integrator = new NormalIntegrator();
	}
	else if (renderMode == 4)
	{
		integrator = new DiffuseLocalDirectIlluminationIntegrator();
	}
	else if (renderMode == 5)
	{
		integrator = new DiffuseDirectIlluminationIntegrator();
	}
	else
	{
		integrator = new TransparencyIntegrator();
	}


	vec3 pointLightTo = vec3(1, 0, 3);

	if (lightMode == 0)
	{
		add(scene, PointLight(vec3(30.0f), vec3(2.0f, 2, 2)));
	}
	else if (lightMode == 1)
	{
		add(scene, DirectionalLight(vec3(3.0f), normalize(pointLightTo - vec3(2, 2, 2))));
	}
	else if (lightMode == 2)
	{
		add(scene, CylinderLight(vec3(3.0f), vec3(2, 2, 2), normalize(pointLightTo - vec3(2, 2, 2)), 3.0f));
	}
	else
	{
		vec3 lightPos = vec3(2.0f, 2, 2);
		add(scene, ConeLight(vec3(30.0f), lightPos, normalize(vec3(1, 0, 3) - lightPos), cos(0.25f*PI)));
	}

	integrator->render(image, camera, scene);
	delete integrator;

	image.savePPM("out.ppm");

	return 0;
}