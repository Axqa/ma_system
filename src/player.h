#ifndef PLAYER_H
#define PLAYER_H

#include "worldmodel.h"
#include "logger.h"
#include "connection.h"
#include "acthandler.h"
#include "utilities.h"
#include <string>
#include "formation.h"

void* stdin_callback( void * v );

class Player
{
    Formations *fm;
    WorldModel *wm;
    ActHandler *act;

    double version;

    bool alive;
    bool movedToFormationPos;

    MoveT moveType;

    VecPosition target;

    bool isGoalie;
public:
    Player(WorldModel *wm, ActHandler *act, Formations *fm, string teamName, double version, MoveT moveType = OBSERVE);

    void mainLoop();
    void doMove();
    void LineMover();

    void handleStdin();
    void executeStringCommand(char* cmd);
};

#endif // PLAYER_H
