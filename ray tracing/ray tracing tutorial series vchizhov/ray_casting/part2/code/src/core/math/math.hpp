#pragma once
#undef INFINITY
namespace HydrogenCG
{
	float min(float lhs, float rhs)
	{
		return lhs <= rhs ? lhs : rhs;
	}

	float max(float lhs, float rhs)
	{
		return lhs >= rhs ? lhs : rhs;
	}

	float clamp(float arg, float minVal, float maxVal)
	{
		return max(min(arg, maxVal), minVal);
	}

	// https://en.wikipedia.org/wiki/Smoothstep
	// we do not clamp
	float smoothstepCubic(float x)
	{
		return x * x*(3.0f - 2.0f*x);
	}

	// Ken Perlin, "Improving Noise": http://staffwww.itn.liu.se/~stegu/TNM022-2005/perlinnoiselinks/paper445.pdf
	// we do not clamp
	float smoothstepQuintic(float x)
	{
		return x * x*x*(x*(6.0f*x - 15.0f) + 10.0f);
	}

	// mirrors GLSL's smoothstep implementation
	float smoothstep(float edge0, float edge1, float x)
	{
		float y = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return smoothstepCubic(y);
	}

	const float EPSILON = 0.0001f;
	const float INFINITY = std::numeric_limits<float>::infinity();
	const float PI = static_cast<float>(3.14159265358979323846264338327950288L);
	const float INV_PI = 1.0f / PI;
}