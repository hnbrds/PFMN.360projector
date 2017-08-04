
#include <iostream>
#include <cstring>
#include <fstream>
#include <sys/types.h>
#ifdef _MSC_VER
#include <Windows.h>
#endif
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
using namespace std;

#include "WaveFrontOBJ.h"

//------------------------------------------------------------------------------
// 객체를 생성하면서 filename 에서 object 를 읽는다
// Construct object and read object from filename
WaveFrontOBJ::WaveFrontOBJ(const char *filename) 
{
	isFlat = true;
	mode = GL_POLYGON;

	char *line = new char[200];
	char *line_back = new char[200];
	char wspace[] = " \t\n";
	char separator[] = "/";
	char *token;
	int indices[3];
	double x, y, z;
	double tex_u, tex_v;

	ifstream file(filename);
	if ( !file ) {
		cerr <<"Cannot open file: " <<filename <<" exiting." <<endl;
		exit ( -1 );
	}

	while ( !file.eof() ) {
		file.getline( line, 199 );
		// first, strip off comments
		if ( line[0] == '#' )
			continue;
		else if ( !strcmp( line, "" ) )
			continue;
		else {
			strcpy( line_back, line ); // strtok destroys line.
			token = strtok( line, wspace);
			if ( !strcmp( token, "v" ) ) {
				x = atof( strtok( NULL, wspace ) );
				y = atof( strtok( NULL, wspace ) );
				z = atof( strtok( NULL, wspace ) );
				verts.push_back( Vertex( x, y, z ) );
			}
			else if ( !strcmp( token, "vn" ) ) {
				x = atof( strtok( NULL, wspace ) );
				y = atof( strtok( NULL, wspace ) );
				z = atof( strtok( NULL, wspace ) );
				normals.push_back( Normal( x, y, z ) );
			}
			else if ( !strcmp( token, "vt" ) ) {
				tex_u = atof( strtok( NULL, wspace ) );
				tex_v = atof( strtok( NULL, wspace ) );
				texCoords.push_back( TexCoord(tex_u, tex_v ) );
			}
			else if ( !strcmp( token, "f" ) ) {
				int vi = (int)vIndex.size();
				faces.push_back( Face( vi ) );
				Face& curFace = faces.back();
				for (char *p = strtok( NULL, wspace ); p; p = strtok( NULL, wspace ) ) {
					indices[0] = 0; 
					indices[1] = 0;
					indices[2] = 0;
					char* pos = p;
					int len = (int)strlen(p);
					for ( int j=0, i=0;  j <= len && i < 3; j++ ) {
						if ( p[j] == '/' || p[j] == 0) {
							p[j] = 0;
							indices[i++] = atoi( pos );
							pos = p + j+1;
						}
					}

					vIndex.push_back( indices[0] - 1 );
					tIndex.push_back( indices[1] - 1 );
					nIndex.push_back( indices[2] - 1 );                        
					curFace.vCount++;

					if( indices[2] != 0 )
						isFlat = false;
				}
				if( curFace.vCount > 2 ){
					curFace.normal = faceNormal( verts[vIndex[vi]], 
						verts[vIndex[vi+1]], verts[vIndex[vi+2]] );
				}
			}
			else if ( !strcmp( token, "g" ) ) {      // group
			}
			else if ( !strcmp( token, "s" ) ) {      // smoothing group
			}
			else if ( !strcmp( token, "u" ) ) {      // material line
			}
			else if ( !strcmp( token, "" ) ) {       // blank line
			}
			else {
				cout <<line_back <<endl;
			}
		}
	}
	computeExtents();
}


//------------------------------------------------------------------------------
// face normal 을 계산
// Compute face normal
Normal WaveFrontOBJ::faceNormal(Vertex& v0, Vertex& v1, Vertex& v2) {
	double ax, ay, az;
	double bx, by, bz;
	double nx, ny, nz;

	ax = v1.x - v0.x;
	ay = v1.y - v0.y;
	az = v1.z - v0.z;

	bx = v2.x - v0.x;
	by = v2.y - v0.y;
	bz = v2.z - v0.z;

	nx = ay*bz - az*by;
	ny = az*bx - ax*bz;
	nz = ax*by - ay*bx;

	return Normal(nx, ny, nz);
}

//------------------------------------------------------------------------------
// OpenGL API 를 사용해서 파일에서 읽어둔 object 를 그린다.
// Draw object which is read from file
void WaveFrontOBJ::Draw() {
	int f, v, i;      
	for (f = 0; f < (int)faces.size(); f++) {
		Face& curFace = faces[f];        
		glBegin(mode);
		for (v = 0; v < curFace.vCount; v++) {
			int vi = curFace.vIndexStart + v;
			if (isFlat) {
				if (v == 0) { 
					glNormal3d( curFace.normal.x, curFace.normal.y, curFace.normal.z);
				}
			}
			else if ((i = nIndex[vi]) >= 0) {
				glNormal3d(normals[i].x, normals[i].y, normals[i].z);
			}
			if ((i = tIndex[vi]) >= 0) {
				glTexCoord2d(texCoords[i].u, texCoords[i].v);
			}
			if ((i = vIndex[vi]) >= 0) {
				glVertex3d(verts[i].x, verts[i].y, verts[i].z);
			}
		}
		glEnd();
	}
}

//------------------------------------------------------------------------------
void WaveFrontOBJ::computeExtents()
{
	if( verts.size() > 0 )
	{
		bbmin.x = verts[0].x;
		bbmin.y = verts[0].y;
		bbmin.z = verts[0].z;
		bbmax.x = verts[0].x;
		bbmax.y = verts[0].y;
		bbmax.z = verts[0].z;
		for( int i=1; i < (int)verts.size(); i++ )
		{
			if( verts[i].x < bbmin.x ) bbmin.x = verts[i].x;
			if( verts[i].y < bbmin.y ) bbmin.y = verts[i].y;
			if( verts[i].z < bbmin.z ) bbmin.z = verts[i].z;
			if( verts[i].x > bbmax.x ) bbmax.x = verts[i].x;
			if( verts[i].y > bbmax.y ) bbmax.y = verts[i].y;
			if( verts[i].z > bbmax.z ) bbmax.z = verts[i].z;
		}
	}
}
