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

/*!
    Class Player provide main player cycle and controls
    agent movement.
*/
class Player
{
    Formations *fm;     /*!< reference to formation information */
    WorldModel *wm;     /*!< reference to world model */
    ActHandler *act;    /*!< reference to ActHandler used  */
    Strategy *st;       /*!< reference to team strategy */

    double version;     /*!< client version */

    bool alive;         /*!< is server alive */
    bool movedToFormationPos;   /*!< indicates whether player was moved to pos */

    MoveT moveType;     /*!< player's move type */

    VecPosition target; /*!< position of current target */

    bool isGoalie;      /*!< is player a goalie */
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
