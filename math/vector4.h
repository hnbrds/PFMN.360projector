#ifndef _VECTOR4_H_
#define _VECTOR4_H_
class matrix4;

class vector4
{
	public:
		double x,y,z,w;
		// constructors
		vector4() {}
		vector4( double v)				{ x=v; y=v; z=v; w=1;}
		vector4( double xx, double yy, double zz , double ww )				{ x=xx; y=yy; z=zz; w=ww;}


		vector4    operator+( vector4 const& ) const;
		vector4    operator-( vector4 const& ) const;
		vector4    operator*( double ) const;
		vector4    operator/( double ) const;
		// unary -
		inline vector4  operator-() const	{ return vector4(-x,-y,-z,w);}

		friend vector4    operator*( double, vector4 const& ) ;

		void operator+=( vector4 const& );
		void operator-=( vector4 const& );
		void operator*=( double );
		void operator/=( double );
		vector4& operator=(vector4 const& );

		inline operator const double*() const		{ return &x;}
		inline operator double*()					{ return &x;}
		const vector3& toVec3() const 	{ return *(vector3*)&x;}
		vector3& toVec3() 				{ return *(vector3*)&x;}
};
#endif
