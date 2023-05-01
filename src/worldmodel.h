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

using namespace std;

class WorldModel
{
    pthread_mutex_t  mutex_newInfo;       /*!< mutex to protect newInfo       */
    pthread_cond_t   cond_newInfo;        /*!< cond variable for newInfo      */

    Formations *fm;

    string teamName;

    Agent agent;

        PlayerObject teammates[MAX_TEAMMATES];
        PlayerObject opponents[MAX_OPPONENTS];


    PlayerObject unknownPlayers[MAX_TEAMMATES + MAX_OPPONENTS];

    FixedObject flags[MAX_FLAGS];
    FixedObject lines[MAX_LINES];

    list<FixedObject> viewedFlags;

    vector<PlayerObject> unsureAll;
    vector<PlayerObject> unsureTeammates;
    vector<PlayerObject> unsureOpponents;

    list<PlayerObject> listUnsureAll;
    list<PlayerObject> listUnsureTeammates;
    list<PlayerObject> listUnsureOpponents;

    int unknownCount;

    int agentNum;
    SideT side;

    PlayModeT playMode;

    char    lastSeeMsg[MAX_MSG];
    int     lastSeeTime;
    bool    newSeeInfo;
    char    lastSenseMsg[MAX_MSG];
    int     lastSenseTime;
    bool    newSenseInfo;
    bool    newInfo;

    std::ostringstream extraInfo;
public:
    WorldModel(Formations *fm);

    void show(ostream &os = cout);
    void showAgent(ostream &os = cout);
    string getExtraInfo();

    int getCurrentTime();

    Agent &getAgent();

    bool waitNewInfo();

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
    bool   mapBeforeKickOff();
    bool   mapInGame();
    bool   mapTeammates();
    bool   mapOpponents();
    bool   mapUnsure();

    void   decideUnknownOpponentBeforeKickOff(VisiblePlayer &vp);

    int    findFirstEmptyPlayer(PlayerObject* arr, int count);
    int    findClosestPlayer(PlayerObject* arr, int count, VecPosition pos);

    int    findClosestForLastSee(PlayerObject* arr, int count, VecPosition pos);
    bool   mapToClosest(PlayerObject player, PlayerObject* arr, int count);
};

#endif // WORLDMODEL_H
