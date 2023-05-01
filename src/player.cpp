#include "player.h"

#include <string.h>
#include <stdlib.h>

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
        if (/*wm->getNewInfo() &&*/ wm->update(moveType==OBSERVE))
        {
//            wm->setNewInfo(false);
            if (wm->getPlayMode() == PM_BEFORE_KICK_OFF)
            {
                if (movedToFormationPos == false) {
                    act->sendMoveCmd(fm->getAgentPos());
                    movedToFormationPos = true;
                    continue;
                }

                if (wm->getAgent().getViewAngle() != VA_WIDE)
                {
                    act->sendChangeViewCmd(VA_WIDE);
                    continue;
                }

                if (abs(wm->getAgent().getAbsNeckAngle()) > EPSILON)
                {
                    act->sendTurnCmd(-wm->getAgent().getAbsNeckAngle());
                    continue;
                }
            }
            else if ((wm->getPlayMode() == PM_KICK_OFF_LEFT && wm->getSide() == SIDE_LEFT) ||
                     (wm->getPlayMode() == PM_KICK_OFF_RIGHT && wm->getSide() == SIDE_RIGHT))
            {
                if (fm->getPlayerNum() == 9) // assume 9 player always kick off ball
                {   // weakly kick ball
                    act->sendKickCmd(10, 0);
                    continue;
                }
            }
            else
            {
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
        break;

    case LINE:
        LineMover();
        break;

    default:
        break;
    }
}

void Player::LineMover()
{
    if (target == VecPosition() || target.getDistanceTo(wm->getAgent().getAbsPos()) < 10)
    {
        srand(fm->getPlayerNum() * (1 + wm->getSide()));
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

void Player::handleStdin()
{
    char buf[MAX_MSG];

    while( alive )
    {

      fgets( buf, MAX_MSG, stdin ); // does unblock with signal !!!!!

      if (buf[0] != '\0')
      {
         printf( "Get command: %s\n", buf );
         executeStringCommand( buf );
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
