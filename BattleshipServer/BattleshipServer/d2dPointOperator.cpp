#include "stdafx.h"
#include "d2dPointOperator.h"

D2D_POINT_2F operator+(const D2D_POINT_2F lhs, const D2D_POINT_2F& rhs){
	D2D_POINT_2F temp = lhs;
	temp.x += rhs.x;
	temp.y += rhs.y;
	return temp;
}


D2D_POINT_2F operator-(const D2D_POINT_2F lhs, const D2D_POINT_2F& rhs){
	D2D_POINT_2F temp = lhs;
	temp.x -= rhs.x;
	temp.y -= rhs.y;
	return temp;
}

D2D_POINT_2F operator*(const D2D_POINT_2F lhs, const float& rhs){
	D2D_POINT_2F temp = lhs;
	temp.x *= rhs;
	temp.y *= rhs;
	return temp;
}

float D2DPointSize(const D2D_POINT_2F point)
{
	return powf((point.x * point.x + point.y * point.y), 0.5);
}