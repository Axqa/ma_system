#ifndef WORLDMODEL_H
#define WORLDMODEL_H

#include <iostream>
#include <string>
#include "objects.h"
#include "utilities.h"
#include "formation.h"
#include <sstream>
#include <vector>
#include <list>
#include "playermapper.h"
#include "strategy.h"

using namespace std;

/*!
    This class contains all the information about field used by agent.
    Provides methods to update world model with new information and
    process enw information.
*/
class WorldModel
{
    pthread_mutex_t  mutex_newInfo;       /*!< mutex to protect newInfo       */
    pthread_cond_t   cond_newInfo;        /*!< cond variable for newInfo      */

    Formations *fm;     /*!< reference to formation used */
    PlayerMapper *pm;   /*!< reference to player mappping algorithm used */
    Strategy *st;       /*!< reference to team strategy used */

    string teamName;    /*!< Team name */

    Agent agent;        /*!< Information about agent */

    PlayerObject teammates[MAX_TEAMMATES];  /*!< informations about teammates */
    PlayerObject opponents[MAX_OPPONENTS];  /*!< informations about opponents */

    PlayerObject unknownPlayers[MAX_TEAMMATES + MAX_OPPONENTS];
                                            /*!< info about players seen last cycle */

    FixedObject flags[MAX_FLAGS];   /*!< information about flags and last vision */
    FixedObject lines[MAX_LINES];   /*!< information about lines and last vision */

    list<FixedObject> viewedFlags;  /*!< info about flags viewed last cycle */

    int unknownCount;   /*!< number of seen players last cycle */

    int agentNum;       /*!< agent's team number */
    SideT side;         /*!< agent's team side */

    PlayModeT playMode; /*!< current play mode */

    char    lastSeeMsg[MAX_MSG];    /*!< see message for agent */
    int     lastSeeTime;            /*!< cycle for last see message */
    bool    newSeeInfo;             /*!< indicates whether new see info received */
    char    lastSenseMsg[MAX_MSG];  /*!< sense message for agent */
    int     lastSenseTime;          /*!< cycle for last sense message */
    bool    newSenseInfo;           /*!< indicates whether new sense info received */
    bool    newInfo;                /*!< indicates whether new info available */

    std::ostringstream extraInfo;   /*!< buffer for extra info */
public:
    WorldModel(Formations *fm, PlayerMapper* pm, Strategy *st);

    void show(ostream &os = cout);
    void showAgent(ostream &os = cout);
    string getExtraInfo();

    int getCurrentTime();
    void setCurrentTime(int time);

    Agent &getAgent();

    bool waitNewInfo();

    FixedObject *getLines();

    void setAgentNum(int num);
    int getAgentNum();

    SideT getSide();
    void  setSide(SideT s);

    string getTeamName();
    void   setTeamName(string name);

    PlayModeT getPlayMode();
    void      setPlayMode(PlayModeT pm);

    bool   getNewInfo();
    void   setNewInfo(bool bInfo);

    void   setSeeMessage(char* msg, int time);
    char*  getSeeMessage();

    void   setSenseMessage(char* msg, int time);
    char*  getSenseMessage();

    void   updateFormationToSide();

    bool   clearInfo();
    bool   update(bool needMap = false);

    bool   processLastSeeMsg();
    bool   processLastSenseMsg();

    bool   processFlagInfo(ObjectT flag, int time, double dist, int ang);
    bool   processPlayerInfo(ObjectT        player,
                             int            time,
                             double         dDist,
                             int            iDir,
                             double         dDistChange,
                             double         dDirChange,
                             AngDeg         angRelBodyAng,
                             AngDeg         angRelNeckAng,
                             bool           isGoalie);
    bool   processLineInfo(ObjectT line, int time, double dist, int ang);

    bool   calculateAllPos(bool needUpdateOther = false);
    bool   calculateAgentPos();
    void   updateAllUnknown();

    bool   mapUnknownPlayers();

    bool   updatePredictions();
    void   calcPrediction(PlayerObject *player);

    void   predictTeammateWithStrategy(PlayerObject *player);
};

#endif // WORLDMODEL_H
