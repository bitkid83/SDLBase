#ifndef _H_VECTORMATH
#define _H_VECTORMATH

struct vec2
{
	double x, y;
};

double Vec2Length(vec2 A);

vec2 Vec2ScalarDivide(vec2 A, double scalar);

vec2 Vec2Normalize(vec2 A);

#endif