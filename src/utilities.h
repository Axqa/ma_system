#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include "enums.h"
#include <cmath>

using namespace std;

#define MAX_TEAMMATES          11       /*!< Maximum number of teammates     */
#define MAX_OPPONENTS          11       /*!< Maximum number of opponents     */
#define MAX_HETERO_PLAYERS      18       /*!< Maximum number of hetero players*/
#define MAX_MSG              4096       /*!< maximum message size from server*/
#define MAX_SAY_MSG            10       /*!< maximum size of say message     */
#define MAX_TEAM_NAME_LENGTH   64       /*!< maximum length of teamname      */
#define MAX_FLAGS              55       /*!< maximum number of flags on field*/
#define MAX_LINES               4       /*!< maximum number of lines on field*/
#define DEFAULT_TEAM_NAME     "Team_L"  /*!< default teamname for own team   */
#define DEFAULT_OPPONENT_NAME "Team_R"  /*!< default teamname for opponent   */
#define PITCH_LENGTH        105.0       /*!< length of the pitch             */
#define PITCH_WIDTH          68.0       /*!< width  of the pitch             */
#define PITCH_MARGIN          5.0       /*!< margin next to the pitch        */
#define PENALTY_AREA_LENGTH  16.5       /*!< length of the penalty area      */
#define PENALTY_AREA_WIDTH   40.32      /*!< width of the penalty area       */
#define PENALTY_X (PITCH_LENGTH/2.0-PENALTY_AREA_LENGTH) /*!< penalty line of
                                             the opponent team               */

#define MAX_FORMATIONS      5           /*!< default maximum formations      */

#define PLAYER_SPEED_MAX    1.05        /*!< default maximal player speed    */
#define PLAYER_MAX_POWER    100         /*!< default player max dash power   */

#define SERVER_TIME_OUT     3

typedef double AngRad;  /*!< Type definition for angles in degrees. */
typedef double AngDeg;  /*!< Type definition for angles in radians. */

extern int    HISTORY_SIZE;                  /*!< default maximum history size    */
const double  UnknownDoubleValue  = -1000.0; /*!< indicates unknown double   */
const AngDeg  UnknownAngleValue   = -1000.0; /*!< indicates unknown angle    */

#define EPSILON 0.0001  /*!< Value used for floating point equality tests. */
#define MIN_DIST 0.01   /*!< Value used for minimum distance in division.  */
class VecPosition;

// auxiliary numeric functions for determining the
// maximum and minimum of two given double values and the sign of a value
double max     ( double d1, double d2 );
double min     ( double d1, double d2 );
int    sign    ( double d1            );

bool equal      (double v1, double v2);
bool equal      (VecPosition v1, VecPosition v2);

// auxiliary goniometric functions which enable you to
// specify angles in degrees rather than in radians
AngDeg Rad2Deg ( AngRad x             );
AngRad Deg2Rad ( AngDeg x             );
double cosDeg  ( AngDeg x             );
double sinDeg  ( AngDeg x             );
double tanDeg  ( AngDeg x             );
AngDeg atanDeg ( double x             );
double atan2Deg( double x,  double y  );
AngDeg acosDeg ( double x             );
AngDeg asinDeg ( double x             );

// various goniometric functions
bool   isAngInInterval     ( AngDeg ang,    AngDeg angMin,    AngDeg angMax );
AngDeg getBisectorTwoAngles( AngDeg angMin, AngDeg angMax );

bool   vecInField(VecPosition pos);

class VecPosition
{
  // private member data
private:

  double m_x;   /*!< x-coordinate of this position */
  double m_y;   /*!< y-coordinate of this position */

  // public methods
public:
  // constructor for VecPosition class
  VecPosition                               ( double            vx = 0,
                                              double            vy = 0,
                                              CoordSystemT      cs =CARTESIAN);

  // overloaded arithmetic operators
  VecPosition        operator -             (                                );
  VecPosition        operator +             ( const double      &d           );
  VecPosition        operator +             ( const VecPosition &p           );
  VecPosition        operator -             ( const double      &d           );
  VecPosition        operator -             ( const VecPosition &p           );
  VecPosition        operator *             ( const double      &d           );
  VecPosition        operator *             ( const VecPosition &p           );
  VecPosition        operator /             ( const double      &d           );
  VecPosition        operator /             ( const VecPosition &p           );
  void               operator =             ( const double      &d           );
  void               operator +=            ( const VecPosition &p           );
  void               operator +=            ( const double      &d           );
  void               operator -=            ( const VecPosition &p           );
  void               operator -=            ( const double      &d           );
  void               operator *=            ( const VecPosition &p           );
  void               operator *=            ( const double      &d           );
  void               operator /=            ( const VecPosition &p           );
  void               operator /=            ( const double      &d           );
  bool               operator !=            ( const VecPosition &p           );
  bool               operator !=            ( const double      &d           );
  bool               operator ==            ( const VecPosition &p           );
  bool               operator ==            ( const double      &d           );

  // methods for producing output
  friend ostream&    operator <<            ( ostream           &os,
                                              VecPosition       p            );
  void               show                   ( CoordSystemT      cs =CARTESIAN);
  string             str                    ( CoordSystemT      cs =CARTESIAN);

  // set- and get methods for private member variables
  bool               setX                   ( double            dX           );
  double             getX                   (                          ) const;
  bool               setY                   ( double            dY           );
  double             getY                   (                          ) const;

  // set- and get methods for derived position information
  void               setVecPosition         ( double            dX = 0,
                                              double            dY = 0,
                                              CoordSystemT      cs =CARTESIAN);
  double             getDistanceTo          ( const VecPosition p            );
  VecPosition        setMagnitude           ( double            d            );
  double             getMagnitude           (                          ) const;
  AngDeg             getDirection           (                          ) const;

  // comparison methods for positions
  bool               isInFrontOf            ( const VecPosition &p           );
  bool               isInFrontOf            ( const double      &d           );
  bool               isBehindOf             ( const VecPosition &p           );
  bool               isBehindOf             ( const double      &d           );
  bool               isLeftOf               ( const VecPosition &p           );
  bool               isLeftOf               ( const double      &d           );
  bool               isRightOf              ( const VecPosition &p           );
  bool               isRightOf              ( const double      &d           );
  bool               isBetweenX             ( const VecPosition &p1,
                                              const VecPosition &p2          );
  bool               isBetweenX             ( const double      &d1,
                                              const double      &d2          );
  bool               isBetweenY             ( const VecPosition &p1,
                                              const VecPosition &p2          );
  bool               isBetweenY             ( const double      &d1,
                                              const double      &d2          );

  // conversion methods for positions
  VecPosition        normalize              (                                );
  VecPosition        rotate                 ( AngDeg            angle        );
  VecPosition        globalToRelative       ( VecPosition       orig,
                                              AngDeg            ang          );
  VecPosition        relativeToGlobal       ( VecPosition       orig,
                                              AngDeg            ang          );
  VecPosition        getVecPositionOnLineFraction( VecPosition  &p,
                                              double            dFrac        );

  // static class methods
  static VecPosition getVecPositionFromPolar( double            dMag,
                                              AngDeg            ang          );
  static AngDeg      normalizeAngle         ( AngDeg            angle        );
};

class Utility {
public:

    static VecPosition     getGlobalPositionFlag     ( FlagT      f,
                                                       SideT      s,
                                                       double dGoalWidth =14.02);

    static PlayModeT       getPlayModeFromStr        ( char         *str       );

    static RefereeMessageT getRefereeMessageFromStr  ( char         *str       );
    static PlayModeT       getPlayModeFromRefereeMessage( RefereeMessageT rm   );

    static SideT           getSideFromStr( char* str );
};

#endif // UTILITIES_H
