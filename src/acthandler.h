#ifndef ACTHANDLER_H
#define ACTHANDLER_H

#include "connection.h"
#include "worldmodel.h"

class ActHandler
{
    Connection* cn;
    WorldModel* wm;

    int lastSent;
public:
    ActHandler(Connection* cn, WorldModel* wm);

    bool sendMessage(string msg );

    bool sendMoveCmd(VecPosition pos);
    bool sendChangeViewCmd(ViewAngleT va);
    bool sendTurnCmd(int angle);
    bool sendDashCmd(int power);
    bool sendKickCmd(int power, int direction);
    bool sendTurnNeckCmd(int angle);
};

#endif // ACTHANDLER_H
