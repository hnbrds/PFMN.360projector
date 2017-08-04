#ifndef WAVEFRONTOBJ_H
#define WAVEFRONTOBJ_H 

#include <vector>
#include <stdlib.h>
#include <math.h>
using std::vector;

//==============================================================================
class Vertex {
//==============================================================================
public:
	double x, y, z;

	Vertex( double x=0, double y=0, double z=0 ) {
		setCoordinates( x, y, z );
	}
	void setCoordinates( double xval, double yval, double zval ) {
		x = xval; y = yval; z = zval;
	}
};

//==============================================================================
class Normal {
//==============================================================================
public:
	double x, y, z;

	Normal ( double x=0, double y=0, double z=0 ) {
		setCoordinates( x, y, z );
	}
	void setCoordinates(double xval, double yval, double zval) {
		double l = sqrt(xval*xval + yval*yval + zval*zval);
		if (l != 0.0)
			l = 1.0 / l;
		x = l*xval;
		y = l*yval;
		z = l*zval;
	}
};

//==============================================================================
class TexCoord {
//==============================================================================
public:
	double u, v;

	TexCoord(double u, double v) {
		setCoordinates(u, v);
	}
	void setCoordinates(double uval, double vval) {
		u = uval;
		v = vval;
	}
};

//==============================================================================
class Face {
//==============================================================================
public:
	int vCount;
	int vIndexStart;
	Normal normal;

	Face( int vIndexStart = -1 ) : vIndexStart(vIndexStart), vCount(0) {}
};


//==============================================================================
class WaveFrontOBJ  {
//==============================================================================
public:
	vector<Vertex> verts;
	vector<Normal> normals;
	vector<TexCoord> texCoords;
	vector<Normal> faceNormals;
	vector<Face> faces;
	vector<int> vIndex;
	vector<int> nIndex;
	vector<int> tIndex;

	bool isFlat;
	GLuint mode;

	Vertex bbmin, bbmax; // bounding box

	WaveFrontOBJ(const char *filename);
	void Draw();

protected:
	Normal faceNormal(Vertex& v0, Vertex& v1, Vertex& v2);
	void computeExtents();
};


#endif // WAVEFRONTOBJ_H
