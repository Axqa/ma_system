#include "messagehandler.h"
#include <cstdio>
#include <string.h>
#include "parser.h"
#include "utilities.h"
#include "logger.h"
#include "soccertypes.h"

extern Logger Log;

void *message_callback(void *v)
{
    MessageHandler* s = (MessageHandler*)v;
    s->handleMessagesFromServer( );

    return 0;
}


MessageHandler::MessageHandler(WorldModel *wm, Connection *cn)
    : wm(wm), cn(cn)
{
}

void MessageHandler::handleMessagesFromServer()
{
    char strBuf[MAX_MSG];
    int i=0;

    while( 1 )
    {
      strBuf[0]='\0';
      if( i != -1 )                                         // if no error
        i = cn->receiveMessage( strBuf, MAX_MSG );          // get message
      if( strBuf[0] != '\0' )                               // if not empty
        analyzeMessage( strBuf );                           // parse message
    }
}

bool MessageHandler::analyzeMessage(char* msg)
{
    Log.log(1, "%s", msg);

    bool bReturn = false;

    switch( msg[1] )
    {
      case 'c':
          if( msg[2] == 'h' )
            ;      // ( c hange_
          else
            ;      // (clang
          break;
      case 'f':    // ( f ullstate_
          break;
      case 'o':    // ( o k
          if( strlen(msg) > 14 && msg[4] == 'c' && msg[10] == 'b' )
            ;      // (ok check_ball
          break;
      case 's':
      {
        switch( msg[3] )
        {
        case 'e':
          if( msg[5] == 'g')
            ;      // (se e_global // only with fullState option
          else
            return analyzeSeeMessage ( msg ); // (se e
          break;
        case 'n':  // (se n se
            return analyzeSenseMessage(msg);
        case 'r': ;//return analyzeServerParamMessage( msg ); // (se r ver_param
        default : break;
        }
      }
          break;
      case 'i':     return analyzeInitMessage( msg ); // ( i nit
      case 'h':     return analyzeHearMessage(msg); // ( h ear
      case 'p':     break;//return ( msg[8] == 't')
                    //? analyzePlayerTypeMessage ( msg )  // (player_ t ype
                    //: analyzePlayerParamMessage( msg ); // (player_ p aram
      case 'e':     printf("Error: (t: %d, num: %d) %s\n", wm->getCurrentTime(),
                             wm->getAgentNum(), msg);// ( error
          break;
      case 't':               // ( think
          break;
      default:      cerr << "(" << wm->getCurrentTime() << ", " <<
                          wm->getAgentNum()
                          << ") " <<
                       "ignored message: " << msg << "\n";
                    return false;
    }
    return false;
}

/*! This method analyzes an init message. All information from the
    initialization is parsed and updated in the WorldModel.
    An init message looks like (init [l|r] 10 before_kick_off)
    \param strMsg message that should be parsed
    \return bool indicating whether the message was parsed correctly. */
bool MessageHandler::analyzeInitMessage( char *msg )
{
//  Log.log( 999, "%s", msg );
  msg += 6;                                            // go to Side
  wm->setSide( SoccerTypes::getSideFromStr( msg ) );   // get and set Side
  int nr = Parse::parseFirstInt( &msg );               // get and set number

  wm->setAgentNum( nr );
  msg++;                                               // skip space to pm
  wm->setPlayMode( SoccerTypes::getPlayModeFromStr( msg ) ); // get playmode

  wm->updateFormationToSide();
  return true;
}

bool MessageHandler::analyzeSeeMessage(char *msg)
{
    char *str = msg;
    int time = Parse::parseFirstInt(&msg);

    wm->setSeeMessage(str, time);
    return true;
}

/*! This method analyzes a hear message. When the message is from the
    referee the message is parsed and the new play mode is set or the
    goal difference is adjusted.
    looks like (hear 0 self|referee|dir message)

    \param msg message that should be parsed
    \return bool indicating whether the message was parsed correctly. */
bool MessageHandler::analyzeHearMessage(char *msg)
{
  RefereeMessageT rm;
  PlayModeT       pm;

  int time = Parse::parseFirstInt( &msg );              // ignore time

  switch( Parse::gotoFirstNonSpace( &msg ) )
  {
    case 'r':                                               // referee
//      wm->setTimeLastRefereeMessage( time );
      Parse::gotoFirstOccurenceOf( ' ', &msg );          // go to start
      Parse::gotoFirstNonSpace   ( &msg      );          // and first part
      rm = SoccerTypes::getRefereeMessageFromStr( msg ); // get the ref msg

      pm = SoccerTypes::getPlayModeFromRefereeMessage( rm );// get play mode
      if( pm != PM_ILLEGAL )                                // from ref msg
        wm->setPlayMode( pm );                              // if was pm, set

      switch( rm )
      {
        case REFC_GOAL_LEFT:                            // goal left

          break;
        case REFC_GOAL_RIGHT:                      // goal right

          break;
        case REFC_GOALIE_CATCH_BALL_LEFT:         // catch ball
        case REFC_GOALIE_CATCH_BALL_RIGHT:
          break;
        case REFC_PENALTY_ONFIELD_LEFT:
          break;
        case REFC_PENALTY_ONFIELD_RIGHT:
          break;
        case REFC_PENALTY_MISS_LEFT:
        case REFC_PENALTY_SCORE_LEFT:
          wm->setPlayMode( PM_FROZEN );
          break;
        case REFC_PENALTY_MISS_RIGHT:
        case REFC_PENALTY_SCORE_RIGHT:
          wm->setPlayMode( PM_FROZEN );
          break;
        case REFC_PENALTY_FOUL_LEFT:
        case REFC_PENALTY_FOUL_RIGHT:
        default:
          break;
      }
      break;
    case 'o':                                               // online_coach_
        break;
    case 's':                                               // self
      break;                                                // do nothing
    default:                                                // from direction

      break;
  }

  return true;
}

/*! This method analyzes a sense message. All information from the player is
    parsed and updated in the WorldModel.
    A sense message looks like (sense_body 0 (view_mode high normal)
    (stamina 2000 1) (speed 0 0) (head_angle 0) (kick 0) (dash 0)
    (turn 0) (say 0) (turn_neck 0) (catch 0) (move 0) (change_view 0))
    \param strMsg message that should be parsed
    \return bool indicating whether the message was parsed correctly. */
bool MessageHandler::analyzeSenseMessage(char *msg)
{
    char *str = msg;
    int time = Parse::parseFirstInt( &msg );// get time

    wm->setSenseMessage(str, time);

    return true;
}
