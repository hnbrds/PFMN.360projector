#ifndef MATRIX4_H_
#define MATRIX4_H_
#pragma once

class vector3;
class quater;
#include <iostream>
/// row-major matrix4 class following opengl convention 
// (positions are multiplied at right-side of the matrix).
/** 
openGL matrix는 R1*r2*v형태로 오른쪽에 점이 곱해진다.
하지만 D3DXMATRIX는 v*R1*R2형태로 점이 왼쪽에 곱해지고, 마찬가지로 D3DXQUATERNION은 q1*q2가 원래 정의의 q2*q1과 일치하도록 
곱하기순서가 바뀌어 있다. 즉 D3DXMATRIX와 D3DXQUATERNION모두 왼쪽에 있는 것이 로컬, 가장 오른쪽에 있는 것이 글로벌이다.
양쪽의 convention을 맞추기 위해서는 transpose가 필요하다.
*/

#ifdef minor
#undef minor
#endif
class matrix4
{
public:

	union {
        struct {
            double        _11, _12, _13, _14;
            double        _21, _22, _23, _24;
            double        _31, _32, _33, _34;
            double        _41, _42, _43, _44;

        };
        double m[4][4];
    };

	matrix4(const matrix4& mat); // copy constructor
	matrix4(const quater& rot, const vector3& trans)	{ setRotation(rot); setTranslation(trans);}
	matrix4();
	~matrix4();

	inline void identity()	{ setIdentityRot(); }

	// n-ary operators
	void lookAtRH(const vector3& eye, const vector3& at, const vector3& up);
	void setScaling(double sx, double sy, double sz);
	
	void setTransform(const quater& rot, const vector3& trans);
	void setTransform(const vector3& position, const vector3& scale, const quater& orientation);

	// all setRotation fuctions remove translation terms unless manually specified otherwise (bPreserveCurrentTranslation=true).
	void setRotation(const quater& q, bool bPreserveCurrentTranslation=false);
	void setRotation(const vector3& axis, double angle, bool bPreserveCurrentTranslation=false);
	void setRotationX(double angle);
	void setRotationY(double angle);
	void setRotationZ(double angle);
	void setRotation(const char* aChannel, double *aValue, bool bRightToLeft=false);	//!< from euler angle. aChannel="YXZ" or something like that.
	void setAxisRotation(const vector3& vecAxis, const vector3& front, const vector3& vecTarget); 	//!< front벡터를 vecAxis를 중심으로 회전해서 vecTarget과 vecAxis가 이루는 평면에 놓이도록 만드는 Matrix를 구한다.
	void setIdentityRot();	

	// setTranslation preserves rotation parts unless manually specified otherwise (bPreserveCurrentRotation=false)
	void setTranslation(const vector3& tx, bool bPreserveCurrentRotation=true);		//!< 다른 set계열 함수와 달리 rotation파트는 건드리지 않는다. 	
	vector3 getTranslation() const;

	void transpose();
	
	void getCurrentOpenGLmatrix(int type);
	// GLmatrix function is not thread-safe because it uses a static variable (global variable).
	const double* GLmatrix() const;

	bool operator==(const matrix4& other) const;

	void mult(const matrix4& a, const matrix4& b);
	void mult(const matrix4& a, const quater& b);
	void mult(const quater& a, const matrix4& b);

	// unary operators
	void adjoint(const matrix4& a);
	void inverse(const matrix4& a);
	void extractRot(const matrix4& a);
	void transpose(const matrix4& a);
	void leftMult(const matrix4& a);	//!< this=a*this;
	void operator*=(const matrix4& a);	//!< this=this*a;
	void rightMult(const matrix4& a) { (*this)*=a;}
	void leftMult(double scalar);		//!< a=matScale*a
	void leftMultScaling(double sx, double sy, double sz);
	void leftMultRotation(const quater& b);
	void leftMultRotation(const vector3& axis, double angle);
	void leftMultTranslation(const vector3& vec);

	matrix4 operator*(matrix4 const& a) const { matrix4 t; t.mult(*this,a); return t;}
	vector3 operator*(vector3 const& a) const;
	vector3 rotate(vector3 const & direction) const;
	
	// inquiry functions
	void decomposeLH(vector3& eye, vector3& at, vector3& up);
	void decomposeRH(vector3& eye, vector3& at, vector3& up);

	void setValue( double x00, double x01, double x02,
				   double x10, double x11, double x12,		// remaining values are set as 0 or 1.
				   double x20, double x21, double x22 )	;

	void setValue( double x00, double x01, double x02, double x03,
				   double x10, double x11, double x12, double x13,
				   double x20, double x21, double x22, double x23,
				   double x30, double x31, double x32, double x33)	;

	double determinant() const;
	double minor(const size_t r0, const size_t r1, const size_t r2, 
				const size_t c0, const size_t c1, const size_t c2) const;

	friend std::ostream& operator<<(std::ostream& os, const matrix4& arg);
private:
	void _discardTranslation();

};

#endif
