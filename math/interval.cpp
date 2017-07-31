#include "mathclass.h"
#include "interval.h"

void interval::enlarge(double point)
{
	//!< enlarge interval so that it include the point
	m_start=MIN(m_start, point);
	m_end=MAX(m_end, point);
}

void interval::expand(double epsilon)	//!< min=min-epsilon, max=max+epsilon
{
	m_start-=epsilon;
	m_end+=epsilon;
}

bool interval::isInside(double value) const
{
	if(value<m_start || value>m_end)
		return false;
	return true;
}

/*
ostream& operator<<( ostream& os, interval const& a )
{
    os << "[ " << a.m_start << " , " << a.end << " ]";
    return os;
}

istream& operator>>( istream& is, interval& a )
{
	static char	buf[256];
    //is >> "[" >> a.m_start >> "," >> a.end >> "]";
	is >> buf >> a.m_start >> buf >> a.end >> buf;
    return is;
}
*/
interval operator-( interval const& a )
{
    return interval( -a.m_end, -a.m_start );
}

interval& operator+=( interval& a, double b )
{
    a.m_start += b;
    a.m_end   += b;

    return a;
}

interval operator+( interval const& a, double b )
{
    return interval( a.m_start + b, a.m_end + b );
}

interval operator+( double b, interval const& a )
{
    return interval( a.m_start + b, a.m_end + b );
}

interval& operator-=( interval& a, double b )
{
    a.m_start -= b;
    a.m_end   -= b;

    return a;
}

interval operator-( interval const& a, double b )
{
    return interval( a.m_start - b, a.m_end - b );
}

interval& operator*=( interval& a, double b )
{
    a.m_start *= b;
    a.m_end   *= b;
    adjust_interval( a );

    return a;
}

interval operator*( interval const& a, double b )
{
    interval c( a.m_start * b, a.m_end * b );
    adjust_interval( c );

    return c;
}

interval operator*( double b, interval const& a )
{
    interval c( a.m_start * b, a.m_end * b );
    adjust_interval( c );

    return c;
}

interval& operator/=( interval& a, double b )
{
    a.m_start /= b;
    a.m_end   /= b;
    adjust_interval( a );

    return a;
}

interval operator/( interval const& a, double b )
{
    interval c( a.m_start / b, a.m_end / b );
    adjust_interval( c );

    return c;
}

interval& operator|=( interval& a, interval const& b )
{
    a.m_start = MIN( a.m_start, b.m_start );
    a.m_end   = MAX( a.m_end  , b.m_end   );

    return a;
}

interval operator|( interval const& a, interval const& b )
{
    return interval( MIN( a.m_start, b.m_start ), MAX( a.m_end, b.m_end ) );
}

int operator>>( interval const& a, double b )
{
    if ( a.m_start<b+EPS && b<a.m_end+EPS ) return TRUE;
                                   else return FALSE;
}

int operator<<( double b, interval const& a )
{
    if ( a.m_start<b+EPS && b<a.m_end+EPS ) return TRUE;
                                   else return FALSE;
}

int operator>>( interval const& a, interval const& b )
{
    if ( a.m_start<b.m_start+EPS && b.m_end<a.m_end+EPS ) return TRUE;
                                             else return FALSE;
}

int operator<<( interval const& b, interval const& a )
{
    if ( a.m_start<b.m_start+EPS && b.m_end<a.m_end+EPS ) return TRUE;
                                             else return FALSE;
}

interval& operator&=( interval& a, interval const& b )
{
    if ( a<<b ) ;
    else if ( b<<a ) { a = b; }
    else if ( b.m_end<a.m_start ) { a *= 0.0; }
    else if ( a.m_end<b.m_start ) { a *= 0.0; }
    else if ( a.m_start<b.m_start ) { a.m_start = b.m_start; }
    else { a.m_end = b.m_end; }
    return a;
}

interval operator&( interval const& a, interval const& b )
{
    interval c( 0.0 );

    if ( a<<b ) { c = a; }
    else if ( b<<a ) { c = b; }
    else if ( b.m_end<a.m_start ) { c *= 0.0; }
    else if ( a.m_end<b.m_start ) { c *= 0.0; }
    else if ( a.m_start<b.m_start ) { c.m_start = b.m_start; c.m_end = a.m_end; }
    else { c.m_start = a.m_start; c.m_end = b.m_end; }
                               
    return c;
}

int operator&&( interval const& a, interval const& b )
{
    if ( a.m_start-EPS > b.m_end || a.m_end+EPS < b.m_start ) return FALSE;
                                                 else return TRUE;
}

interval operator^( interval const& a, double b )
{
    return interval( a.m_start - b, a.m_end + b );
}

void adjust_interval( interval& a )
{
    if ( a.m_start > a.m_end )
    {
        double temp = a.m_start;
        a.m_start = a.m_end;
        a.m_end = temp;
    }
}

double
interval::project( double d ) const
{
    if ( this->m_start > d ) return this->m_start;
    if ( this->m_end < d ) return this->m_end;
    return d;
}

double
interval::distance( double d ) const
{
    if ( this->m_start > d ) return this->m_start - d;
    if ( this->m_end < d ) return d - this->m_end;
    return 0;
}

double interval::uninterpolate( double input) const
{
	return (input-m_start)/(m_end-m_start);
}



void interval::scale(double s)	//!< scale the range from the mid point.
{
	double mid=(m_start+m_end)/2.0;

	m_start=mid+(m_start-mid)*s;
	m_end=mid+(m_end-mid)*s;
}


double interval::rand_pt() const	// sampled from uniform distribution
{
	return ((double)rand()/(double)RAND_MAX)*(m_end-m_start)+m_start;
}
