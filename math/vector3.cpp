
#include "mathclass.h"
#include <iostream>
#include "vector3.h"
#include "quater.h"
#include "math_macro.h"

static double eps = 0.0001f;


void vector3::interpolate( double t, vector3 const& a, vector3 const& b )
{
	add((1.0f-t)*a , t*b);
}

double len( vector3 const& v )
{
    return (double)sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
}

void vector3::zero()
{
	x=0;
	y=0;
	z=0;
}

void vector3::add(const vector3& a, const vector3& b)
{
	x=a.x+b.x;
	y=a.y+b.y;
	z=a.z+b.z;
}

void vector3::add(const vector3& other)
{
	x+=other.x;
	y+=other.y;
	z+=other.z;
}

void vector3::sub(const vector3& other)
{
	x-=other.x;
	y-=other.y;
	z-=other.z;
}

void vector3::sub(const vector3& a, const vector3& b)
{
	x=a.x-b.x;
	y=a.y-b.y;
	z=a.z-b.z;
}

double
vector3::length() const
{
    return (double)sqrt( x*x + y*y + z*z );
}

void vector3::operator+=( vector3 const& b)
{
	x+=b.x;
	y+=b.y;
	z+=b.z;
}

void vector3::operator-=( vector3 const& b)
{
	x-=b.x;
	y-=b.y;
	z-=b.z;
}


bool vector3::operator==(vector3 const& a)const
{
	return x==a.x && y==a.y && z==a.z;
}
vector3    vector3::operator*( vector3 const& a) const
{
	return vector3(x*a.x, y*a.y, z*a.z);

}
void vector3::operator*=(vector3 const& a)
{
	x*=a.x;
	y*=a.y;
	z*=a.z;
}

void vector3::operator*=( double a)
{
	x*=a;
	y*=a;
	z*=a;
}

void vector3::leftMult( const matrix4& mat)
{
	vector3 temp(*this);
	this->mult(mat,temp);
}

vector3    vector3::operator+( vector3 const& b) const
{
	const vector3& a=*this;
	vector3 c;
	c.add(a,b);
	return c;
}

vector3    vector3::operator-( vector3 const& b) const
{
	const vector3& a=*this;
	vector3 c;
	c.sub(a,b);
	return c;
}

void vector3::operator/=( double a)
{
	this->operator*=(1.f/a);
}

vector3& vector3::operator=(vector3 const& a)
{
	x=a.x;
	y=a.y;
	z=a.z;
	return *this;
}

/*
matrix4
vector3::cross() const
{
    return matrix4(	vector3(  0 ,-z(), y()),
					vector3( z(),  0 ,-x()),
					vector3(-y(), x(),  0 ) );
}
*/

void vector3::mult(const vector3& a,double b)
{
	setValue(a.x*b, a.y*b, a.z*b);
}

void vector3::multadd(const vector3& a, double w)
{
	x+=a.x*w;
	y+=a.y*w;
	z+=a.z*w;
}

void vector3::mult(const matrix4& mat, const vector3& v)
{
#ifdef USE_D3DFUNC
	//Transforms a 3-D vector by a given matrix, projecting the result back into w = 1
	D3DXMATRIX dxmat;
	mat.toDXmat(dxmat);
	D3DXVec3TransformCoord(*this, v, &dxmat);
#else
	
	double w=mat._41*v.x+mat._42*v.y+mat._43*v.z+mat._44;
	x=mat._11*v.x+mat._12*v.y+mat._13*v.z+mat._14;
	y=mat._21*v.x+mat._22*v.y+mat._23*v.z+mat._24;
	z=mat._31*v.x+mat._32*v.y+mat._33*v.z+mat._34;
	if(w!=1.0)
	{
		x/=w;
		y/=w;
		z/=w;
	}

#endif
}

void vector3::linearVelocity(vector3 const& v1, vector3 const& v2)
{
	//v1+this=v2;
	sub(v2, v1);
}

void vector3::angularVelocity( quater const& a, quater const& b )
{
	quater c;
	c.difference(a,b);
	rotationVector(c);
}

void vector3::cross(const vector3& a, const vector3& b)
{
    x = a.y*b.z - a.z*b.y;
    y = a.z*b.x - a.x*b.z;
    z = a.x*b.y - a.y*b.x;
}

/*vector3 vector3::operator*(vector3 const& b ) const
{
	vector3 const& a=*this;
    vector3 c;
	c.cross(a,b);
    return c;
}*/

vector3 vector3::operator*( double b) const
{
	vector3 const& a=*this;
	vector3 c;
	c.mult(a,b);
	return c;
}

vector3 vector3::operator/( double b) const
{
	vector3 const& a=*this;
	vector3 c;
	c.mult(a,1/b);
	return c;
}

vector3    operator*(double b, vector3 const& a )
{
	vector3 c;
	c.mult(a,b);
	return c;
}

double vector3::operator%(vector3 const& b) const
{
	vector3 const& a=*this;
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

void vector3::ln(quater const& q)
{
    double sc = (double)sqrt(q.x * q.x + q.y * q.y + q.z * q.z);
    double theta = (double)atan2((double)sc, (double)q.w);
    if(sc > eps)
        sc = theta / sc;
    else  sc = 1.0 ;
    setValue(sc * q.x, sc * q.y, sc * q.z);
}

double vector3::cosTheta(vector3 const& b) const
{
	vector3 const& a=*this;
	  return ( (a%b)/(len(a)*len(b)) );
}

double vector3::angle( vector3 const& b ) const
{
	vector3 const& a=*this;
    return (double)ACOS( (a%b)/(len(a)*len(b)) );
}

double vector3::sinTheta(vector3 const& b) const
{
	vector3 const& a=*this;

	// |a cross b| = |a||b|sinTheta
	ASSERT(a.z==0);
	ASSERT(b.z==0);

	vector3 a3,b3;

	a3.x=a.x;
	a3.y=a.y;
	a3.z=0;
	b3.x=b.x;
	b3.y=b.y;
	b3.z=0;
	double sinTheta;
	if(a%b>0)
	{
		vector3 crs;
		crs.cross(a3,b3);
		sinTheta=crs.z/(a.length()*b.length());
	}
	else
	{
		vector3 crs;
		crs.cross(b3,-a3);
		sinTheta=crs.z/(a.length()*b.length());
	}
	ASSERT(-1<=sinTheta);
	ASSERT(sinTheta<=1);

	return sinTheta;
}

// calc angle between 0 to 2pi assumming z=0;
double vector3::angle2d(vector3 const& b) const
{
	vector3 const& a=*this;

	double rad=a.angle(b);
	// length 가 0인경우 발생.
	if(a.sinTheta(b)<0)
		return (double)(2.0*M_PI-rad);
	
	return rad;
}

double vector3::angle2ds(vector3 const& b) const
{
	double angle=angle2d(b);
	if(angle>M_PI)
		angle-=2*M_PI;
	return angle;
}

double vector3::angle2ds(vector3 const& b, int axis) const
{
	vector3 c, d;
	
	switch(axis)
	{
	case 0:
		c.setValue(y,z,0);
		d.setValue(b.y, b.z, 0);
		break;
	case 1:
		c.setValue(z,x,0);
		d.setValue(b.z, b.x, 0);
		break;
	case 2:
		c.setValue(x,y,0);
		d.setValue(b.x, b.y, 0);
	}

	return c.angle2ds(d);
}

std::ostream& operator<<( std::ostream& os, vector3 const& a )
{
    os << "( " << a.x << " , " << a.y << " , " << a.z << " )";
    return os;
}

/*
istream& operator>>( istream& is, vector3& a )
{
	static char buf[256];

    is >> buf >> a.x >> buf >> a.y >> buf >> a.z >> buf;
    return is;
}
*/
void vector3::normalize()
{
	double invl;
	if(this->length()==0)
		invl=0;
	else
		invl=1/this->length();
    (*this)*=invl;
}

void vector3::normalize( const vector3 & a)
{
	double invl;
	if(a.length()==0)
		invl=0;
	else
		invl=1/a.length();
	this->mult(a,invl);
}

void vector3::negate(const vector3& a)
{
	vector3& c=*this;
	c.mult(a, -1);
}

double vector3::squaredDistance(const vector3& other) const
{
	vector3 s;
	s.sub(*this, other);
	return s%s;
}
double vector3::distance(const vector3& other) const
{
	vector3 s;
	s.sub(*this, other);
	return s.length();
}

void vector3::column(int col, const matrix4& other)
{
	//!< extract a column from the rotation part in a matrix
	for(int i=0; i<3; i++)
		(*this)[i]=other.m[i][col];
}

void vector3::row(int row, const matrix4& other)
{
	//!< extract a row from the rotation part in a matrix
	for(int j=0; j<3; j++)
		(*this)[j]=other.m[row][j];
}

void vector3::translation(const matrix4& other)
{
	//!< extract translation vector from a matrix
	column(3, other);
}

void vector3::imaginaries(const quater& in)
{
	x=in.x; y=in.y; z=in.z;
}

void vector3::rotate( const quater& q)
{
	vector3 t(*this);
	rotate(q, t);
}

void vector3::rotate( const quater& a, vector3 const& v)
{
/*
#ifdef USE_D3DFUNC
	D3DXMATRIX matRot;
	D3DXMatrixRotationQuaternion(&matRot, a);
	D3DXVec3TransformNormal(*this, v, &matRot);
#else*/
	quater inv_a;
	inv_a.inverse(a);
    quater c = a * quater(0, v.x, v.y, v.z) * inv_a;
    x=c.x;
	y=c.y;
	z=c.z;
//#endif
}


void vector3::rotationVector(const quater& in)
{
	vector3& out=*this;
	out.log(in);
	out*=2.f;
}

quater	 vector3::exp() const		
{ 
	quater c; 
	c.exp(*this); 
	return c;
}

void vector3::hermite(const vector3& p1, const vector3& t1, const vector3& p2, const vector3& t2, double s)
{
	//!< hermite interpolation of p1 and p2. 0<=t<=1
	double h1 =  2*CUBIC(s) - 3*SQR(s) + 1;          // calculate basis function 1
	double h2 = -2*CUBIC(s) + 3*SQR(s);              // calculate basis function 2
	double h3 =   CUBIC(s) - 2*SQR(s) + s;         // calculate basis function 3
	double h4 =   CUBIC(s) -  SQR(s);              // calculate basis function 4

	// multiply and sum all funtions together to build the interpolated point along the curve.
	*this= h1*p1 + h2*p2 + h3*t1 + h4*t2;
}


quater vector3::quaternion() const
{
	//!< rotation vector를 quaternion으로 바꾼다.	
	quater q; q.setRotation(*this);  return q;
}


