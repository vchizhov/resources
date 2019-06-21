#pragma once
/*
	@author: Vassillen Chizhov, 2019

	A very simple mathematical vector library
*/

#include <assert.h>
#include <math.h>
#include "math.hpp"
#include "..\typedef.hpp"

namespace HydrogenCG
{
	/*
		\brief	A 2-dimensional float-valued vector structure
	*/
	struct vec2
	{
		// this union is not standard conformant, but supported by major compilers
		// allows to alias the same variables through x,y; r,g; s,t; e[0],e[1]
		union
		{
			float e[2];		// e stands for element here
			struct
			{
				float x, y;	//!< Coordinates (components) of the vector
			};


			// convenience
			struct
			{
				float r, g;
			};
			struct
			{
				float s, t;
			};

		};

		vec2() {}
		vec2(const vec2& arg) : x(arg.x), y(arg.y) {}

		//! Creates a vector with all components equal to 'scalar'
		explicit vec2(float scalar) : x(scalar), y(scalar) {}
		vec2(float x, float y) : x(x), y(y) {}

		vec2& operator=(const vec2& arg)
		{
			x = arg.x;
			y = arg.y;
			return *this;
		}

		float& at(u32 i) { assert(i < 2 && "HydrogenCG::vec2::at():: Array index out of bounds"); return e[i]; }
		const float& at(u32 i) const { assert(i < 2 && "HydrogenCG::vec2::at():: Array index out of bounds"); return e[i]; }
		float& operator()(u32 i) { return e[i]; }
		const float& operator()(u32 i) const { return e[i]; }
		float& operator[](u32 i) { return e[i]; }
		const float& operator[](u32 i) const { return e[i]; }


		// Cooordinatewise / Hadamard operations

		vec2& operator+=(const vec2& arg)
		{
			x += arg.x;
			y += arg.y;
			return *this;
		}

		vec2& operator-=(const vec2& arg)
		{
			x -= arg.x;
			y -= arg.y;
			return *this;
		}

		vec2& operator*=(const vec2& arg)
		{
			x *= arg.x;
			y *= arg.y;
			return *this;
		}

		vec2& operator/=(const vec2& arg)
		{
			x /= arg.x;
			y /= arg.y;
			return *this;
		}

		// Scalar operations
		vec2& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		vec2& operator/=(float scalar)
		{
			float invScalar = 1.0f / scalar; // note: precision is not as good as dividing each component
			x *= invScalar;
			y *= invScalar;
			return *this;
		}
	};

	inline vec2 operator+(const vec2& arg)
	{
		return arg;
	}

	inline vec2 operator-(const vec2& arg)
	{
		return vec2(-arg.x, -arg.y);
	}

	// Coordinatewise / Hadamard operations

	inline vec2 operator+(const vec2& lhs, const vec2& rhs)
	{
		return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	inline vec2 operator-(const vec2& lhs, const vec2& rhs)
	{
		return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	inline vec2 operator*(const vec2& lhs, const vec2& rhs)
	{
		return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
	}

	inline vec2 operator/(const vec2& lhs, const vec2& rhs)
	{
		return vec2(lhs.x / rhs.x, lhs.y / rhs.y);
	}

	// Scalar operations

	inline vec2 operator*(const vec2& lhs, float rhs)
	{
		return vec2(lhs.x * rhs, lhs.y * rhs);
	}

	inline vec2 operator/(const vec2& lhs, float rhs)
	{
		float invRhs = 1.0f / rhs; // note: precision is not as good as dividing each component
		return vec2(lhs.x * invRhs, lhs.y * invRhs);
	}

	inline vec2 operator*(float lhs, const vec2& rhs)
	{
		return vec2(lhs * rhs.x, lhs * rhs.y);
	}

	inline vec2 operator/(float lhs, const vec2& rhs)
	{
		return vec2(lhs / rhs.x, lhs / rhs.y);
	}

	//! Returns the dot/inner/scalar product of two vectors
	inline float dot(const vec2& lhs, const vec2& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	//! Returns the squared euclidean norm of a vector
	inline float lengthSquared(const vec2& arg)
	{
		return dot(arg, arg);
	}

	//! Returns the euclidean norm of a vector
	inline float length(const vec2& arg)
	{
		return sqrtf(lengthSquared(arg));
	}

	//! Returns a vector with the same direction as arg, but unit length (undefined for 0)
	inline vec2 normalize(const vec2& arg)
	{
		return arg / length(arg);
	}

	//! Returns the signed parallelogram area of two 2D vectors (||lhs|| * ||rhs|| * sin(lhs,rhs))
	inline float cross(const vec2& lhs, const vec2& rhs)
	{
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	inline vec2 clamp(const vec2& arg, float minVal, float maxVal)
	{
		return vec2(clamp(arg.x, minVal, maxVal), clamp(arg.y, minVal, maxVal));
	}




	/*
		\brief	A 3-dimensional float-valued vector structure
	*/
	struct vec3
	{
		// this union is not standard conformant, but supported by major compilers
		// allows to alias the same variables through xyz, rgb, stp, e[0],e[1],e[2]
		union
		{
			struct
			{
				float x, y, z;	//!< Coordinates (components) of the vector
			};
			struct
			{
				float r, g, b;
			};
			struct
			{
				float s, t, p;
			};
			float e[3];		// e stands for element here
		};

		vec3() {}
		//! Creates a vector with all components equal to 'scalar'
		explicit vec3(float scalar) : x(scalar), y(scalar), z(scalar) {}
		vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		vec3(const vec3& arg) : x(arg.x), y(arg.y), z(arg.z) {}

		vec3& operator=(const vec3& arg)
		{
			x = arg.x;
			y = arg.y;
			z = arg.z;
			return *this;
		}

		float& at(u32 i) { assert(i < 3 && "HydrogenCG::vec3::operator():: Array index out of bounds"); return e[i]; }
		const float& at(u32 i) const { assert(i < 3 && "HydrogenCG::vec3::operator():: Array index out of bounds"); return e[i]; }
		float& operator()(u32 i) { return e[i]; }
		const float& operator()(u32 i) const { return e[i]; }
		float& operator[](u32 i) { return e[i]; }
		const float& operator[](u32 i) const { return e[i]; }


		// Coordinatewise / Hadamard operations

		vec3& operator+=(const vec3& arg)
		{
			x += arg.x;
			y += arg.y;
			z += arg.z;
			return *this;
		}

		vec3& operator-=(const vec3& arg)
		{
			x -= arg.x;
			y -= arg.y;
			z -= arg.z;
			return *this;
		}

		vec3& operator*=(const vec3& arg)
		{
			x *= arg.x;
			y *= arg.y;
			z *= arg.z;
			return *this;
		}

		vec3& operator/=(const vec3& arg)
		{
			x /= arg.x;
			y /= arg.y;
			z /= arg.z;
			return *this;
		}

		// Scalar operations
		vec3& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		vec3& operator/=(float scalar)
		{
			float invScalar = 1.0f / scalar; // note: precision is not as good as dividing each component
			x *= invScalar;
			y *= invScalar;
			z *= invScalar;
			return *this;
		}
	};

	inline vec3 operator+(const vec3& arg)
	{
		return arg;
	}

	inline vec3 operator-(const vec3& arg)
	{
		return vec3(-arg.x, -arg.y, -arg.z);
	}

	// Coordinatewise / Hadamard operations

	inline vec3 operator+(const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	inline vec3 operator-(const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	inline vec3 operator*(const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
	}

	inline vec3 operator/(const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
	}

	// Scalar operations

	inline vec3 operator*(const vec3& lhs, float rhs)
	{
		return vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
	}

	inline vec3 operator/(const vec3& lhs, float rhs)
	{
		float invRhs = 1.0f / rhs; // note: precision is not as good as dividing each component
		return vec3(lhs.x * invRhs, lhs.y * invRhs, lhs.z * invRhs);
	}

	inline vec3 operator*(float lhs, const vec3& rhs)
	{
		return vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
	}

	inline vec3 operator/(float lhs, const vec3& rhs)
	{
		return vec3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
	}

	//! Returns the dot/inner/scalar product of two vectors
	inline float dot(const vec3& lhs, const vec3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	//! Returns the squared euclidean norm of a vector
	inline float lengthSquared(const vec3& arg)
	{
		return dot(arg, arg);
	}

	//! Returns the euclidean norm of a vector
	inline float length(const vec3& arg)
	{
		return sqrtf(lengthSquared(arg));
	}

	//! Returns a vector with the same direction as arg, but unit length (undefined for 0)
	inline vec3 normalize(const vec3& arg)
	{
		return arg / length(arg);
	}

	//! Returns the cross product of two vectors (a vector orthogonal to both, with length ||lhs|| * ||rhs|| * |sin(lhs,rhs)|)
	inline vec3 cross(const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
	}

	inline vec3 clamp(const vec3& arg, float minVal, float maxVal)
	{
		return vec3(clamp(arg.x, minVal, maxVal), clamp(arg.y, minVal, maxVal), clamp(arg.z, minVal, maxVal));
	}

	// convenience coordinatewise operations
	inline vec3 pow(const vec3& arg, float scalar)
	{
		return vec3(powf(arg.x, scalar), powf(arg.y, scalar), powf(arg.z, scalar));
	}

	inline vec3 max(const vec3& arg, float scalar)
	{
		return vec3(max(arg.x, scalar), max(arg.y, scalar), max(arg.z, scalar));
	}

	inline vec3 min(const vec3& arg, float scalar)
	{
		return vec3(min(arg.x, scalar), min(arg.y, scalar), min(arg.z, scalar));
	}

	inline vec3 round(const vec3& arg)
	{
		return vec3(roundf(arg.x), roundf(arg.y), roundf(arg.z));
	}



}