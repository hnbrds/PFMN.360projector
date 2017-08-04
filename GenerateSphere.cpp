#include <stdio.h>
#include <math.h>

typedef struct{
  float x;
  float y;
  float z;
}  Point;

float DEGS_TO_RAD = 3.14159f/180.0f;
int numVertices = 0;    // Tallies the number of vertex points added.

//------------------------
//-- Prints a sphere as a "standard sphere" triangular mesh with the specified
//-- number of latitude (nLatitude) and longitude (nLongitude) lines and
//-- writes results to the specified output file (fout).

void printStandardSphere(Point pt, float radius, int nLatitude, int nLongitude, FILE *fout)
{
  int p, s, i, j;
  float x, y, z, out;
  int nPitch = nLongitude + 1;

  float pitchInc = (180. / (float)nPitch) * DEGS_TO_RAD;
  float rotInc   = (360. / (float)nLatitude) * DEGS_TO_RAD;

  //## PRINT VERTICES:

  fprintf(fout,"v %g %g %g\n", pt.x, pt.y+radius, pt.z);    // Top vertex.
  fprintf(fout,"v %g %g %g\n", pt.x, pt.y-radius, pt.z);    // Bottom vertex.
  numVertices = numVertices+2;

  int fVert = numVertices;    // Record the first vertex index for intermediate vertices.
  for(p=1; p<nPitch; p++)     // Generate all "intermediate vertices":
  {
    out = radius * sin((float)p * pitchInc);
    if(out < 0) out = -out;    // abs() command won't work with all compilers
    y   = radius * cos(p * pitchInc);
    printf("OUT = %g\n", out);    // bottom vertex
    printf("nPitch = %d\n", nPitch);    // bottom vertex
    for(s=0; s<nLatitude; s++)
    {
      x = out * cos(s * rotInc);
      z = out * sin(s * rotInc);

      fprintf(fout,"v %g %g %g\n", x+pt.x, y+pt.y, z+pt.z);
      numVertices++;
    }
  }

  //## PRINT SQUARE FACES BETWEEN INTERMEDIATE POINTS:

  for(p=1; p<nPitch-1; p++) {
    for(s=0; s<nLatitude; s++) {
      i = p*nLatitude + s;
      j = (s==nLatitude-1) ? i-nLatitude : i;
      fprintf(fout,"f %d %d %d %d\n",
              (i+1-nLatitude)+fVert, (j+2-nLatitude)+fVert, (j+2)+fVert, (i+1)+fVert);
    }
  }

  //## PRINT TRIANGLE FACES CONNECTING TO TOP AND BOTTOM VERTEX:

  int offLastVerts  = fVert + (nLatitude * (nLongitude-1));
  for(s=0; s<nLatitude; s++)
  {
    j = (s==nLatitude-1) ? -1 : s;
    fprintf(fout,"f %d %d %d\n", fVert-1, (j+2)+fVert,        (s+1)+fVert       );
    fprintf(fout,"f %d %d %d\n", fVert,   (s+1)+offLastVerts, (j+2)+offLastVerts);
  }
}




//------------------------
//-- Entry point. This main() function demonstrates how you can
//-- use "printStandardSphere()", but you probably won't
//-- want/need to copy it in your own code.

int main(int argc, char *argv[])
{
  int nLatitude  = 160;                  // Number vertical lines.
  //int nLongitude = nLatitude / 2;      // Number horizontal lines.
  int nLongitude = 90;
  // NOTE: for a good sphere use ~half the number of longitude lines than latitude.
  float radius = 203;
  Point centerPt;            // Position the center of out sphere at (0,0,0).

  if (argc < 2) {
    fprintf(stderr, "Must enter: './programname outputfile.obj'\n");
    return (-1);
  }

  FILE *fout = fopen(argv[1] , "w");
  if (fout == NULL) {
     printf("Couldn't open output file %s.\n", argv[1]);
     return (-1);
  }
  printStandardSphere(centerPt, radius, nLatitude, nLongitude, fout);      // Print sphere with radius 10 into file.
  fclose(fout);
  fprintf(stdout, "  # vertices:   %d\n", numVertices);
  return (0);
}
