#include "mathclass.h"
#include "vector4.h"
#include "math_macro.h"
void vector4::operator+=( vector4 const& b)
{
	x+=b.x;
	y+=b.y;
	z+=b.z;
}

void vector4::operator-=( vector4 const& b)
{
	x-=b.x;
	y-=b.y;
	z-=b.z;
}

void vector4::operator*=( double a)
{
	x*=a;
	y*=a;
	z*=a;
}

vector4    vector4::operator+( vector4 const& b) const
{
	const vector4& a=*this;
	return vector4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}

vector4    vector4::operator-( vector4 const& b) const
{
	const vector4& a=*this;
	return vector4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
}

void vector4::operator/=( double a)
{
	this->operator*=(1.f/a);
}

vector4& vector4::operator=(vector4 const& a)
{
	x=a.x;
	y=a.y;
	z=a.z;
	return *this;
}
vector4 vector4::operator*( double b) const
{
	vector4 const& a=*this;
	return vector4(a.x*b, a.y*b, a.z*b, a.w);
}

vector4 vector4::operator/( double b) const
{
	return this->operator*(1.0/b);
}

vector4    operator*(double b, vector4 const& a )
{
	return a*b;
}


