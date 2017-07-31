#ifndef INTERSECTION_TEST_H_
#define INTERSECTION_TEST_H_
#include <vector>
#include "math_macro.h"
#include "vector3.h"

//	Mainly for picking objects.
/*
	A plane is defined in 3D space by the equation
        Ax + By + Cz + D = 0
		(normal=(A,B,C))
 */
class Plane
{
public:
    Plane ();
	// vNormal should be a unit vector, offset=-d.
    Plane (const vector3& vNormal, double offset);
    Plane (const vector3& vNormal, const vector3& vPoint);
    Plane (const vector3& vPoint0, const vector3& vPoint1,
        const vector3& vPoint2);

	// + if a point is normal-side, otherwise - values
    double distance (const vector3& point) const;

    /** define this plane based on 3 points. */
    void setPlane(const vector3& vPoint0, const vector3& vPoint1,
        const vector3& vPoint2);

	/** define this plane based on a normal and a point. */
	void setPlane(const vector3& vNormal, const vector3& vPoint);

	// should be unit vector
	vector3 normal;
    double d;
};

class Sphere
{
public:
	vector3 center;
	double radius;	// half radius
	Sphere(vector3 c, double r):center(c), radius(r){}

	bool isInside(std::vector<Plane>& selectionVolume) const;
};

class Ray
{
protected:
    vector3 mOrigin;
    vector3 mDirection;
public:
    Ray():mOrigin(0,0,0), mDirection(0,0,1){}
    Ray(const vector3& origin, const vector3& direction)
        :mOrigin(origin), mDirection(direction) {}

	vector3& origin()					{ return mOrigin;}
	vector3 const& origin() const		{ return mOrigin;}

	vector3& direction()				{ return mDirection;}
	vector3 const& direction() const	{ return mDirection;}

	vector3 getPoint(double t) const
	{
		return mOrigin + (mDirection * t);
	}

	std::pair<bool, double> intersects(const Plane& p) const;
	// planes form a convex volume, and normal vectors face outside
	std::pair<bool, double> intersects(const std::vector<Plane>& planes) const;
	std::pair<bool, double> intersects(const Sphere& s) const;
	std::pair<bool, double> intersects(const vector3& a,
    const vector3& b, const vector3& c, const vector3& normal,
    bool positiveSide, bool negativeSide) const;

	std::pair<bool, double> intersects(const vector3& a,
    const vector3& b, const vector3& c, bool bCCW=true) const;
};

#endif
