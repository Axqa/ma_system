#include "soccertypes.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include "parser.h"

/*! This constant defines the string names corresponding to the
    ObjectT names, defined in SoccerTypes.h. Players are not added
    since they depend on the team name. Note that the order is
    important, since the names are in the same order as the ObjectT
    enumeration. */
const char * ObjectNames[] =
{
"(b)", "(g l)", "(g r)", "(g ?)", "(l l)", "(l r)", "(l b)", "(l t)",
"(f l t)","(f t l 50)", "(f t l 40)", "(f t l 30)", "(f t l 20)",
"(f t l 10)", "(f t 0)","(f c t)","(f t r 10)", "(f t r 20)", "(f t r 30)",
"(f t r 40)","(f t r 50)", "(f r t)", "(f r t 30)", "(f r t 20)","(f r t 10)",
"(f g r t)" , "(f r 0)", "(f g r b)" , "(f r b 10)", "(f r b 20)",
"(f r b 30)", "(f r b)"   , "(f b r 50)", "(f b r 40)", "(f b r 30)",
"(f b r 20)", "(f b r 10)", "(f c b)"   , "(f b 0)"   , "(f b l 10)",
"(f b l 20)", "(f b l 30)", "(f b l 40)", "(f b l 50)", "(f l b)",
"(f l b 30)", "(f l b 20)", "(f l b 10)", "(f g l b)" , "(f l 0)",
"(f g l t)" , "(f l t 10)", "(f l t 20)", "(f l t 30)", "(f p l t)",
"(f p l c)", "(f p l b)",   "(f p r t)",  "(f p r c)",  "(f p r b)", "(f c)" };

/*! This method returns the string that corresponds to a specific object. This
    string name is exactly the same as the (short) name of the RoboCup
    Simulation.
    \param strBuf is the string in which the string representation is stored
    \param o ObjectT that has to be converted to the string representation
    \param strTeamName teamname that should be placed in case of player object
    \return pointer to strBuf, which contains the string representation */
char* SoccerTypes::getObjectStr( char* strBuf, ObjectT o,
                                 const char *strTeamName )
{
  if( o >= OBJECT_BALL && o <=   OBJECT_FLAG_C )
    sprintf( strBuf, ObjectNames[(int)o - 1] );
  else if( isKnownPlayer( o ) && strTeamName != NULL )
    sprintf( strBuf, "(p %s %d)", strTeamName, getIndex( o ) + 1);
  else if( isKnownPlayer( o ) && isTeammate( o ) )
    sprintf( strBuf, "(p l %d)", getIndex( o ) + 1);
  else if( isKnownPlayer( o ) && isOpponent( o ) )
    sprintf( strBuf, "(p r %d)", getIndex( o ) + 1);
  else if( o == OBJECT_OPPONENT_UNKNOWN || o == OBJECT_TEAMMATE_UNKNOWN )
    sprintf( strBuf, "(p %s)", strTeamName );
  else if( o == OBJECT_PLAYER_UNKNOWN )
    sprintf( strBuf, "(p)" );
  else if( o == OBJECT_UNKNOWN )
    sprintf( strBuf, "(unknown)" );
  else
    sprintf( strBuf, "illegal: %d", (int)o );
  return strBuf;
}

/*! This method returns an ObjectT that corresponds to the string
    passed as the first argument. The string representation equals the
    representation used in the Soccer Server. Format is with
    parenthesis, so possible arguments for str are (ball), (p Team_L
    1), etc.

    \param str pointer to string containing string representation of object
    \param isGoalie bool representing the fact whether object is a goalie
    \param strMyTeamName in case of player or opponent object, own teamname
           has to be matched, when it matches it is teammate otherwise opponent

    \return return the corresponding ObjectT, OBJECT_ILLEGAL in case
    of error */
ObjectT SoccerTypes::getObjectFromStr( char** str, bool *isGoalie,
                                       const char* strMyTeamName )
{
  ObjectT o = OBJECT_ILLEGAL;
  char* ptrStr = *str;
  *isGoalie = false;

  switch( ptrStr[1] )
  {
    case 'b':                       // (ball)
    case 'B':                       // (B) in case of ball very close
       o = OBJECT_BALL; break;
    case 'G':
       o = OBJECT_GOAL_UNKNOWN;      // (G) in case of goal very close, ignored
      break;                        // (g l) or (g r) goal left or goal right
    case 'g': o = (ptrStr[3] == 'l') ? OBJECT_GOAL_L : OBJECT_GOAL_R; break;
    case 'l':                       // (l l), (l r), (l b) or (l t)
      switch( ptrStr[3] )
      {
        case 'l': o = OBJECT_LINE_L;  break;
        case 'r': o = OBJECT_LINE_R;  break;
        case 'b': o = OBJECT_LINE_B;  break;
        case 't': o = OBJECT_LINE_T;  break;
        default:  o = OBJECT_ILLEGAL; break;
      }
      break;
    case 'F':                       // (F) unkown flag very close.. ignored
      o = OBJECT_UNKNOWN; break;
    case 'f':                       // (f ...),  many options...
      switch( ptrStr[3] )
      {
        case 'l':                   // (f l ... lines on left part of field
          if( ptrStr[6] == ')' )    // only one character at index '5'
          {
            switch( ptrStr[5] )
            {
              case '0': o = OBJECT_FLAG_L_0; break; // (f l 0)
              case 't': o = OBJECT_FLAG_L_T; break; // (f l t)
              case 'b': o = OBJECT_FLAG_L_B; break; // (f l b)
              default:  o = OBJECT_ILLEGAL;  break;
            }
          }
          else                      // more than one character from index '5'
          {
            switch( ptrStr[7] )
            {
              case '1':             // (f l t 10) or (f l b 10)
                o = (ptrStr[5]=='t')? OBJECT_FLAG_L_T_10 :OBJECT_FLAG_L_B_10;
                break;
              case '2':             // (f l t 20) or (f l b 20)
                o = (ptrStr[5]=='t')? OBJECT_FLAG_L_T_20 :OBJECT_FLAG_L_B_20;
                break;
              case '3':             // (f l t 30) or (f l b 30)
                o = (ptrStr[5]=='t')? OBJECT_FLAG_L_T_30 :OBJECT_FLAG_L_B_30;
                break;
              default:
                o = OBJECT_ILLEGAL;
                break;
            }
          }
          break;
        case 'r':                   // (f r ... lines on right side of field
          if( ptrStr[6] == ')' )    // only one character at index '5'
          {
            switch( ptrStr[5] )
            {
              case '0': o = OBJECT_FLAG_R_0; break; // (f l 0)
              case 't': o = OBJECT_FLAG_R_T; break; // (f l t)
              case 'b': o = OBJECT_FLAG_R_B; break; // (f l b)
            }
          }
          else
          {
            switch( ptrStr[7] )     // more than one character from index '5'
            {
              case '1':
                o = (ptrStr[5]=='t')? OBJECT_FLAG_R_T_10 :OBJECT_FLAG_R_B_10;
                break;
              case '2':             // (f r t 10) or (f r b 10)
                o = (ptrStr[5]=='t')? OBJECT_FLAG_R_T_20 :OBJECT_FLAG_R_B_20;
                break;              // (f r t 20) or (f r b 20)
              case '3':
                o = (ptrStr[5]=='t')? OBJECT_FLAG_R_T_30 :OBJECT_FLAG_R_B_30;
                break;              // (f r t 30) or (f r b 30)
              default:
                o = OBJECT_ILLEGAL;
                break;
            }
          }
          break;
        case 't':                   // lines on top part of field
          if( ptrStr[5] == '0' )
            o = OBJECT_FLAG_T_0;    // (f t 0) center flag
          else
          {
            switch( ptrStr[7] )     // rest of the top flags
            {
              case '1':             // (f t l 10) or (f t r 10)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_T_L_10 : OBJECT_FLAG_T_R_10;
                break;
              case '2':             // (f t l 20) or (f t r 20)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_T_L_20 : OBJECT_FLAG_T_R_20;
                break;
              case '3':             // (f t l 30) or (f t r 30)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_T_L_30 : OBJECT_FLAG_T_R_30;
                break;
              case '4':             // (f t l 40) or (f t r 40)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_T_L_40 : OBJECT_FLAG_T_R_40;
                break;
              case '5':             // (f t l 50) or (f t r 50)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_T_L_50 : OBJECT_FLAG_T_R_50;
                break;
            }
          }
          break;
        case 'b':                   // lines on bottom part of field
          if( ptrStr[5] == '0')
            o = OBJECT_FLAG_B_0;    // (f b 0) center flag
          else
          {
            switch( ptrStr[7] )     // rest of the bottom flags
            {
              case '1':             // (f b l 10) or (f b r 10)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_B_L_10 : OBJECT_FLAG_B_R_10;
                break;
              case '2':             // (f b l 20) or (f b r 20)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_B_L_20 : OBJECT_FLAG_B_R_20;
                break;
              case '3':             // (f b l 30) or (f b r 30)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_B_L_30 : OBJECT_FLAG_B_R_30;
                break;
              case '4':             // (f b l 40) or (f b r 40)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_B_L_40 : OBJECT_FLAG_B_R_40;
                break;
              case '5':             // (f b l 50) or (f b r 50)
                o = (ptrStr[5]=='l') ? OBJECT_FLAG_B_L_50 : OBJECT_FLAG_B_R_50;
                break;
            }
          }
          break;
        case 'c':                   // center flags
          if( ptrStr[4] == ')' )
            o = OBJECT_FLAG_C;      // (f c) flag in midpoint field
          else                      // (f c t) or (f c b)
            o = (ptrStr[5] == 't') ? OBJECT_FLAG_C_T : OBJECT_FLAG_C_B;
          break;
        case 'g':                   // goal flags
          if( ptrStr[5] == 'l' )    // (g l t) or (g l b)
            o = (ptrStr[7] == 't') ? OBJECT_FLAG_G_L_T : OBJECT_FLAG_G_L_B;
          else                      // (g r t) or (g r b)
            o = (ptrStr[7] == 't') ? OBJECT_FLAG_G_R_T : OBJECT_FLAG_G_R_B;
          break;
        case 'p':                   // flags at sides penalty area
          switch( ptrStr[7] )
          {
            case 't':               // (p l t) or (p r t) top penalty area
              o = (ptrStr[5] == 'l') ? OBJECT_FLAG_P_L_T : OBJECT_FLAG_P_R_T;
              break;
            case 'c':               // (p l c) or (p r c) center penalty area
              o = (ptrStr[5] == 'l') ? OBJECT_FLAG_P_L_C : OBJECT_FLAG_P_R_C;
              break;
            case 'b':               // (p l b) or (p r b) bottom penalty area
              o = (ptrStr[5] == 'l') ? OBJECT_FLAG_P_L_B : OBJECT_FLAG_P_R_B;
              break;
            default:
              o = OBJECT_ILLEGAL;
              break;
          }
          break;
        default:
          o = OBJECT_ILLEGAL;
      }
      break; // end flags (finally)
  case 'p': // (p team nr) or (p team) or (p) player teammate or opponent
  case 'P': // or (P)
    ptrStr += 2;

    if( Parse::gotoFirstSpaceOrClosingBracket(&ptrStr) == ')' )
      o = OBJECT_PLAYER_UNKNOWN; // if (p) or (P) player is unknown.
    // check also with quotes since later versions use string around "teamname"
    else if( strncmp( ptrStr+1, strMyTeamName, strlen( strMyTeamName )) == 0 ||
             strncmp( ptrStr+2, strMyTeamName, strlen( strMyTeamName )) == 0 )
    {
      ptrStr++;
      if( Parse::gotoFirstSpaceOrClosingBracket(&ptrStr) == ' ' )
      {                                               // also team number
        switch( Parse::parseFirstInt( &ptrStr ) )     // get team number
        {
          case 1:  o = OBJECT_TEAMMATE_1;  break;     // and find team member
          case 2:  o = OBJECT_TEAMMATE_2;  break;
          case 3:  o = OBJECT_TEAMMATE_3;  break;
          case 4:  o = OBJECT_TEAMMATE_4;  break;
          case 5:  o = OBJECT_TEAMMATE_5;  break;
          case 6:  o = OBJECT_TEAMMATE_6;  break;
          case 7:  o = OBJECT_TEAMMATE_7;  break;
          case 8:  o = OBJECT_TEAMMATE_8;  break;
          case 9:  o = OBJECT_TEAMMATE_9;  break;
          case 10: o = OBJECT_TEAMMATE_10; break;
          case 11: o = OBJECT_TEAMMATE_11; break;
          default: o = OBJECT_ILLEGAL;
        }
        if( ptrStr[0] != ')' && ptrStr[1] == 'g' ) // goalie
          *isGoalie = true;
      }
      else
        o = OBJECT_TEAMMATE_UNKNOWN;                  // (p team) but no nr
    }
    else                                              // not a teammate
    {
      ptrStr++;
      if( Parse::gotoFirstSpaceOrClosingBracket( &ptrStr ) == ' ' )
      {                                               // also team number
        switch( Parse::parseFirstInt( &ptrStr ) )     // get team numer
        {
          case 1:  o = OBJECT_OPPONENT_1;  break;     // and return opponent
          case 2:  o = OBJECT_OPPONENT_2;  break;
          case 3:  o = OBJECT_OPPONENT_3;  break;
          case 4:  o = OBJECT_OPPONENT_4;  break;
          case 5:  o = OBJECT_OPPONENT_5;  break;
          case 6:  o = OBJECT_OPPONENT_6;  break;
          case 7:  o = OBJECT_OPPONENT_7;  break;
          case 8:  o = OBJECT_OPPONENT_8;  break;
          case 9:  o = OBJECT_OPPONENT_9;  break;
          case 10: o = OBJECT_OPPONENT_10; break;
          case 11: o = OBJECT_OPPONENT_11; break;
          default: o = OBJECT_ILLEGAL;
        }
        if( ptrStr[0] != ')' && ptrStr[1] == 'g' ) // goalie
          *isGoalie = true;
      }
      else
        o = OBJECT_OPPONENT_UNKNOWN;                  // number not known

    }
    break;
  default:
    cerr << "(SoccerTypes::getObjectFromStr) Unknown msg: " <<  ptrStr << "\n";
    o = OBJECT_ILLEGAL;
    break;
  }
  // go to the end of the object
  Parse::gotoFirstOccurenceOf( ')', &ptrStr );
  *str=ptrStr;
  return o;
}

/*! This method returns the index of an object relative to the first object
    in that set.
    The index is always 1 smaller than its number, so OBJECT_OPPONENT_1
    will become 0. This can be used for indexing an array of objects.
    \param o ObjectT type of object of which the index has to be calculated
    \return index of object or -1 when o was not a correct object */
int SoccerTypes::getIndex( ObjectT o )
{
  if( o >= OBJECT_OPPONENT_1 && o <= OBJECT_OPPONENT_11 )
    return o - OBJECT_OPPONENT_1;
  else if( o >= OBJECT_TEAMMATE_1 && o <= OBJECT_TEAMMATE_11 )
    return o - OBJECT_TEAMMATE_1;
  else if( o >= OBJECT_GOAL_L && o <= OBJECT_GOAL_R )
    return o - OBJECT_GOAL_L;
  else if( o >= OBJECT_FLAG_L_T && o <= OBJECT_FLAG_C )
    return o - OBJECT_FLAG_L_T + 2; // 2 added for the two goals
  else if( o >= OBJECT_LINE_L && o <= OBJECT_LINE_T )
    return o - OBJECT_LINE_L;
  else
    return -1;
}

/*! This method returns the object type of a teammate with index iIndex. When
    iIndex equals 3 for example OBJECT_TEAMMATE_4 is returned.
    \param iIndex index of teammate range is [0..10]
    \return object type corresponding to this index */
ObjectT SoccerTypes::getTeammateObjectFromIndex( int iIndex )
{
  return (ObjectT) ( OBJECT_TEAMMATE_1 + iIndex );
}

/*! This method returns the object type of an opponent with index iIndex. When
    iIndex equals 9 for example OBJECT_OPPONENT_10 is returned.
    \param iIndex index of opponent range is [0..10]
    \return object type corresponding to this index */
ObjectT SoccerTypes::getOpponentObjectFromIndex( int iIndex )
{
  return (ObjectT) ( OBJECT_OPPONENT_1 + iIndex );
}

/*! This method returns the object type of an flag with index iIndex.
    \param iIndex index of flag range is [0..54]
    \return object type corresponding to this index */
ObjectT SoccerTypes::getFlagFromIndex( int iIndex )
{
    switch (iIndex) {
        case 0: return OBJECT_GOAL_L;
        case 1: return OBJECT_GOAL_R;
        default: return (ObjectT)(OBJECT_FLAG_L_T + iIndex - 2);
    }
}

ObjectT SoccerTypes::getLineFromIndex(int iIndex)
{
    return (ObjectT)(OBJECT_LINE_L + iIndex);
}


/*! This method determines whether object o is a flag.
    \param o an object type
    \return bool indicating whether o is a flag (return true) or not (false) */
bool SoccerTypes::isFlag( ObjectT o )
{
  return ( o >= OBJECT_FLAG_L_T && o <= OBJECT_FLAG_C ) ||
         ( o >= OBJECT_GOAL_L   && o <= OBJECT_GOAL_R        );
}

/*! This method determines whether object o is a line.
     \param o an object type
     \return bool indicating whether o is a line return true) or not (false) */
bool SoccerTypes::isLine( ObjectT o )
{
  return o >= OBJECT_LINE_L && o <= OBJECT_LINE_T;
}

/*! This method determines whether object o is a goal
    \param o an object type
    \return bool indicating whether o is a goal (return true) or not (false) */
bool SoccerTypes::isGoal( ObjectT o )
{
  return o == OBJECT_GOAL_L || o == OBJECT_GOAL_R;
}

/*! This method determines whether object o is a teammate
    \param o an object type
    \return bool indicating whether o is a teammate (true) or not (false) */
bool SoccerTypes::isTeammate( ObjectT o )
{
  return o >= OBJECT_TEAMMATE_1 && o <= OBJECT_TEAMMATE_UNKNOWN;
}

/*! This method determines whether object o is an opponent
    \param o an object type
    \return bool indicating whether o is an opponent (true) or not (false) */
bool SoccerTypes::isOpponent( ObjectT o )
{
  return o >= OBJECT_OPPONENT_1 && o <= OBJECT_OPPONENT_UNKNOWN;
}

/*! This method determines whether object o is a player without checking
    whether its number or side is available.
    \param o an object type

    \return bool indicating whether o is a known player (true) or not
    (false) */
bool SoccerTypes::isPlayer( ObjectT o )
{
  return isKnownPlayer( o )           || o == OBJECT_TEAMMATE_UNKNOWN ||
         o == OBJECT_OPPONENT_UNKNOWN || o == OBJECT_PLAYER_UNKNOWN;
}

/*! This method determines whether object o is a known player, thus
    containing a number
    \param o an object type

    \return bool indicating whether o is a known player (true) or not
    (false) */
bool SoccerTypes::isKnownPlayer( ObjectT o )
{
  return (o >= OBJECT_OPPONENT_1 && o <= OBJECT_OPPONENT_11) ||
         (o >= OBJECT_TEAMMATE_1 && o <= OBJECT_TEAMMATE_11);
}

/*! This method determines whether object o is a goalie = teammate number is 1
    (for now)
    \param o an object type
    \return bool indicating whether o is a goalie (true) or not (false) */
bool SoccerTypes::isGoalie( ObjectT o )
{
  return o == OBJECT_TEAMMATE_1 || o == OBJECT_OPPONENT_1;
}

/*! This method determines whether object o is the ball
    \param o an object type
    \return bool indicating whether o is the ball (true) or not (false) */
bool SoccerTypes::isBall( ObjectT o )
{
  return o == OBJECT_BALL;
}

/*! This method returns the object representing the own goal
    \param s own side
    \return object of the own goal, OBJECT_ILLEGAL when s is SIDE_ILLEGAL*/
ObjectT SoccerTypes::getOwnGoal( SideT s )
{
  if( SIDE_LEFT == s )
    return OBJECT_GOAL_L;
  else if( SIDE_RIGHT == s )
    return OBJECT_GOAL_R;

  cerr << "(SoccerTypes::isOwnGoal) Wrong side argument" << "\n";
  return OBJECT_ILLEGAL;
}

/*! This method returns the object representing the opponent goal
    \param s own side

    \return object of the goal opponent, OBJECT_ILLEGAL when s is
    SIDE_ILLEGAL*/
ObjectT SoccerTypes::getGoalOpponent( SideT s )
{
  if( SIDE_LEFT == s )
    return OBJECT_GOAL_R;
  else if( SIDE_RIGHT == s )
    return OBJECT_GOAL_L;

  cerr << "(SoccerTypes::isGoalOpponent) Wrong side argument" << "\n";
  return OBJECT_ILLEGAL;
}

/*! This method returns the global position on the field of a flag (a goal
    is also a flag). Since the global positions for both teams differ, the
    side of the agent team is also needed. Note that the global
    positions of the flags will not change in the second half.
    \param o flag of which global position should be determined
    \param s side of your team.
    \param dGoalWidth for some flags the goalWidth is necessary (default 14.02)
    \return VecPosition representing the global position. x and y value are
            both UnknownDoubleValue when o is not a flag or goal. */
VecPosition SoccerTypes::getGlobalPositionFlag( ObjectT o, SideT s,
                                                double dGoalWidth )
{
  VecPosition pos;
  if( !(isFlag(o) || isGoal(o)) )
      return VecPosition(UnknownDoubleValue, UnknownDoubleValue);
  switch( o ) // for every object the global position is entered
  {
    case OBJECT_GOAL_L:
      pos.setVecPosition( -PITCH_LENGTH/2.0, 0.0 );                  break;
    case OBJECT_GOAL_R:
      pos.setVecPosition(  PITCH_LENGTH/2.0, 0.0 );                  break;
    case OBJECT_FLAG_L_T:
      pos.setVecPosition( -PITCH_LENGTH/2.0, -PITCH_WIDTH/2.0 );     break;
    case OBJECT_FLAG_T_L_50:
      pos.setVecPosition( -50.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case OBJECT_FLAG_T_L_40:
      pos.setVecPosition( -40.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case OBJECT_FLAG_T_L_30:
      pos.setVecPosition( -30.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case OBJECT_FLAG_T_L_20:
      pos.setVecPosition( -20.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case OBJECT_FLAG_T_L_10:
      pos.setVecPosition( -10.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case OBJECT_FLAG_T_0:
      pos.setVecPosition(   0.0, -PITCH_WIDTH/2.0 - PITCH_MARGIN );  break;
    case OBJECT_FLAG_C_T:
      pos.setVecPosition(   0.0, -PITCH_WIDTH/2.0);                  break;
    case OBJECT_FLAG_T_R_10:
      pos.setVecPosition(  10.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case OBJECT_FLAG_T_R_20:
      pos.setVecPosition(  20.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case OBJECT_FLAG_T_R_30:
      pos.setVecPosition(  30.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case OBJECT_FLAG_T_R_40:
      pos.setVecPosition(  40.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case OBJECT_FLAG_T_R_50:
      pos.setVecPosition(  50.0,  -PITCH_WIDTH/2.0 - PITCH_MARGIN);  break;
    case OBJECT_FLAG_R_T:
      pos.setVecPosition( PITCH_LENGTH/2.0,  -PITCH_WIDTH/2.0 );     break;
    case OBJECT_FLAG_R_T_30:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  -30.0 ); break;
    case OBJECT_FLAG_R_T_20:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  -20.0 ); break;
    case OBJECT_FLAG_R_T_10:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  -10.0 ); break;
    case OBJECT_FLAG_G_R_T:
      pos.setVecPosition( PITCH_LENGTH/2.0,  -dGoalWidth/2.0 );      break;
    case OBJECT_FLAG_R_0:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  0.0 );   break;
    case OBJECT_FLAG_G_R_B:
      pos.setVecPosition( PITCH_LENGTH/2.0,  dGoalWidth/2.0 );       break;
    case OBJECT_FLAG_R_B_10:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  10.0 );  break;
    case OBJECT_FLAG_R_B_20:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  20.0 );  break;
    case OBJECT_FLAG_R_B_30:
      pos.setVecPosition( PITCH_LENGTH/2.0 + PITCH_MARGIN,  30.0 );  break;
    case OBJECT_FLAG_R_B:
      pos.setVecPosition( PITCH_LENGTH/2.0,  PITCH_WIDTH/2.0 );      break;
    case OBJECT_FLAG_B_R_50:
      pos.setVecPosition(  50.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_B_R_40:
      pos.setVecPosition(  40.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_B_R_30:
      pos.setVecPosition(  30.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_B_R_20:
      pos.setVecPosition(  20.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_B_R_10:
      pos.setVecPosition(  10.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_C_B:
      pos.setVecPosition(   0.0, PITCH_WIDTH/2.0 );                  break;
    case OBJECT_FLAG_B_0:
      pos.setVecPosition(   0.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_B_L_10:
      pos.setVecPosition( -10.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_B_L_20:
      pos.setVecPosition( -20.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_B_L_30:
      pos.setVecPosition( -30.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_B_L_40:
      pos.setVecPosition( -40.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_B_L_50:
      pos.setVecPosition( -50.0, PITCH_WIDTH/2.0 + PITCH_MARGIN );   break;
    case OBJECT_FLAG_L_B:
      pos.setVecPosition( -PITCH_LENGTH/2.0,  PITCH_WIDTH/2.0 );     break;
    case OBJECT_FLAG_L_B_30:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN,  30.0 ); break;
    case OBJECT_FLAG_L_B_20:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN,  20.0 ); break;
    case OBJECT_FLAG_L_B_10:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN,  10.0 ); break;
    case OBJECT_FLAG_G_L_B:
      pos.setVecPosition( -PITCH_LENGTH/2.0,  dGoalWidth/2.0 );      break;
    case OBJECT_FLAG_L_0:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN,  0.0 );  break;
    case OBJECT_FLAG_G_L_T:
      pos.setVecPosition( -PITCH_LENGTH/2.0,  -dGoalWidth/2.0 );     break;
    case OBJECT_FLAG_L_T_10:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN, -10.0 ); break;
    case OBJECT_FLAG_L_T_20:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN, -20.0 ); break;
    case OBJECT_FLAG_L_T_30:
      pos.setVecPosition( -PITCH_LENGTH/2.0 - PITCH_MARGIN, -30.0 ); break;
    case OBJECT_FLAG_P_L_T:
      pos.setVecPosition( -PITCH_LENGTH/2.0 + PENALTY_AREA_LENGTH,
                                         - PENALTY_AREA_WIDTH/2.0 ); break;
    case OBJECT_FLAG_P_L_C:
      pos.setVecPosition( -PITCH_LENGTH/2.0 + PENALTY_AREA_LENGTH, 0.0 );break;
    case OBJECT_FLAG_P_L_B:
      pos.setVecPosition( -PITCH_LENGTH/2.0 + PENALTY_AREA_LENGTH,
                                           PENALTY_AREA_WIDTH/2.0 ); break;
    case OBJECT_FLAG_P_R_T:
      pos.setVecPosition(  PITCH_LENGTH/2.0 - PENALTY_AREA_LENGTH,
                                          -PENALTY_AREA_WIDTH/2.0 ); break;
    case OBJECT_FLAG_P_R_C:
      pos.setVecPosition(  PITCH_LENGTH/2.0 - PENALTY_AREA_LENGTH, 0.0 );break;
    case OBJECT_FLAG_P_R_B:
      pos.setVecPosition(  PITCH_LENGTH/2.0 - PENALTY_AREA_LENGTH,
                                           PENALTY_AREA_WIDTH/2.0 ); break;
    case OBJECT_FLAG_C:
      pos.setVecPosition(  0.0 , 0.0 );                              break;
    default:
      cerr << "(SoccerTypes::getGlobalPositionObject) wrong objecttype! " <<
        (int)o << "\n" ;
  }

  if( s == SIDE_RIGHT ) // change side for team on the right side.
      pos.setVecPosition( -pos.getX(), -pos.getY() );
  return pos;
}

/*! This method returns the global angle of a lines on the field.  The
    global angle differs for the left and right side. For both teams
    the line behind the opponent goal is seen with global angle
    0. Only for the left team this is the right line and for the right
    team this is the left line.

    \param o one of the four line objects
    \param s side on which the team was started
    \return AngDeg global angle of this line. */
AngDeg SoccerTypes::getGlobalAngleLine( ObjectT o , SideT s )
{
  AngDeg angle;
  switch( o )
  {
    case OBJECT_LINE_L: angle = 180.0; break;
    case OBJECT_LINE_R: angle =   0.0; break;
    case OBJECT_LINE_T: angle = -90.0; break;
    case OBJECT_LINE_B: angle =  90.0; break;
  default:
    cerr << "(SoccerTypes::getGlobalAngleLine) wrong objecttype! " <<
      (int)o << "\n";
    return UnknownAngleValue;
  }

  if( s == SIDE_RIGHT )
    angle += 180;

  return VecPosition::normalizeAngle( angle );
}

SideT SoccerTypes::getSideFromPlayer(ObjectT o, SideT selfSide)
{
    SideT teammate = selfSide;
    SideT opponent = selfSide == SIDE_LEFT ? SIDE_RIGHT : SIDE_LEFT;
    if (SoccerTypes::isOpponent(o)) return opponent;
    if (SoccerTypes::isTeammate(o)) return teammate;

    return SIDE_ILLEGAL;
}

int SoccerTypes::getNumFromPlayer(ObjectT o)
{
    if (!SoccerTypes::isKnownPlayer(o)) return -1;

    return SoccerTypes::getIndex(o) + 1;
}

/*! This method returns the string representation of a PlayModeT as is used
    in the Robocup Soccer Simulation and also said by the referee.
    \param pm PlayModeT which should be converted
    \return  pointer to the string (enough memory has to be allocated) */
const char* SoccerTypes::getPlayModeStr( PlayModeT pm )
{
  switch( pm )
  {
    case PM_BEFORE_KICK_OFF:          return "before_kick_off";
    case PM_KICK_OFF_LEFT:            return "kick_off_l";
    case PM_KICK_OFF_RIGHT:           return "kick_off_r";
    case PM_KICK_IN_LEFT:             return "kick_in_l";
    case PM_KICK_IN_RIGHT:            return "kick_in_r";
    case PM_CORNER_KICK_LEFT:         return "corner_kick_l";
    case PM_CORNER_KICK_RIGHT:        return "corner_kick_r";
    case PM_GOAL_KICK_LEFT:           return "goal_kick_l";
    case PM_GOAL_KICK_RIGHT:          return "goal_kick_r";
    case PM_GOAL_LEFT:                return "goal_r";
    case PM_GOAL_RIGHT:               return "goal_l";
    case PM_FREE_KICK_FAULT_LEFT:     return "free_kick_fault_l";
    case PM_FREE_KICK_FAULT_RIGHT:    return "free_kick_fault_r";
    case PM_FREE_KICK_LEFT:           return "free_kick_l";
    case PM_FREE_KICK_RIGHT:          return "free_kick_r";
    case PM_INDIRECT_FREE_KICK_RIGHT: return "indirect_free_kick_r";
    case PM_INDIRECT_FREE_KICK_LEFT:  return "indirect_free_kick_l";
    case PM_BACK_PASS_LEFT:           return "back_pass_l";
    case PM_BACK_PASS_RIGHT:          return "back_pass_r";
    case PM_OFFSIDE_LEFT:             return "offside_l";
    case PM_OFFSIDE_RIGHT:            return "offside_l";
    case PM_PENALTY_SETUP_LEFT:       return "penalty_setup_left";
    case PM_PENALTY_SETUP_RIGHT:      return "penalty_setup_right";
    case PM_PENALTY_READY_LEFT:       return "penalty_ready_left";
    case PM_PENALTY_READY_RIGHT:      return "penalty_ready_right";
    case PM_PENALTY_TAKEN_LEFT:       return "penalty_taken_left";
    case PM_PENALTY_TAKEN_RIGHT:      return "penalty_taken_right";
    case PM_PLAY_ON:                  return "play_on";
    case PM_FROZEN:                   return "play_off";
    case PM_QUIT:                     return "quit";
    case PM_ILLEGAL:
    default:                          return NULL;
   }
}

/*! This method returns the play mode associated with a string.
   \param str representing the play mode
   \return PlayModeT of string, PM_ILLEGAL if it is not recognized */
PlayModeT SoccerTypes::getPlayModeFromStr( char* str )
{
  // all play modes are sent as referee message, so get referee message
  // and look up the associated play mode
  return getPlayModeFromRefereeMessage( getRefereeMessageFromStr( str ) );
}

/*! This method returns the play mode from the referee message.
    \param rm RefereeMessage that contains the play mode
    \return PlayModeT of RefereeMessage, PM_ILLEGAL if it is not recognized */
PlayModeT SoccerTypes::getPlayModeFromRefereeMessage( RefereeMessageT rm )
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

/*! This method returns the string representation of a RefereeMessageT as is
    used in the Robocup Soccer Simulation and said by the referee.
    \param pm RefereeMessageT which should be converted
    \return  pointer to the string (enough memory should be allocated) */
const char* SoccerTypes::getRefereeMessageStr( RefereeMessageT rm )
{
  switch( rm )
  {
    case REFC_BEFORE_KICK_OFF:          return "before_kick_off";
    case REFC_KICK_OFF_LEFT:            return "kick_off_l";
    case REFC_KICK_OFF_RIGHT:           return "kick_off_r";
    case REFC_KICK_IN_LEFT:             return "kick_in_l";
    case REFC_KICK_IN_RIGHT:            return "kick_in_r";
    case REFC_CORNER_KICK_LEFT:         return "corner_kick_l";
    case REFC_CORNER_KICK_RIGHT:        return "corner_kick_r";
    case REFC_GOAL_KICK_LEFT:           return "goal_kick_l";
    case REFC_GOAL_KICK_RIGHT:          return "goal_kick_r";
    case REFC_FREE_KICK_LEFT:           return "free_kick_l";
    case REFC_FREE_KICK_RIGHT:          return "free_kick_r";
    case REFC_INDIRECT_FREE_KICK_LEFT:  return "indirect_free_kick_l";
    case REFC_INDIRECT_FREE_KICK_RIGHT: return "indirect_free_kick_r";
    case REFC_FREE_KICK_FAULT_LEFT:     return "free_kick_fault_l";
    case REFC_FREE_KICK_FAULT_RIGHT:    return "free_kick_fault_r";
    case REFC_BACK_PASS_LEFT:           return "back_pass_l";
    case REFC_BACK_PASS_RIGHT:          return "back_pass_r";
    case REFC_PLAY_ON:                  return "play_on";
    case REFC_FROZEN:                   return "play_off";
    case REFC_QUIT:                     return "quit";
    case REFC_OFFSIDE_LEFT:             return "offside_l";
    case REFC_OFFSIDE_RIGHT:            return "offside_r";
    case REFC_HALF_TIME:                return "half_time";
    case REFC_TIME_UP:                  return "time_up";
    case REFC_TIME_OVER:                return "time_over";
    case REFC_TIME_UP_WITHOUT_A_TEAM:   return "time_up_without_a_team";
    case REFC_TIME_EXTENDED:            return "time_extended";
    case REFC_FOUL_LEFT:                return "foul_l";
    case REFC_FOUL_RIGHT:               return "foul_r";
    case REFC_GOAL_LEFT:                return "goal_l";
    case REFC_GOAL_RIGHT:               return "goal_r";
    case REFC_DROP_BALL:                return "drop_ball";
    case REFC_GOALIE_CATCH_BALL_LEFT:   return "goalie_catch_ball_l";
    case REFC_GOALIE_CATCH_BALL_RIGHT:  return "goalie_catch_ball_r";
    case REFC_PENALTY_SETUP_LEFT:       return "penalty_setup_left";
    case REFC_PENALTY_SETUP_RIGHT:      return "penalty_setup_right";
    case REFC_PENALTY_READY_LEFT:       return "penalty_ready_left";
    case REFC_PENALTY_READY_RIGHT:      return "penalty_ready_right";
    case REFC_PENALTY_TAKEN_LEFT:       return "penalty_taken_left";
    case REFC_PENALTY_TAKEN_RIGHT:      return "penalty_taken_right";
    case REFC_PENALTY_MISS_LEFT:        return "penalty_miss_left";
    case REFC_PENALTY_MISS_RIGHT:       return "penalty_miss_right";
    case REFC_PENALTY_SCORE_LEFT:       return "penalty_score_left";
    case REFC_PENALTY_SCORE_RIGHT:      return "penalty_score_right";
    case REFC_PENALTY_FOUL_LEFT:        return "penalty_foul_left";
    case REFC_PENALTY_FOUL_RIGHT:       return "penalty_foul_right";
    case REFC_PENALTY_ONFIELD_LEFT:     return "penalty_onfield_left";
    case REFC_PENALTY_ONFIELD_RIGHT:    return "penalty_onfield_right";
    case REFC_PENALTY_WINNER_LEFT:      return "penalty_winner_l";
    case REFC_PENALTY_WINNER_RIGHT:     return "penalty_winner_r";
    case REFC_PENALTY_DRAW:             return "penalty_draw";
    case REFC_ILLEGAL:
    default:                            return NULL;
  }
}


/*! This method returns the referee message from the string that is passed.
    \param str pointer to a string with the referee message starting at index 0

    \return RefereeMessageT of string representation, REFC_ILLEGAL if
    it fails */
RefereeMessageT SoccerTypes::getRefereeMessageFromStr( char* str )
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
      printf("(SoccerTypes::getRefereeMessageFromStr) ?? ref msg %s\n",str);
      return REFC_ILLEGAL;
  }
  printf("(SoccerTypes::getRefereeMessageFromStr) ?? ref msg %s\n",str);
  return REFC_ILLEGAL;
}


/*! This method returns the string representation of a ViewAngleT as is used
    in the Robocup Soccer Simulation
    \param va ViewAngleT which should be converted
    \return  pointer to the string (enough memory should be allocatd) */
const char* SoccerTypes::getViewAngleStr( ViewAngleT va )
{
  switch( va )
  {
    case VA_NARROW:  return "narrow";
    case VA_NORMAL:  return "normal";
    case VA_WIDE:    return "wide";
    case VA_ILLEGAL:
    default:         return NULL;
  }
}

/*! This method returns et the view angle from the specified string
    \param str pointer to a string that contains view angle string at index 0
    \return ViewAngleT of string, VA_ILLEGAL if it is not recognized */
ViewAngleT SoccerTypes::getViewAngleFromStr( char* str )
{
  switch( str[1] )
  {
    case 'a': return VA_NARROW;
    case 'o': return VA_NORMAL;
    case 'i': return VA_WIDE;
    default:  return VA_ILLEGAL;
  }
}

/*! This method returns the half angle value that belongs to the
    ViewAngle that is given as the first argument (VA_NARROW,
    VA_NORMAL or VA_WIDE). The half view angle is returned since this
    makes it easier to check whether an object lies in the view cone
    (the global relative angle must be smaller than the half view
    angle.

    \param va view angle
    \return angle denoting the half of the corresponding view angle */
AngDeg SoccerTypes::getHalfViewAngleValue( ViewAngleT va )
{
  switch( va )
  {
    case VA_NARROW:  return 22.5;
    case VA_NORMAL:  return 45.0;
    case VA_WIDE:    return 90.0;
    case VA_ILLEGAL:
    default:         return 0.0;
  }

}

/*! This method returns the string representation of a ViewQualityT as is used
    in the Robocup Soccer Simulation
    \param vq ViewQualityT which should be converted
    \return  pointer to the string (enough memory should be allocated) */
const char* SoccerTypes::getViewQualityStr( ViewQualityT vq )
{
  switch( vq )
  {
    case VQ_HIGH:    return "high";
    case VQ_LOW:     return "low";
    case VQ_ILLEGAL:
    default:         return NULL;
  }
}

/*! This method returns the view quality from the string that is passed as the
    first argument
   \param str pointer to a string that contains view quality string at index 0
   \return ViewQualityT of string, VQ_ILLEGAL if it is not known */
ViewQualityT SoccerTypes::getViewQualityFromStr( char* str )
{
    if( str[0] == 'h' )      return VQ_HIGH;
    else if( str[0] == 'l' ) return VQ_LOW;
    else                     return VQ_ILLEGAL;
}

/*! This method returns the string representation of a CommandT as is
    used in the Robocup Soccer Simulation
    \param com CommandT that should be converted
    \return  pointer to the string (enough memory should be allocated) */
const char* SoccerTypes::getCommandStr( CommandT com )
{
  switch( com )
  {
    case CMD_DASH:        return "dash";
    case CMD_TURN:        return "turn";
    case CMD_TURNNECK:    return "turn_neck";
    case CMD_CHANGEVIEW:  return "change_view";
    case CMD_CATCH:       return "catch";
    case CMD_KICK:        return "kick";
    case CMD_MOVE:        return "move";
    case CMD_SENSEBODY:   return "sense_body";
    case CMD_SAY:         return "neck";
    case CMD_ATTENTIONTO: return "attentionto";
    case CMD_TACKLE:      return "tackle";
    case CMD_POINTTO:     return "pointto";
    case CMD_ILLEGAL:
    default:               return NULL;

  }
}

/*! This method returns return true when argument is a primary action
    (action that can only be sent once a cycle). This is the case for kick,
    dash, move, turn and catch commands.
    \param CommandT command that should be checked
    \return true when it is a primary action, false otherwise */
bool SoccerTypes::isPrimaryCommand( CommandT com )
{
  return com == CMD_KICK || com == CMD_DASH || com == CMD_MOVE ||
         com == CMD_TURN || com == CMD_CATCH || com == CMD_TACKLE ;
}


/*! This method returns the string representation of a SideT as is used in the
    Robocup Soccer Simulation (r or l).
    \param s SideT which should be converted
    \return pointer to the string */
const char* SoccerTypes::getSideStr( SideT s )
{
  switch( s )
  {
    case SIDE_LEFT:    return "l";
    case SIDE_RIGHT:   return "r";
    case SIDE_ILLEGAL:
    default:           return NULL;
  }
}

/*! This method returns the SideT from the string that is passed as the first
    argument.
    \param str pointer to a string that contains side info string at index 0
    \return SideT of string representation, SIDE_ILLEGAL if it is not known */
SideT SoccerTypes::getSideFromStr( char* str )
{
  if( str[0] == 'l' )      return SIDE_LEFT;
  else if( str[0] == 'r' ) return SIDE_RIGHT;
  else                     return SIDE_ILLEGAL;

}
