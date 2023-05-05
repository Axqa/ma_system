#include "utilities.h"
#include <math.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <string.h>
#include <cmath>

PredT PREDICT_TYPE = PRED_LS_WITH_TEAM;
int    HISTORY_SIZE = 3;
VecPosition AGENT_POS;
MapT MAP_TYPE = MAP_MINSUM_PRED;

using namespace std;

/*! This function returns the sign of a give double.
    1 is positive, -1 is negative
    \param d1 first parameter
    \return the sign of this double */
int sign( double d1 )
{
  return (d1>0)?1:-1;
}

/*! This function returns the maximum of two given doubles.
    \param d1 first parameter
    \param d2 second parameter
    \return the maximum of these two parameters */
double max( double d1, double d2 )
{
  return (d1>d2)?d1:d2;
}

/*! This function returns the minimum of two given doubles.
    \param d1 first parameter
    \param d2 second parameter
    \return the minimum of these two parameters */
double min( double d1, double d2 )
{
  return (d1<d2)?d1:d2;
}


/*! This function converts an angle in radians to the corresponding angle in
    degrees.
    \param x an angle in radians
    \return the corresponding angle in degrees */
AngDeg Rad2Deg( AngRad x )
{
  return ( x * 180 / M_PI );
}

/*! This function converts an angle in degrees to the corresponding angle in
    radians.
    \param x an angle in degrees
    \return the corresponding angle in radians */
AngRad Deg2Rad( AngDeg x )
{
  return ( x * M_PI / 180 );
}

/*! This function returns the cosine of a given angle in degrees using the
    built-in cosine function that works with angles in radians.
    \param x an angle in degrees
    \return the cosine of the given angle */
double cosDeg( AngDeg x )
{
  return ( cos( Deg2Rad( x ) ) );
}

/*! This function returns the sine of a given angle in degrees using the
    built-in sine function that works with angles in radians.
    \param x an angle in degrees
    \return the sine of the given angle */
double sinDeg( AngDeg x )
{
  return ( sin( Deg2Rad( x ) ) );
}

/*! This function returns the tangent of a given angle in degrees using the
    built-in tangent function that works with angles in radians.
    \param x an angle in degrees
    \return the tangent of the given angle */
double tanDeg( AngDeg x )
{
  return ( tan( Deg2Rad( x ) ) );
}

/*! This function returns the principal value of the arc tangent of x
    in degrees using the built-in arc tangent function which returns
    this value in radians.
    \param x a double value
    \return the arc tangent of the given value in degrees */
AngDeg atanDeg( double x )
{
  return ( Rad2Deg( atan( x ) ) );
}

/*! This function returns the principal value of the arc tangent of y/x in
    degrees using the signs of both arguments to determine the quadrant of the
    return value. For this the built-in 'atan2' function is used which returns
    this value in radians.
    \param x a double value
    \param y a double value
    \return the arc tangent of y/x in degrees taking the signs of x and y into
    account */
double atan2Deg( double x, double y )
{
  if( fabs( x ) < EPSILON && fabs( y ) < EPSILON )
    return ( 0.0 );

  return ( Rad2Deg( atan2( x, y ) ) );
}

/*! This function returns the principal value of the arc cosine of x in degrees
    using the built-in arc cosine function which returns this value in radians.
    \param x a double value
    \return the arc cosine of the given value in degrees */
AngDeg acosDeg( double x )
{
  if( x >= 1 )
    return ( 0.0 );
  else if( x <= -1 )
    return ( 180.0 );

  return ( Rad2Deg( acos( x ) ) );
}

/*! This function returns the principal value of the arc sine of x in degrees
    using the built-in arc sine function which returns this value in radians.
    \param x a double value
    \return the arc sine of the given value in degrees */
AngDeg asinDeg( double x )
{
  if( x >= 1 )
    return ( 90.0 );
  else if ( x <= -1 )
    return ( -90.0 );

  return ( Rad2Deg( asin( x ) ) );
}

/*! This function returns a boolean value which indicates whether the value
   'ang' (from interval [-180..180] lies in the interval [angMin..angMax].
    Examples: isAngInInterval( -100, 4, -150) returns false
             isAngInInterval(   45, 4, -150) returns true
    \param ang angle that should be checked
    \param angMin minimum angle in interval
    \param angMax maximum angle in interval
    \return boolean indicating whether ang lies in [angMin..angMax] */
bool isAngInInterval( AngDeg ang, AngDeg angMin, AngDeg angMax )
{
  // convert all angles to interval 0..360
  if( ( ang    + 360 ) < 360 ) ang    += 360;
  if( ( angMin + 360 ) < 360 ) angMin += 360;
  if( ( angMax + 360 ) < 360 ) angMax += 360;

  if( angMin < angMax ) // 0 ---false-- angMin ---true-----angMax---false--360
    return angMin < ang && ang < angMax ;
  else                  // 0 ---true--- angMax ---false----angMin---true---360
    return !( angMax < ang && ang < angMin );
}

/*! This method returns the bisector (average) of two angles. It deals
    with the boundary problem, thus when 'angMin' equals 170 and 'angMax'
    equals -100, -145 is returned.
    \param angMin minimum angle [-180,180]
    \param angMax maximum angle [-180,180]
    \return average of angMin and angMax. */
AngDeg getBisectorTwoAngles( AngDeg angMin, AngDeg angMax )
{
  // separate sine and cosine part to circumvent boundary problem
  return VecPosition::normalizeAngle(
            atan2Deg( (sinDeg( angMin) + sinDeg( angMax ) )/2.0,
                      (cosDeg( angMin) + cosDeg( angMax ) )/2.0 ) );
}


/*! Constructor for the VecPosition class. When the supplied
    Coordinate System type equals CARTESIAN, the arguments x and y
    denote the x- and y-coordinates of the new position. When it
    equals POLAR however, the arguments x and y denote the polar
    coordinates of the new position; in this case x is thus equal to
    the distance r from the origin and y is equal to the angle phi
    that the polar vector makes with the x-axis.
    \param x the x-coordinate of the new position when cs == CARTESIAN; the
    distance of the new position from the origin when cs = POLAR
    \param y the y-coordinate of the new position when cs = CARTESIAN; the
    angle that the polar vector makes with the x-axis when cs = POLAR
    \param cs a CoordSystemT indicating whether x and y denote cartesian
    coordinates or polar coordinates
    \return the VecPosition corresponding to the given arguments */
VecPosition::VecPosition( double x, double y, CoordSystemT cs )
{
  setVecPosition( x, y, cs );
}

/*! Overloaded version of unary minus operator for VecPositions. It returns the
    negative VecPosition, i.e. both the x- and y-coordinates are multiplied by
    -1. The current VecPosition itself is left unchanged.
    \return a negated version of the current VecPosition */
VecPosition VecPosition::operator - ( )
{
  return ( VecPosition( -m_x, -m_y ) );
}

/*! Overloaded version of the binary plus operator for adding a given double
    value to a VecPosition. The double value is added to both the x- and
    y-coordinates of the current VecPosition. The current VecPosition itself is
    left unchanged.
    \param d a double value which has to be added to both the x- and
    y-coordinates of the current VecPosition
    \return the result of adding the given double value to the current
    VecPosition */
VecPosition VecPosition::operator + ( const double &d )
{
  return ( VecPosition( m_x + d, m_y + d ) );
}

/*! Overloaded version of the binary plus operator for VecPositions. It returns
    the sum of the current VecPosition and the given VecPosition by adding their
    x- and y-coordinates. The VecPositions themselves are left unchanged.
    \param p a VecPosition
    \return the sum of the current VecPosition and the given VecPosition */
VecPosition VecPosition::operator + ( const VecPosition &p )
{
  return ( VecPosition( m_x + p.m_x, m_y + p.m_y ) );
}

/*! Overloaded version of the binary minus operator for subtracting a
    given double value from a VecPosition. The double value is
    subtracted from both the x- and y-coordinates of the current
    VecPosition. The current VecPosition itself is left unchanged.
    \param d a double value which has to be subtracted from both the x- and
    y-coordinates of the current VecPosition
    \return the result of subtracting the given double value from the current
    VecPosition */
VecPosition VecPosition::operator - ( const double &d )
{
  return ( VecPosition( m_x - d, m_y - d ) );
}

/*! Overloaded version of the binary minus operator for
    VecPositions. It returns the difference between the current
    VecPosition and the given VecPosition by subtracting their x- and
    y-coordinates. The VecPositions themselves are left unchanged.

    \param p a VecPosition
    \return the difference between the current VecPosition and the given
    VecPosition */
VecPosition VecPosition::operator - ( const VecPosition &p )
{
  return ( VecPosition( m_x - p.m_x, m_y - p.m_y ) );
}

/*! Overloaded version of the multiplication operator for multiplying a
    VecPosition by a given double value. Both the x- and y-coordinates of the
    current VecPosition are multiplied by this value. The current VecPosition
    itself is left unchanged.
    \param d the multiplication factor
    \return the result of multiplying the current VecPosition by the given
    double value */
VecPosition VecPosition::operator * ( const double &d  )
{
  return ( VecPosition( m_x * d, m_y * d  ) );
}

/*! Overloaded version of the multiplication operator for
    VecPositions. It returns the product of the current VecPosition
    and the given VecPosition by multiplying their x- and
    y-coordinates. The VecPositions themselves are left unchanged.

    \param p a VecPosition
    \return the product of the current VecPosition and the given VecPosition */
VecPosition VecPosition::operator * ( const VecPosition &p )
{
  return ( VecPosition( m_x * p.m_x, m_y * p.m_y ) );
}

/*! Overloaded version of the division operator for dividing a
    VecPosition by a given double value. Both the x- and y-coordinates
    of the current VecPosition are divided by this value. The current
    VecPosition itself is left unchanged.

    \param d the division factor
    \return the result of dividing the current VecPosition by the given double
    value */
VecPosition VecPosition::operator / ( const double &d )
{
  return ( VecPosition( m_x / d, m_y / d  ) );
}

/*! Overloaded version of the division operator for VecPositions. It
    returns the quotient of the current VecPosition and the given
    VecPosition by dividing their x- and y-coordinates. The
    VecPositions themselves are left unchanged.

    \param p a VecPosition
    \return the quotient of the current VecPosition and the given one */
VecPosition VecPosition::operator / ( const VecPosition &p )
{
  return ( VecPosition( m_x / p.m_x, m_y / p.m_y ) );
}

/*! Overloaded version of the assignment operator for assigning a given double
    value to both the x- and y-coordinates of the current VecPosition. This
    changes the current VecPosition itself.
    \param d a double value which has to be assigned to both the x- and
    y-coordinates of the current VecPosition */
void VecPosition::operator = ( const double &d )
{
  m_x = d;
  m_y = d;
}

/*! Overloaded version of the sum-assignment operator for VecPositions. It
    returns the sum of the current VecPosition and the given VecPosition by
    adding their x- and y-coordinates. This changes the current VecPosition
    itself.
    \param p a VecPosition which has to be added to the current VecPosition */
void VecPosition::operator +=( const VecPosition &p )
{
  m_x += p.m_x;
  m_y += p.m_y;
}

/*! Overloaded version of the sum-assignment operator for adding a given double
    value to a VecPosition. The double value is added to both the x- and
    y-coordinates of the current VecPosition. This changes the current
    VecPosition itself.
    \param d a double value which has to be added to both the x- and
    y-coordinates of the current VecPosition */
void VecPosition::operator += ( const double &d )
{
  m_x += d;
  m_y += d;
}

/*! Overloaded version of the difference-assignment operator for
    VecPositions.  It returns the difference between the current
    VecPosition and the given VecPosition by subtracting their x- and
    y-coordinates. This changes the current VecPosition itself.

    \param p a VecPosition which has to be subtracted from the current
    VecPosition */
void VecPosition::operator -=( const VecPosition &p )
{
  m_x -= p.m_x;
  m_y -= p.m_y;
}

/*! Overloaded version of the difference-assignment operator for
    subtracting a given double value from a VecPosition. The double
    value is subtracted from both the x- and y-coordinates of the
    current VecPosition. This changes the current VecPosition itself.

    \param d a double value which has to be subtracted from both the x- and
    y-coordinates of the current VecPosition */
void VecPosition::operator -=( const double &d )
{
  m_x -= d;
  m_y -= d;
}

/*! Overloaded version of the multiplication-assignment operator for
    VecPositions. It returns the product of the current VecPosition
    and the given VecPosition by multiplying their x- and
    y-coordinates. This changes the current VecPosition itself.

    \param p a VecPosition by which the current VecPosition has to be
    multiplied */
void VecPosition::operator *=( const VecPosition &p )
{
  m_x *= p.m_x;
  m_y *= p.m_y;
}

/*! Overloaded version of the multiplication-assignment operator for
    multiplying a VecPosition by a given double value. Both the x- and
    y-coordinates of the current VecPosition are multiplied by this
    value. This changes the current VecPosition itself.

    \param d a double value by which both the x- and y-coordinates of the
    current VecPosition have to be multiplied */
void VecPosition::operator *=( const double &d )
{
  m_x *= d;
  m_y *= d;
}

/*! Overloaded version of the division-assignment operator for
    VecPositions. It returns the quotient of the current VecPosition
    and the given VecPosition by dividing their x- and
    y-coordinates. This changes the current VecPosition itself.

    \param p a VecPosition by which the current VecPosition is divided */
void VecPosition::operator /=( const VecPosition &p )
{
  m_x /= p.m_x;
  m_y /= p.m_y;
}

/*! Overloaded version of the division-assignment operator for
    dividing a VecPosition by a given double value. Both the x- and
    y-coordinates of the current VecPosition are divided by this
    value. This changes the current VecPosition itself.

    \param d a double value by which both the x- and y-coordinates of the
    current VecPosition have to be divided */
void VecPosition::operator /=( const double &d )
{
  m_x /= d;
  m_y /= d;
}

/*! Overloaded version of the inequality operator for VecPositions. It
    determines whether the current VecPosition is unequal to the given
    VecPosition by comparing their x- and y-coordinates.

    \param p a VecPosition
    \return true when either the x- or y-coordinates of the given VecPosition
    and the current VecPosition are different; false otherwise */
bool VecPosition::operator !=( const VecPosition &p )
{
  return ( ( m_x != p.m_x ) || ( m_y != p.m_y ) );
}

/*! Overloaded version of the inequality operator for comparing a
    VecPosition to a double value. It determines whether either the x-
    or y-coordinate of the current VecPosition is unequal to the given
    double value.

    \param d a double value with which both the x- and y-coordinates of the
    current VecPosition have to be compared.
    \return true when either the x- or y-coordinate of the current VecPosition
    is unequal to the given double value; false otherwise */
bool VecPosition::operator !=( const double &d )
{
  return ( ( m_x != d ) || ( m_y != d ) );
}

/*! Overloaded version of the equality operator for VecPositions. It
    determines whether the current VecPosition is equal to the given
    VecPosition by comparing their x- and y-coordinates.

    \param p a VecPosition
    \return true when both the x- and y-coordinates of the given
    VecPosition and the current VecPosition are equal; false
    otherwise */
bool VecPosition::operator ==( const VecPosition &p )
{
  return ( ( m_x == p.m_x ) && ( m_y == p.m_y ) );
}

/*! Overloaded version of the equality operator for comparing a
    VecPosition to a double value. It determines whether both the x-
    and y-coordinates of the current VecPosition are equal to the
    given double value.

    \param d a double value with which both the x- and y-coordinates of the
    current VecPosition have to be compared.
    \return true when both the x- and y-coordinates of the current VecPosition
    are equal to the given double value; false otherwise */
bool VecPosition::operator ==( const double &d )
{
  return ( ( m_x == d ) && ( m_y == d ) );
}

/*! Overloaded version of the C++ output operator for
    VecPositions. This operator makes it possible to use VecPositions
    in output statements (e.g.  cout << v). The x- and y-coordinates
    of the VecPosition are printed in the format (x,y).

    \param os output stream to which information should be written
    \param v a VecPosition which must be printed
    \return output stream containing (x,y) */
ostream& operator <<( ostream &os, VecPosition v )
{
  return ( os << "( " << v.m_x << ", " << v.m_y << " )" );
}

/*! This method writes the current VecPosition to standard output. It
    can also print a polar representation of the current VecPosition.

    \param cs a CoordSystemtT indicating whether a POLAR or CARTESIAN
     representation of the current VecPosition should be printed */
void VecPosition::show( CoordSystemT cs )
{
  if( cs == CARTESIAN )
    cout << *this << endl;
  else
    cout << "( r: " << getMagnitude( ) << ", phi: " << getDirection( ) << "  )";
}

/*! This method writes the current VecPosition to a string. It can
    also write a polar representation of the current VecPosition.

    \param cs a CoordSystemtT indicating whether a POLAR or CARTESIAN
     representation of the current VecPosition should be written
    \return a string containing a polar or Cartesian representation of the
    current VecPosition depending on the value of the boolean argument */
string VecPosition::str( CoordSystemT cs )
{
  char buf[ 1024 ];

  if( cs == CARTESIAN )
    sprintf( buf, "( %f, %f )", getX( ), getY( ) );
  else
    sprintf( buf, "( r: %f, phi: %f )", getMagnitude( ), getDirection( ) );

  string str( buf );
  return ( str );
}

/*! Set method for the x-coordinate of the current VecPosition.

    \param dX a double value representing a new x-coordinate
    \return a boolean indicating whether the update was successful */
bool VecPosition::setX( double dX )
{
  m_x = dX;
  return ( true );
}

/*! Get method for the x-coordinate of the current VecPosition.

    \return the x-coordinate of the current VecPosition */
double VecPosition::getX( ) const
{
  return ( m_x );
}

/*! Set method for the y-coordinate of the current VecPosition.

    \param dY a double value representing a new y-coordinate
    \return a boolean indicating whether the update was successful */
bool VecPosition::setY( double dY )
{
  m_y = dY;
  return ( true );
}

/*! Get method for the y-coordinate of the current VecPosition.

    \return the y-coordinate of the current VecPosition */
double VecPosition::getY( ) const
{
  return ( m_y );
}

/*! This method (re)sets the coordinates of the current
    VecPosition. The given coordinates can either be polar or
    Cartesian coordinates. This is indicated by the value of the third
    argument.

    \param dX a double value indicating either a new Cartesian
    x-coordinate when cs=CARTESIAN or a new polar r-coordinate
    (distance) when cs=POLAR

    \param dY a double value indicating either a new Cartesian
    y-coordinate when cs=CARTESIAN or a new polar phi-coordinate
    (angle) when cs=POLAR

    \param cs a CoordSystemT indicating whether x and y denote
    cartesian coordinates or polar coordinates */
void VecPosition::setVecPosition( double dX, double dY, CoordSystemT cs)
{
  if( cs == CARTESIAN )
  {
    m_x = dX;
    m_y = dY;
  }
  else
    *this = getVecPositionFromPolar( dX, dY );
}

/*! This method determines the distance between the current
    VecPosition and a given VecPosition. This is equal to the
    magnitude (length) of the vector connecting the two positions
    which is the difference vector between them.

    \param p a Vecposition
    \return the distance between the current VecPosition and the given
    VecPosition */
double VecPosition::getDistanceTo( const VecPosition p )
{
  return ( ( *this - p ).getMagnitude( ) );
}

/*! This method adjusts the coordinates of the current VecPosition in
    such a way that the magnitude of the corresponding vector equals
    the double value which is supplied as an argument. It thus scales
    the vector to a given length by multiplying both the x- and
    y-coordinates by the quotient of the argument and the current
    magnitude. This changes the VecPosition itself.

    \param d a double value representing a new magnitude

    \return the result of scaling the vector corresponding with the
    current VecPosition to the given magnitude thus yielding a
    different VecPosition */
VecPosition VecPosition::setMagnitude( double d )
{
  if( getMagnitude( ) > EPSILON )
     ( *this ) *= ( d / getMagnitude( ) );

  return ( *this );
}

/*! This method determines the magnitude (length) of the vector
    corresponding with the current VecPosition using the formula of
    Pythagoras.

    \return the length of the vector corresponding with the current
    VecPosition */
double VecPosition::getMagnitude( ) const
{
  return ( sqrt( m_x * m_x + m_y * m_y ) );
}

/*! This method determines the direction of the vector corresponding
    with the current VecPosition (the phi-coordinate in polar
    representation) using the arc tangent function. Note that the
    signs of x and y have to be taken into account in order to
    determine the correct quadrant.

    \return the direction in degrees of the vector corresponding with
    the current VecPosition */
AngDeg VecPosition::getDirection( ) const
{
  return ( atan2Deg( m_y, m_x ) );
}

/*! This method determines whether the current VecPosition is in front
    of a given VecPosition, i.e. whether the x-coordinate of the
    current VecPosition is larger than the x-coordinate of the given
    VecPosition.

    \param p a VecPosition to which the current VecPosition must be compared
    \return true when the current VecPosition is in front of the given
    VecPosition; false otherwise */
bool VecPosition::isInFrontOf( const VecPosition &p )
{
  return ( ( m_x > p.getX( ) ) ? true : false );
}

/*! This method determines whether the x-coordinate of the current
    VecPosition is in front of (i.e. larger than) a given double
    value.

    \param d a double value to which the current x-coordinate must be
    compared

    \return true when the current x-coordinate is in front of the
    given value; false otherwise */
bool VecPosition::isInFrontOf( const double &d )
{
  return ( ( m_x > d ) ? true : false );
}

/*! This method determines whether the current VecPosition is behind a
    given VecPosition, i.e. whether the x-coordinate of the current
    VecPosition is smaller than the x-coordinate of the given
    VecPosition.

    \param p a VecPosition to which the current VecPosition must be
    compared

    \return true when the current VecPosition is behind the given
    VecPosition; false otherwise */
bool VecPosition::isBehindOf( const VecPosition &p )
{
  return ( ( m_x < p.getX( ) ) ? true : false );
}

/*! This method determines whether the x-coordinate of the current
    VecPosition is behind (i.e. smaller than) a given double value.

    \param d a double value to which the current x-coordinate must be
    compared

    \return true when the current x-coordinate is behind the given
    value; false otherwise */
bool VecPosition::isBehindOf( const double &d )
{
  return ( ( m_x < d ) ? true : false );
}

/*! This method determines whether the current VecPosition is to the
    left of a given VecPosition, i.e. whether the y-coordinate of the
    current VecPosition is smaller than the y-coordinate of the given
    VecPosition.

    \param p a VecPosition to which the current VecPosition must be
    compared

    \return true when the current VecPosition is to the left of the
    given VecPosition; false otherwise */
bool VecPosition::isLeftOf( const VecPosition &p )
{
  return ( ( m_y < p.getY( ) ) ? true : false );
}

/*! This method determines whether the y-coordinate of the current
    VecPosition is to the left of (i.e. smaller than) a given double
    value.

    \param d a double value to which the current y-coordinate must be
    compared

    \return true when the current y-coordinate is to the left of the
    given value; false otherwise */
bool VecPosition::isLeftOf( const double &d )
{
  return ( ( m_y < d ) ? true : false );
}

/*! This method determines whether the current VecPosition is to the
    right of a given VecPosition, i.e. whether the y-coordinate of the
    current VecPosition is larger than the y-coordinate of the given
    VecPosition.

    \param p a VecPosition to which the current VecPosition must be
    compared

    \return true when the current VecPosition is to the right of the
    given VecPosition; false otherwise */
bool VecPosition::isRightOf( const VecPosition &p )
{
  return ( ( m_y > p.getY( ) ) ? true : false );
}

/*! This method determines whether the y-coordinate of the current
    VecPosition is to the right of (i.e. larger than) a given double
    value.

    \param d a double value to which the current y-coordinate must be
    compared

    \return true when the current y-coordinate is to the right of the
    given value; false otherwise */
bool VecPosition::isRightOf( const double &d )
{
  return ( ( m_y > d ) ? true : false );
}

/*! This method determines whether the current VecPosition is in
    between two given VecPositions when looking in the x-direction,
    i.e. whether the current VecPosition is in front of the first
    argument and behind the second.

    \param p1 a VecPosition to which the current VecPosition must be
    compared

    \param p2 a VecPosition to which the current VecPosition must be
    compared

    \return true when the current VecPosition is in between the two
    given VecPositions when looking in the x-direction; false
    otherwise */
bool VecPosition::isBetweenX( const VecPosition &p1, const VecPosition &p2 )
{
  return ( ( isInFrontOf( p1 ) && isBehindOf( p2 ) ) ? true : false );
}

/*! This method determines whether the x-coordinate of the current
    VecPosition is in between two given double values, i.e. whether
    the x-coordinate of the current VecPosition is in front of the
    first argument and behind the second.

    \param d1 a double value to which the current x-coordinate must be
    compared

    \param d2 a double value to which the current x-coordinate must be
    compared

    \return true when the current x-coordinate is in between the two
    given values; false otherwise */
bool VecPosition::isBetweenX( const double &d1, const double &d2 )
{
  return ( ( isInFrontOf( d1 ) && isBehindOf( d2 ) ) ? true : false );
}

/*! This method determines whether the current VecPosition is in
    between two given VecPositions when looking in the y-direction,
    i.e. whether the current VecPosition is to the right of the first
    argument and to the left of the second.

    \param p1 a VecPosition to which the current VecPosition must be
    compared

    \param p2 a VecPosition to which the current VecPosition must be
    compared

    \return true when the current VecPosition is in between the two
    given VecPositions when looking in the y-direction; false
    otherwise */
bool VecPosition::isBetweenY( const VecPosition &p1, const VecPosition &p2 )
{
  return ( ( isRightOf( p1 ) && isLeftOf( p2 ) ) ? true : false );
}

/*! This method determines whether the y-coordinate of the current
    VecPosition is in between two given double values, i.e. whether
    the y-coordinate of the current VecPosition is to the right of the
    first argument and to the left of the second.

    \param d1 a double value to which the current y-coordinate must be
    compared

    \param d2 a double value to which the current y-coordinate must be
    compared

    \return true when the current y-coordinate is in between the two
    given values; false otherwise */
bool VecPosition::isBetweenY( const double &d1, const double &d2 )
{
  return ( ( isRightOf( d1 ) && isLeftOf( d2 ) ) ? true : false );
}

/*! This method normalizes a VecPosition by setting the magnitude of
    the corresponding vector to 1. This thus changes the VecPosition
    itself.

    \return the result of normalizing the current VecPosition thus
    yielding a different VecPosition */
VecPosition VecPosition::normalize( )
{
  return ( setMagnitude( 1.0 ) );
}

/*! This method rotates the vector corresponding to the current
    VecPosition over a given angle thereby changing the current
    VecPosition itself. This is done by calculating the polar
    coordinates of the current VecPosition and adding the given angle
    to the phi-coordinate in the polar representation. The polar
    coordinates are then converted back to Cartesian coordinates to
    obtain the desired result.

    \param angle an angle in degrees over which the vector
    corresponding to the current VecPosition must be rotated

    \return the result of rotating the vector corresponding to the
    current VecPosition over the given angle thus yielding a different
    VecPosition */
VecPosition VecPosition::rotate( AngDeg angle )
{
  // determine the polar representation of the current VecPosition
  double dMag    = this->getMagnitude( );
  double dNewDir = this->getDirection( ) + angle;  // add rotation angle to phi
  setVecPosition( dMag, dNewDir, POLAR );          // convert back to Cartesian
  return ( *this );
}

/*! This method converts the coordinates of the current VecPosition
    (which are represented in an global coordinate system with the
    origin at (0,0)) into relative coordinates in a different
    coordinate system (e.g. relative to a player). The new coordinate
    system is defined by the arguments to the method. The relative
    coordinates are now obtained by aligning the relative coordinate
    system with the global coordinate system using a translation to
    make both origins coincide followed by a rotation to align the
    axes.

    \param origin the origin of the relative coordinate frame

    \param ang the angle between the world frame and the relative
    frame (reasoning from the world frame)

    \return the result of converting the current global VecPosition
    into a relative VecPosition */
VecPosition VecPosition::globalToRelative( VecPosition origin, AngDeg ang )
{
  // convert global coordinates into relative coordinates by aligning
  // relative frame and world frame. First perform translation to make
  // origins of both frames coincide. Then perform rotation to make
  // axes of both frames coincide (use negative angle since you rotate
  // relative frame to world frame).
  *this -= origin;
  return ( rotate( -ang ) );
}

/*! This method converts the coordinates of the current VecPosition
    (which are represented in a relative coordinate system) into
    global coordinates in the world frame (with origin at (0,0)). The
    relative coordinate system is defined by the arguments to the
    method. The global coordinates are now obtained by aligning the
    world frame with the relative frame using a rotation to align the
    axes followed by a translation to make both origins coincide.

    \param origin the origin of the relative coordinate frame

    \param ang the angle between the world frame and the relative
    frame (reasoning from the world frame)

    \return the result of converting the current relative VecPosition
    into an global VecPosition */
VecPosition VecPosition::relativeToGlobal( VecPosition origin, AngDeg ang )
{
  // convert relative coordinates into global coordinates by aligning
  // world frame and relative frame. First perform rotation to make
  // axes of both frames coincide (use positive angle since you rotate
  // world frame to relative frame). Then perform translation to make
  // origins of both frames coincide.
  rotate( ang );
  *this += origin;
  return ( *this );
}

/*! This method returns a VecPosition that lies somewhere on the
    vector between the current VecPosition and a given
    VecPosition. The desired position is specified by a given fraction
    of this vector (e.g. 0.5 means exactly in the middle of the
    vector). The current VecPosition itself is left unchanged.

    \param p a VecPosition which defines the vector to the current
    VecPosition

    \param dFrac double representing the fraction of the connecting
    vector at which the desired VecPosition lies.

    \return the VecPosition which lies at fraction dFrac on the vector
    connecting p and the current VecPosition */
VecPosition VecPosition::getVecPositionOnLineFraction( VecPosition &p,
                                                       double      dFrac )
{
  // determine point on line that lies at fraction dFrac of whole line
  // example: this --- 0.25 ---------  p
  // formula: this + dFrac * ( p - this ) = this - dFrac * this + dFrac * p =
  //          ( 1 - dFrac ) * this + dFrac * p
  return ( ( *this ) * ( 1.0 - dFrac ) + ( p * dFrac ) );
}

/*! This method converts a polar representation of a VecPosition into
    a Cartesian representation.

    \param dMag a double representing the polar r-coordinate, i.e. the
    distance from the point to the origin

    \param ang the angle that the polar vector makes with the x-axis,
    i.e. the polar phi-coordinate

    \return the result of converting the given polar representation
    into a Cartesian representation thus yielding a Cartesian
    VecPosition */
VecPosition VecPosition::getVecPositionFromPolar( double dMag, AngDeg ang )
{
  // cos(phi) = x/r <=> x = r*cos(phi); sin(phi) = y/r <=> y = r*sin(phi)
  return ( VecPosition( dMag * cosDeg( ang ), dMag * sinDeg( ang ) ) );
}

/*! This method normalizes an angle. This means that the resulting
    angle lies between -180 and 180 degrees.

    \param angle the angle which must be normalized

    \return the result of normalizing the given angle */
AngDeg VecPosition::normalizeAngle( AngDeg angle )
{
  while( angle > 180.0  ) angle -= 360.0;
  while( angle < -180.0 ) angle += 360.0;

  return ( angle );
}


/*! This method returns the global position on the field of a flag (a goal
    is also a flag). Since the global positions for both teams differ, the
    side of the agent team is also needed. Note that the global
    positions of the flags will not change in the second half.
    \param f flag of which global position should be determined
    \param s side of your team.
    \param dGoalWidth for some flags the goalWidth is necessary (default 14.02)
    \return VecPosition representing the global position. x and y value are
            both UnknownDoubleValue when o is not a flag or goal. */
VecPosition Utility::getGlobalPositionFlag(FlagT f, SideT s, double dGoalWidth)
{
  VecPosition pos;

  switch( f ) // for every object the global position is entered
  {
//    case OBJECT_GOAL_L:
//      pos.setVecPosition( -PITCH_LENGTH/2.0, 0.0 );                  break;
//    case OBJECT_GOAL_R:
//      pos.setVecPosition(  PITCH_LENGTH/2.0, 0.0 );                  break;
    case FLAG_L_T:
      pos.setVecPosition( -PITCH_LENGTH/2.0, -PITCH_WIDTH/2.0 );     break;
    case FLAG_T_L_50:
      pos.setVecPosition( -50.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case FLAG_T_L_40:
      pos.setVecPosition( -40.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case FLAG_T_L_30:
      pos.setVecPosition( -30.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case FLAG_T_L_20:
      pos.setVecPosition( -20.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case FLAG_T_L_10:
      pos.setVecPosition( -10.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case FLAG_T_0:
      pos.setVecPosition(   0.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case FLAG_C_T:
      pos.setVecPosition(   0.0, -PITCH_WIDTH/2.0);                  break;
    case FLAG_T_R_10:
      pos.setVecPosition(  10.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case FLAG_T_R_20:
      pos.setVecPosition(  20.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case FLAG_T_R_30:
      pos.setVecPosition(  30.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case FLAG_T_R_40:
      pos.setVecPosition(  40.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case FLAG_T_R_50:
      pos.setVecPosition(  50.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case FLAG_R_T:
      pos.setVecPosition( PITCH_LENGTH/2.0,  -PITCH_WIDTH/2.0 );     break;
    case FLAG_R_T_30:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  -30.0 ); break;
    case FLAG_R_T_20:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  -20.0 ); break;
    case FLAG_R_T_10:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  -10.0 ); break;
    case FLAG_G_R_T:
      pos.setVecPosition( PITCH_LENGTH/2.0,  -dGoalWidth/2.0 );      break;
    case FLAG_R_0:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  0.0 );   break;
    case FLAG_G_R_B:
      pos.setVecPosition( PITCH_LENGTH/2.0,  dGoalWidth/2.0 );       break;
    case FLAG_R_B_10:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  10.0 );  break;
    case FLAG_R_B_20:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  20.0 );  break;
    case FLAG_R_B_30:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  30.0 );  break;
    case FLAG_R_B:
      pos.setVecPosition( PITCH_LENGTH/2.0,  PITCH_WIDTH/2.0 );      break;
    case FLAG_B_R_50:
      pos.setVecPosition(  50.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_B_R_40:
      pos.setVecPosition(  40.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_B_R_30:
      pos.setVecPosition(  30.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_B_R_20:
      pos.setVecPosition(  20.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_B_R_10:
      pos.setVecPosition(  10.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_C_B:
      pos.setVecPosition(   0.0, PITCH_WIDTH/2.0 );                  break;
    case FLAG_B_0:
      pos.setVecPosition(   0.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_B_L_10:
      pos.setVecPosition( -10.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_B_L_20:
      pos.setVecPosition( -20.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_B_L_30:
      pos.setVecPosition( -30.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_B_L_40:
      pos.setVecPosition( -40.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_B_L_50:
      pos.setVecPosition( -50.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case FLAG_L_B:
      pos.setVecPosition( -PITCH_LENGTH/2.0,  PITCH_WIDTH/2.0 );     break;
    case FLAG_L_B_30:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN,  30.0 ); break;
    case FLAG_L_B_20:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN,  20.0 ); break;
    case FLAG_L_B_10:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN,  10.0 ); break;
    case FLAG_G_L_B:
      pos.setVecPosition( -PITCH_LENGTH/2.0,  dGoalWidth/2.0 );      break;
    case FLAG_L_0:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN,  0.0 );  break;
    case FLAG_G_L_T:
      pos.setVecPosition( -PITCH_LENGTH/2.0,  -dGoalWidth/2.0 );     break;
    case FLAG_L_T_10:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN, -10.0 ); break;
    case FLAG_L_T_20:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN, -20.0 ); break;
    case FLAG_L_T_30:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN, -30.0 ); break;
    case FLAG_P_L_T:
      pos.setVecPosition( -PITCH_LENGTH/2.0 + PENALTY_AREA_LENGTH,
                                         - PENALTY_AREA_WIDTH/2.0 ); break;
    case FLAG_P_L_C:
      pos.setVecPosition( -PITCH_LENGTH/2.0 + PENALTY_AREA_LENGTH, 0.0 );break;
    case FLAG_P_L_B:
      pos.setVecPosition( -PITCH_LENGTH/2.0 + PENALTY_AREA_LENGTH,
                                           PENALTY_AREA_WIDTH/2.0 ); break;
    case FLAG_P_R_T:
      pos.setVecPosition(  PITCH_LENGTH/2.0 - PENALTY_AREA_LENGTH,
                                          -PENALTY_AREA_WIDTH/2.0 ); break;
    case FLAG_P_R_C:
      pos.setVecPosition(  PITCH_LENGTH/2.0 - PENALTY_AREA_LENGTH, 0.0 );break;
    case FLAG_P_R_B:
      pos.setVecPosition(  PITCH_LENGTH/2.0 - PENALTY_AREA_LENGTH,
                                           PENALTY_AREA_WIDTH/2.0 ); break;
    case FLAG_C:
      pos.setVecPosition(  0.0 , 0.0 );                              break;
    default:
      cerr << "Wrong flag name: " << (int)f << "\n" ;
  }

  if( s == SIDE_RIGHT ) // change side for team on the right side.
      pos.setVecPosition( -pos.getX(), -pos.getY() );
  return pos;
}
/*! This method returns the play mode associated with a string.
   \param str representing the play mode
   \return PlayModeT of string, PM_ILLEGAL if it is not recognized */
PlayModeT Utility::getPlayModeFromStr(char *str)
{
  // all play modes are sent as referee message, so get referee message
  // and look up the associated play mode
  return getPlayModeFromRefereeMessage( getRefereeMessageFromStr( str ) );
}

/*! This method returns the referee message from the string that is passed.
    \param str pointer to a string with the referee message starting at index 0

    \return RefereeMessageT of string representation, REFC_ILLEGAL if
    it fails */
RefereeMessageT Utility::getRefereeMessageFromStr( char* str )
{
  switch( str[0] )
  {
    case 'b':                   // before_kick_off, back_pass_[lr]
      if( str[1] == 'e' )
        return REFC_BEFORE_KICK_OFF;
      else
        return (str[10] == 'l') ? REFC_BACK_PASS_LEFT : REFC_BACK_PASS_RIGHT;
    case 'c':                   // corner_kick_l or corner_kick_r
      return (str[12] == 'l') ? REFC_CORNER_KICK_LEFT : REFC_CORNER_KICK_RIGHT;
    case 'd':                   // drop_ball
      return REFC_DROP_BALL;
    case 'g':
      switch( str[5] )
      {
        case 'k':              // goal_kick_l, goal_kick_r
          return (str[10] == 'l') ? REFC_GOAL_KICK_LEFT : REFC_GOAL_KICK_RIGHT;
        case 'e':              // goalie_catch_ball_l, goalie_catch_ball_r
          return (str[18] == 'l') ? REFC_GOALIE_CATCH_BALL_LEFT
                                  : REFC_GOALIE_CATCH_BALL_RIGHT;
        case 'l':              // goal_l
          return REFC_GOAL_LEFT;
        case 'r':              // goal_r
          return REFC_GOAL_RIGHT;
      }
    case 'h':                  // half_time
      return REFC_HALF_TIME;
    case 'i':                  // indirect_free_kick_[lr]
      if( strlen( str ) > 19 )
        return ( str[19] == 'l' )
          ?  REFC_INDIRECT_FREE_KICK_LEFT
          :  REFC_INDIRECT_FREE_KICK_RIGHT;
      break;
    case 'f':
      if( str[5] == 'k' )      // free_kick_[lr], free_kick_fault_[lr]
      {
        if( str[10] == 'f' )
          return ( str[16] == 'l' )
                    ? REFC_FREE_KICK_FAULT_LEFT
                    : REFC_FREE_KICK_FAULT_RIGHT;
        else
          return (str[10] == 'l') ? REFC_FREE_KICK_LEFT : REFC_FREE_KICK_RIGHT;
      }
      else if( str[5] == 'l' ) // foul_l
        return REFC_FOUL_LEFT;
      else if( str[5] == 'r' ) // foul_r
        return REFC_FOUL_RIGHT;
    case 'k':                  // kick_in_l, kick_in_r, kick_off_l, kick_off_r
      if( str[5] == 'i' )
        return (str[8] == 'l' )   ? REFC_KICK_IN_LEFT   : REFC_KICK_IN_RIGHT;
      else if( str[5] == 'o' )
        return (str[9] == 'l' )   ? REFC_KICK_OFF_LEFT  : REFC_KICK_OFF_RIGHT;
    case 'o':
      return ( str[8] == 'l' )    ? REFC_OFFSIDE_LEFT   : REFC_OFFSIDE_RIGHT;
    case 'p':                // play_on or penalty_setup_l or penalty_ready_l
      if( str[1] == 'l' )    // penalty_taken_l, penalty_miss_l, penalty_foul_l
        return REFC_PLAY_ON; // penalty_onfield_l, penalty_score_l,
      else if( str[8] == 's' && str[9] == 'e' )
                             // penalty_winner_l,penalty_draw
        return ( str[14] == 'l' ) ? REFC_PENALTY_SETUP_LEFT
                                  : REFC_PENALTY_SETUP_RIGHT;
      else if( str[8] == 's' && str[9] == 'c' )
        return ( str[14] == 'l' ) ? REFC_PENALTY_SCORE_LEFT
                                  : REFC_PENALTY_SCORE_RIGHT;
      else if( str[8] == 'r' )
        return ( str[14] == 'l' ) ? REFC_PENALTY_READY_LEFT
                                  : REFC_PENALTY_READY_RIGHT;
      else if( str[8] == 't' )
        return ( str[14] == 'l' ) ? REFC_PENALTY_TAKEN_LEFT
                                  : REFC_PENALTY_TAKEN_RIGHT;
      else if( str[8] == 'm' )
        return ( str[13] == 'l' ) ? REFC_PENALTY_MISS_LEFT
                                  : REFC_PENALTY_MISS_RIGHT;
      else if( str[8] == 'o' )
        return ( str[16] == 'l' ) ? REFC_PENALTY_ONFIELD_LEFT
                                  : REFC_PENALTY_ONFIELD_RIGHT;
      else if( str[8] == 'f' )
        return ( str[13] == 'l' ) ? REFC_PENALTY_FOUL_LEFT
                                  : REFC_PENALTY_FOUL_RIGHT;
      else if( str[8] == 'f' )
        return ( str[15] == 'l' ) ? REFC_PENALTY_WINNER_LEFT
                                  : REFC_PENALTY_WINNER_RIGHT;
      else if( str[8] == 'd' )
        return REFC_PENALTY_DRAW;
    case 't':
      if( str[5] == 'o' )      // time_over
        return REFC_TIME_OVER;
      else if( str[5] == 'e' ) // time_extended
        return REFC_TIME_EXTENDED;
      else if( str[5] == 'u' ) // time_up
        return REFC_TIME_UP;
      else if( str[8] == 'w' ) // time_up_without_a_team
        return REFC_TIME_UP_WITHOUT_A_TEAM;
    default:
      printf("(Utility::getRefereeMessageFromStr) ?? ref msg %s\n",str);
      return REFC_ILLEGAL;
  }
  printf("(Utility::getRefereeMessageFromStr) ?? ref msg %s\n",str);
  return REFC_ILLEGAL;
}

/*! This method returns the play mode from the referee message.
    \param rm RefereeMessage that contains the play mode
    \return PlayModeT of RefereeMessage, PM_ILLEGAL if it is not recognized */
PlayModeT Utility::getPlayModeFromRefereeMessage( RefereeMessageT rm )
{
  switch( rm )
  {
    case REFC_BEFORE_KICK_OFF:          return PM_BEFORE_KICK_OFF;
    case REFC_KICK_OFF_LEFT:            return PM_KICK_OFF_LEFT;
    case REFC_KICK_OFF_RIGHT:           return PM_KICK_OFF_RIGHT;
    case REFC_KICK_IN_LEFT:             return PM_KICK_IN_LEFT;
    case REFC_KICK_IN_RIGHT:            return PM_KICK_IN_RIGHT;
    case REFC_CORNER_KICK_LEFT:         return PM_CORNER_KICK_LEFT;
    case REFC_CORNER_KICK_RIGHT:        return PM_CORNER_KICK_RIGHT;
    case REFC_GOAL_KICK_LEFT:           return PM_GOAL_KICK_LEFT;
    case REFC_GOAL_KICK_RIGHT:          return PM_GOAL_KICK_RIGHT;
    case REFC_FREE_KICK_LEFT:           return PM_FREE_KICK_LEFT;
    case REFC_FREE_KICK_RIGHT:          return PM_FREE_KICK_RIGHT;
    case REFC_INDIRECT_FREE_KICK_LEFT:  return PM_INDIRECT_FREE_KICK_LEFT;
    case REFC_INDIRECT_FREE_KICK_RIGHT: return PM_INDIRECT_FREE_KICK_RIGHT;
    case REFC_FREE_KICK_FAULT_LEFT:     return PM_FREE_KICK_FAULT_LEFT;
    case REFC_FREE_KICK_FAULT_RIGHT:    return PM_FREE_KICK_FAULT_RIGHT;
    case REFC_BACK_PASS_LEFT:           return PM_BACK_PASS_LEFT;
    case REFC_BACK_PASS_RIGHT:          return PM_BACK_PASS_RIGHT;
    case REFC_FOUL_LEFT:                return PM_FREE_KICK_RIGHT;
    case REFC_FOUL_RIGHT:               return PM_FREE_KICK_LEFT;
    case REFC_OFFSIDE_LEFT:             return PM_OFFSIDE_LEFT;
    case REFC_OFFSIDE_RIGHT:            return PM_OFFSIDE_RIGHT;
    case REFC_GOAL_LEFT:                return PM_GOAL_LEFT;
    case REFC_GOAL_RIGHT:               return PM_GOAL_RIGHT;
    case REFC_PENALTY_SETUP_LEFT:       return PM_PENALTY_SETUP_LEFT;
    case REFC_PENALTY_SETUP_RIGHT:      return PM_PENALTY_SETUP_RIGHT;
    case REFC_PENALTY_READY_LEFT:       return PM_PENALTY_READY_LEFT;
    case REFC_PENALTY_READY_RIGHT:      return PM_PENALTY_READY_RIGHT;
    case REFC_PENALTY_TAKEN_LEFT:       return PM_PENALTY_TAKEN_LEFT;
    case REFC_PENALTY_TAKEN_RIGHT:      return PM_PENALTY_TAKEN_RIGHT;
    case REFC_PLAY_ON:                  return PM_PLAY_ON;
    case REFC_FROZEN:                   return PM_FROZEN;
    case REFC_TIME_OVER:                return PM_TIME_OVER;
    case REFC_QUIT:                     return PM_QUIT;
    default:                            return PM_ILLEGAL;
  }
}

/*! This method returns the SideT from the string that is passed as the first
    argument.
    \param str pointer to a string that contains side info string at index 0
    \return SideT of string representation, SIDE_ILLEGAL if it is not known */
SideT Utility::getSideFromStr( char* str )
{
  if( str[0] == 'l' )      return SIDE_LEFT;
  else if( str[0] == 'r' ) return SIDE_RIGHT;
  else                     return SIDE_ILLEGAL;

}

bool equal(double v1, double v2)
{
    if (abs(v1- v2) < EPSILON) return true;

    return false;
}

bool equal(VecPosition v1, VecPosition v2)
{
    return equal(v1.getX(), v2.getX()) && equal(v1.getY(), v2.getY());
}

bool vecInField(VecPosition pos)
{
    return abs(pos.getX()) <= PITCH_LENGTH/2 && abs(pos.getY()) <= PITCH_WIDTH/2;
}
