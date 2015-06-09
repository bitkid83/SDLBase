#ifndef _H_VECTORMATH
#define _H_VECTORMATH
#include <math.h>

// todo: operator overloading
struct vec2
{
	inline vec2() { x = 0; y = 0; }
	inline vec2(double A, double B) { x = A; y = B; }

	double x;
	double y;
};

inline double LengthSquared(const vec2 &A)
{
	return (A.x * A.x) + (A.y * A.y);
}

inline double Length(const vec2 &A)
{
	double length = 0.0;
	length = sqrt(LengthSquared(A));

	return length;
}

inline vec2 Unit(vec2 A)
{
	double length = 1.0f / Length(A);
	A.x *= length;
	A.y *= length;

	return A;
}

// operator/=
inline vec2 DivScalar(vec2 A, double scalar)
{
	A.x = A.x / scalar;
	A.y = A.y / scalar;
	return A;
}

#endif