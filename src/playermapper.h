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

    // pair <player, unknownPlayer>
    vector<pair<PlayerObject*, PlayerObject*>> mapTo;

    SideT agentSide;

    int curTime;
public:
    PlayerMapper();

    void init(Formations *fm,
              Agent *agent,
              PlayerObject *teammates,
              PlayerObject *opponents,
              PlayerObject *unknownPlayers);

    // unknown to player
    vector<pair<PlayerObject*, PlayerObject*>> getMapTo();

    SideT getSide();
    void  setSide(SideT side);

    int   getCurrentTime();
    void  setCurrentTime(int time);

    virtual bool  mapBeforeKickOff(int unknownCount);
    virtual bool  mapInGame(int unknownCount) = 0;

    virtual bool  provideMap();
protected:
    virtual VecPosition getPos(PlayerObject &player);

    void   addToMap(PlayerObject* player, PlayerObject* unknown);

    void   decideUnknownOpponentBeforeKickOff(PlayerObject *p);

    int    findFirstEmptyPlayer(PlayerObject* arr, int count);
    int    findClosestPlayer(PlayerObject* arr, int count, VecPosition pos);
    int    findClosestUnsurePlayer(PlayerObject* arr, int count, VecPosition pos);
    int    findClosestUnmapped(PlayerObject* arr, int count, VecPosition pos);

    bool   isMapped(PlayerObject* player);
};

class ClosestMapper : public PlayerMapper
{
protected:


    list<PlayerObject*> listUnsureAll;
    list<PlayerObject*> listUnsureTeammates;
    list<PlayerObject*> listUnsureOpponents;
public:
    ClosestMapper();
    bool   mapInGame(int unknownCount) override;

protected:
    bool   mapTeammates();
    bool   mapOpponents();
    bool   mapUnsure();

    int    findClosestForLastSee(PlayerObject* arr, int count, VecPosition pos);
    bool   mapToClosest(PlayerObject *player, PlayerObject* arr, int count);
    bool   mapToClosestFromAll(PlayerObject *player, PlayerObject* arr, int count);
};


class MinSumMapper : public PlayerMapper
{
protected:
    struct PlayerToMap
    {
        PlayerObject *player;
        VecPosition pos;
        bool unmapped;
        int unknownIndex;
    };
    struct PossiblePlayer
    {
        PlayerToMap *ptm;
        double dist;
    };
    struct Unknown
    {
        PlayerObject *player;
        vector<PossiblePlayer> possibleTargets;
        int curPossible;
        PossiblePlayer *minPossible;
        PossiblePlayer *globalMinPossible;
    };

    vector<Unknown> unknowns;
    vector<PlayerToMap> unmappedPlayers;
protected:
    vector<PlayerObject> unsureAll;
    vector<PlayerObject> unsureTeammates;
    vector<PlayerObject> unsureOpponents;
public:
    MinSumMapper();
    bool   mapInGame(int unknownCount) override;

protected:
    bool mapKnowns(int unknownCount);

    void createListUnmapped();

    bool mapUnknowns();
    double mapOne(int curUnknown);
};

#endif // PLAYERMAPPER_H
