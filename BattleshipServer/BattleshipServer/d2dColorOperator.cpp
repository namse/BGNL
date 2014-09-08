#include "stdafx.h"
#include "d2dColorOperator.h"

D2D_COLOR_F operator+(const D2D_COLOR_F lhs, const D2D_COLOR_F& rhs)
{
	D2D_COLOR_F temp = lhs;
	temp.r += rhs.r;
	temp.g += rhs.g;
	temp.b += rhs.b;
	temp.a += rhs.a;
	return temp;
}
D2D_COLOR_F operator-(const D2D_COLOR_F lhs, const D2D_COLOR_F& rhs)
{
	D2D_COLOR_F temp = lhs;
	temp.r -= rhs.r;
	temp.g -= rhs.g;
	temp.b -= rhs.b;
	temp.a -= rhs.a;
	return temp;
}
D2D_COLOR_F operator*(const D2D_COLOR_F lhs, const float& rhs)
{
	D2D_COLOR_F temp = lhs;
	temp.r *= rhs;
	temp.g *= rhs;
	temp.b *= rhs;
	temp.a *= rhs;
	return temp;
}