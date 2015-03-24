#include <math.h>
#include "vectormath.h"

double Vec2Length(vec2 A)
{
	double length = 0.0;
	length = sqrt((A.x * A.x) + (A.y * A.y));

	return length;
}

vec2 Vec2ScalarDivide(vec2 A, double scalar)
{
	A.x = A.x / scalar;
	A.y = A.y / scalar;
	return A;
}

vec2 Vec2Normalize(vec2 A)
{	
	double length = 1.0f / Vec2Length(A);
	A.x *= length;
	A.y *= length;

	return A;
}