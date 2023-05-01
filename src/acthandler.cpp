#include "acthandler.h"

#include <unistd.h>
#include "soccertypes.h"

ActHandler::ActHandler(Connection* cn, WorldModel* wm)
    : cn(cn), wm(wm)
{
    lastSent = -1;
}

bool ActHandler::sendMessage(string msg)
{

    usleep(50000); // wait for 0.05 sec

    bool ret = cn->sendMessage( msg.data() );

    usleep(50000); // wait for 0.05 sec

    return ret;
}

bool ActHandler::sendMoveCmd(VecPosition pos)
{
    char msg[MAX_MSG];
    sprintf(msg, "(move %f %f)", pos.getX(), pos.getY());
    cn->sendMessage(msg);
    return true;
}

bool ActHandler::sendChangeViewCmd(ViewAngleT va)
{
    char msg[MAX_MSG];
    sprintf(msg, "(change_view %s %s)", SoccerTypes::getViewAngleStr(va), "high");
    cn->sendMessage(msg);
    return true;
}

bool ActHandler::sendTurnCmd(int angle)
{
    char msg[MAX_MSG];
    sprintf(msg, "(turn %d)", angle);
    cn->sendMessage(msg);
    return true;
}

bool ActHandler::sendDashCmd(int power)
{
    char msg[MAX_MSG];
    sprintf(msg, "(dash %d)", power);
    cn->sendMessage(msg);
    return true;
}

bool ActHandler::sendKickCmd(int power, int direction)
{
    char msg[MAX_MSG];
    sprintf(msg, "(kick %d %d)", power, direction);
    cn->sendMessage(msg);
    return true;
}
