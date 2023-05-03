#include "player.h"

#include <string.h>
#include <stdlib.h>
#include "soccertypes.h"

extern Logger Log;

void* stdin_callback( void * v )
{
  Log.log( 1, "Starting to listen for user input" );
  Player* p = (Player*)v;
  p->handleStdin();
  return 0;
}

Player::Player(WorldModel *wm, ActHandler *act, Formations *fm, string teamName, double version, MoveT moveType)
    : fm(fm), wm(wm), act(act), version(version), alive(true), moveType(moveType)
{
    char str[MAX_MSG];

    movedToFormationPos = false;

    wm->setTeamName(teamName);

    sprintf(str, "(init %s (version %f))", teamName.data(), version);
    act->sendMessage(str);

    isGoalie = fm->getPlayerNum() == 1;
}

void Player::mainLoop()
{
    if(  wm->waitNewInfo() == false )
      alive =  false;

    while (alive)
    {
        if (/*wm->getNewInfo() &&*/ wm->update(moveType==OBSERVE || moveType==NO_MOVE_OBSERVE))
        {
//            wm->setNewInfo(false);

            if (wm->getPlayMode() == PM_BEFORE_KICK_OFF)
            {

                if (movedToFormationPos == false) {
                    if (wm->getSide() == SIDE_LEFT && wm->getCurrentTime() == 0)
                    {
                        fm->setFormationIndex(0);
                    }
                    else
                    {
                        fm->setFormationIndex(1);
                    }

                    act->sendMoveCmd(fm->getAgentPos());
                    movedToFormationPos = true;
                    continue;
                }

                if (wm->getAgent().getViewAngle() != VA_WIDE)
                {
                    act->sendChangeViewCmd(VA_WIDE);
                    continue;
                }

                if (abs(wm->getAgent().getAbsBodyAngle()) > EPSILON)
                {
                    act->sendTurnCmd(-wm->getAgent().getAbsBodyAngle());
                    continue;
                }

                static int cycle = 0;
                switch (cycle) {
                case 0:
                    act->sendTurnNeckCmd(-90);
                    cycle++;
                    break;
                case 1:
                    act->sendTurnNeckCmd(180);
                    cycle++;
                    break;
                case 2:
                    act->sendTurnNeckCmd(-90);
                    cycle++;
                    break;
                default:
                    break;

                }
            }
            else if ((wm->getPlayMode() == PM_KICK_OFF_LEFT && wm->getSide() == SIDE_LEFT) ||
                     (wm->getPlayMode() == PM_KICK_OFF_RIGHT && wm->getSide() == SIDE_RIGHT))
            {
                if (fm->getPlayerNum() == 9) // assume 9 player always kick off ball
                {   // weakly kick ball
                    act->sendKickCmd(100, 0);
                    continue;
                }
            }
            else
            {
                movedToFormationPos = false;
                doMove();
            }
        }
        if(  wm->waitNewInfo() == false )
          alive =  false;
    }
}

void Player::doMove()
{
    switch (moveType)
    {
    case OBSERVE:
        observeMover();
        break;

    case LINE:
        LineMover();
        break;

    case NO_MOVE:
        break;

    default:
        break;
    }
}

void Player::LineMover()
{
    if (target == VecPosition() || target.getDistanceTo(wm->getAgent().getAbsPos()) < 10)
    {
        target = VecPosition((rand() % (int)PITCH_LENGTH) - (int)PITCH_LENGTH/2, (rand() % (int)PITCH_WIDTH) - (int)PITCH_WIDTH/2);
    }

    // We do not turning neck, so global neck angle == global body angle
    VecPosition dir = target - wm->getAgent().getAbsPos();
    AngDeg angToTarget = atan2Deg(dir.getY(), dir.getX());
    if (abs(angToTarget - wm->getAgent().getAbsNeckAngle()) > 5)
    {
        act->sendTurnCmd(angToTarget - wm->getAgent().getAbsNeckAngle());
        return;
    }

    // If we look in necessary direction, just dash
    act->sendDashCmd(PLAYER_MAX_POWER);
    return;
}

void Player::observeMover()
{
    static bool lookRight = false;
//    static int curAngle = 0;
//    AngDeg absTargetAngle;

////    absTargetAngle = getBisectorTwoAngles()
//    FixedObject *line = nullptr;
//    for(int i = 0; i < MAX_LINES; ++i)
//    {
//        // we dont move so line info should be same for all time
//        if (wm->getLines()[i].getTime() != -1 && (line == nullptr || (wm->getLines()[i].getRelPos().getX() < line->getRelPos().getX())))
//        {
//            line = &wm->getLines()[i];
//        }
//    }

//    absTargetAngle = SoccerTypes::getGlobalAngleLine(line->getType(), wm->getSide());

//    AngDeg viewAngle = SoccerTypes::getHalfViewAngleValue(wm->getAgent().getViewAngle()) * 2;

//    if (!equal(wm->getAgent().getAbsBodyAngle(), absTargetAngle))
//    {
//        act->sendTurnCmd(absTargetAngle - wm->getAgent().getAbsBodyAngle());
//        return;
//    }



    if (equal(wm->getAgent().getNeckToBodyAngle(), 0) )
    {
        act->sendTurnNeckCmd(90);
        lookRight = true;
        return;
    }
    if (lookRight)
    {
        act->sendTurnNeckCmd(-180);
        lookRight = false;
    }
    else
    {
        act->sendTurnNeckCmd(180);
        lookRight = true;
    }

}

void Player::handleStdin()
{
    char buf[MAX_MSG];

    while( alive )
    {

      fgets( buf, MAX_MSG, stdin ); // does unblock with signal !!!!!

      if (buf[0] != '\0')
      {
         printf( "Get command: %s\n", buf );
         try {
             executeStringCommand( buf );

         } catch (exception e) {
             cout << "Error in handling input: " << e.what() << endl;
         }
      }
    }
}

void Player::executeStringCommand(char *cmd)
{
    if (cmd[0] == '(')
    {
        act->sendMessage(cmd);
        return;
    }

    switch (cmd[0])
    {
        case 'a':
            wm->showAgent();
            break;
        case 'w':
            wm->show();
            break;
        case 'l':
            cout << wm->getSeeMessage() << endl;
            break;
        case 's':
            cout << wm->getSenseMessage() << endl;
            break;
        case 'e':
            cout << wm->getExtraInfo() << endl;
        default:
            break;
    }
}
