#ifndef INTERVAL_H_
#define INTERVAL_H_
#include "math_macro.h"
#pragma once
class interval
{
  private:
    double m_start, m_end;
    
    // negation
    friend interval operator-( interval const& );

    // addtion
    friend interval& operator+=( interval&, double );
    friend interval  operator+( double, interval const& );
    friend interval  operator+( interval const&, double );

    // subtraction
    friend interval& operator-=( interval&, double );
    friend interval  operator-( interval const&, double );

    // multiplication by scalar
    friend interval& operator*=( interval&, double );
    friend interval  operator*( interval const&, double );
    friend interval  operator*( double, interval const& );

    // division by scalar
    friend interval& operator/=( interval&, double );
    friend interval  operator/( interval const&, double );

    // inclusion operation
    friend int   operator>>( interval const&, double );
    friend int   operator<<( double, interval const& );

    // inclusion operation
    friend int   operator>>( interval const&, interval const& );
    friend int   operator<<( interval const&, interval const& );

    // or operation
    friend interval& operator|=( interval&, interval const& );
    friend interval  operator|( interval const&, interval const& );

    // and operation (calc intersection between two intervals)
    friend interval& operator&=( interval&, interval const& );
    friend interval  operator&( interval const&, interval const& );

    // logical-and operation
    friend int       operator&&( interval const&, interval const& );

    // expansion
    friend interval  operator^( interval const&, double );

    // axiliary functions
    friend void adjust_interval( interval& );

    // stream
//    friend ostream& operator<<( ostream&, interval const& );
  //  friend istream& operator>>( istream&, interval& );

  public:
    // constructors
    interval( ) { m_start = m_end = 0.0; }
    interval( double a ) { m_start = m_end = a; }
    interval( double a, double b ) { m_start = MIN(a,b);
                                     m_end   = MAX(a,b); }

	void setValue(double a, double b) 	{ m_start = MIN(a,b); m_end   = MAX(a,b); }
    // inquiry functions
    double start_pt() const { return m_start ; }
    double end_pt() const { return m_end; }
	double& start()		{ return this->m_start ;}
	double& end()		{ return this->m_end;}
	double& left()		{ return m_start ;}
	double& right()		{ return m_end;}

	bool isInside(double value) const;
    double mid_pt() const { return (m_start +m_end)/2.0f; }
	double rand_pt() const;	// sampled from uniform distribution

    double interpolate( double t ) const { return (1-t)*m_start  + t*m_end; }
	double uninterpolate( double value) const;	//!< find t such that interpolate(t)==value
	
	double len( ) const { return m_end - m_start ; }

	double distance( double ) const;
    double project( double ) const;

	void scale(double s);	//!< scale the range from the mid point.
	void enlarge(double point);	//!< enlarge interval so that it include the point
	void expand(double epsilon);	//!< min=min-epsilon, max=max+epsilon
	
};
#endif
