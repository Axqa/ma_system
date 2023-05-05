#ifndef PLAYER_H
#define PLAYER_H

#include "worldmodel.h"
#include "logger.h"
#include "connection.h"
#include "acthandler.h"
#include "utilities.h"
#include <string>
#include "formation.h"
#include "strategy.h"

void* stdin_callback( void * v );

class Player
{
    Formations *fm;
    WorldModel *wm;
    ActHandler *act;
    Strategy *st;

    double version;

    bool alive;
    bool movedToFormationPos;

    MoveT moveType;

    VecPosition target;

    bool isGoalie;
public:
    Player(WorldModel *wm, ActHandler *act, Formations *fm, Strategy *st,
           string teamName, double version, MoveT moveType = OBSERVE);

    void mainLoop();
    void doMove();
    void LineMover();
    void observeMover();

    void handleStdin();
    void executeStringCommand(char* cmd);
};

#endif // PLAYER_H
