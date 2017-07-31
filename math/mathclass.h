#ifndef _MATHCLASS_H_
#define _MATHCLASS_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <math.h>
#include <assert.h>

#include <limits>

#ifdef _DEBUG
#define ASSERT(x) assert(x)
#define RANGE_ASSERT(x) assert(x)
#define VERIFY(x) assert(x)
#define TRACE	printf
#else 
#define ASSERT(x) 
#define RANGE_ASSERT(x) 
#define VERIFY(x)	(x)
#define TRACE	__noop
#endif
#ifndef	M_PI
#define	M_PI	3.14159265358979323846
#endif

#define EPS (1.0e-10)
inline double IsZero(double f) { return ((f) < EPS && (f) >-EPS);}


#include <limits.h>
#include <float.h>

#include "quater.h"
#include "vector3.h"
#include "matrix4.h"

// include only when needed.
//#include "traits.h"
//#include "vector_n.h"
//#include "bitVectorN.h"
//#include "matrix_n.h"
//#include "complex.h"
//#include "Metric.h"
//#include "hyperMatrixN.h"
//#include "intervalN.h"
//#include "DynamicTimeWarping.h"// include only when needed.
//#include "smatrixN.h"// include only when needed.
//#include "optimize.h"// include only when needed.
//#include "Operator.h"	// include only when needed.
//#include "quaterN.h" // include only when needed.
//#include "vector3N.h"// include only when needed.
//#include "BSpline.h" 
//#include "cmplxVectorN.h"
#endif
