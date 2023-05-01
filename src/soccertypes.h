#ifndef SOCCERTYPES_H
#define SOCCERTYPES_H

#include "utilities.h"

/*! The class SoccerTypes contains different methods to work with the
    different enumerations defined in SoccerTypes.h. It is possible to
    convert soccertypes to strings and strings to soccertypes. It is
    also possible to get more specific information about some of the
    soccertypes. All methods are static so it is possible to call the
    methods without instantiating the class. */
class SoccerTypes
{
 public:
  // methods that deal with the differen objects
  static char*           getObjectStr              ( char         *strBuf,
                                                     ObjectT      o,
                                                     const char   *strTe=NULL);
  static ObjectT         getObjectFromStr          ( char         **str,
                                                     bool         *isGoalie,
                                                     const char   *strTeam   );

  static bool            isFlag                    ( ObjectT      o          );
  static bool            isLine                    ( ObjectT      o          );
  static bool            isGoal                    ( ObjectT      o          );
  static ObjectT         getOwnGoal                ( SideT        s          );
  static ObjectT         getGoalOpponent           ( SideT        s          );
  static bool            isBall                    ( ObjectT      o          );
  static bool            isTeammate                ( ObjectT      o          );
  static bool            isOpponent                ( ObjectT      o          );
  static bool            isGoalie                  ( ObjectT      o          );
  static bool            isPlayer                  ( ObjectT      o          );
  static bool            isKnownPlayer             ( ObjectT      o          );
  static int             getIndex                  ( ObjectT      o          );
  static ObjectT         getTeammateObjectFromIndex( int          iIndex     );
  static ObjectT         getOpponentObjectFromIndex( int          iIndex     );
  static ObjectT         getFlagFromIndex          ( int          iIndex     );
  static ObjectT         getLineFromIndex          ( int          iIndex     );
  static VecPosition     getGlobalPositionFlag     ( ObjectT      o,
                                                     SideT        s,
                                                     double dGoalWidth =14.02);
  static AngDeg          getGlobalAngleLine        ( ObjectT      o,
                                                     SideT        s          );

  static SideT           getSideFromPlayer          (ObjectT o, SideT selfSide);
  static int             getNumFromPlayer           (ObjectT o               );
  // methods that deal with the differen play modes
  static PlayModeT       getPlayModeFromStr        ( char         *str       );
  static PlayModeT       getPlayModeFromRefereeMessage( RefereeMessageT rm   );
const  static char*           getPlayModeStr            ( PlayModeT    p          );
  const static char*           getRefereeMessageStr      ( RefereeMessageT r       );
  static RefereeMessageT getRefereeMessageFromStr  ( char         *str       );

  // methods that deal with the frequency of the visual information
  const static char*           getViewAngleStr           ( ViewAngleT   v          );
  static ViewAngleT      getViewAngleFromStr       ( char         *str       );
  static AngDeg          getHalfViewAngleValue     ( ViewAngleT   va         );
  const static char*           getViewQualityStr         ( ViewQualityT v          );
  static ViewQualityT    getViewQualityFromStr     ( char         *str       );

  // methods that deal with the commands
  const static char*           getCommandStr             ( CommandT     com        );
  static bool            isPrimaryCommand          ( CommandT     com        );

  // methods that deal with the side information
  const static char*           getSideStr                ( SideT        s          );
 static SideT           getSideFromStr            ( char*        str        );

} ;


#endif // SOCCERTYPES_H
