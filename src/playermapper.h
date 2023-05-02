#ifndef PLAYERMAPPER_H
#define PLAYERMAPPER_H
#include <iostream>
#include <string>
#include "objects.h"
#include "utilities.h"
#include "formation.h"
#include <sstream>
#include <vector>
#include <list>


class PlayerMapper
{
protected:

    Formations *fm;

    Agent *agent;

    PlayerObject *teammates;
    PlayerObject *opponents;

    PlayerObject *unknownPlayers;

    SideT agentSide;

    int curTime;
public:
    PlayerMapper();

    void init(Formations *fm,
              Agent *agent,
              PlayerObject *teammates,
              PlayerObject *opponents,
              PlayerObject *unknownPlayers);

    SideT getSide();
    void  setSide(SideT side);

    int   getCurrentTime();
    void  setCurrentTime(int time);

    virtual bool  mapBeforeKickOff(int unknownCount);
    virtual bool  mapInGame(int unknownCount) = 0;

protected:
    void   decideUnknownOpponentBeforeKickOff(VisiblePlayer &vp);

    int    findFirstEmptyPlayer(PlayerObject* arr, int count);
    int    findClosestPlayer(PlayerObject* arr, int count, VecPosition pos);
    int    findClosestUnsurePlayer(PlayerObject* arr, int count, VecPosition pos);
};

class ClosestMapper : public PlayerMapper
{
protected:
    vector<PlayerObject> unsureAll;
    vector<PlayerObject> unsureTeammates;
    vector<PlayerObject> unsureOpponents;

    list<PlayerObject> listUnsureAll;
    list<PlayerObject> listUnsureTeammates;
    list<PlayerObject> listUnsureOpponents;
public:
    ClosestMapper();
    bool   mapInGame(int unknownCount) override;

protected:
    bool   mapTeammates();
    bool   mapOpponents();
    bool   mapUnsure();

    int    findClosestForLastSee(PlayerObject* arr, int count, VecPosition pos);
    bool   mapToClosest(PlayerObject player, PlayerObject* arr, int count);
    bool   mapToClosestFromAll(PlayerObject player, PlayerObject* arr, int count);
};

#endif // PLAYERMAPPER_H
