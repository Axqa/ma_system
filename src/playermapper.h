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

/*!<
    Basic class for mapping algorithm
*/
class PlayerMapper
{
protected:

    Formations *fm;     /*!< reference to formation info */

    Agent *agent;       /*!< reference to agent object */

    PlayerObject *teammates;    /*!< reference to list of teammates */
    PlayerObject *opponents;    /*!< reference to list of opponents */

    PlayerObject *unknownPlayers;   /*!< reference to list of unmapped players */

    // pair <player, unknownPlayer>
    vector<pair<PlayerObject*, PlayerObject*>> mapTo;
                                /*!< array of mapped players */
    SideT agentSide;    /*!< side of agent */

    int curTime;        /*!< current cycle */
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
    double confFromDist(double dist);
};

/*!
    Class ClosestMapper implement mapping to closest.
*/
class ClosestMapper : public PlayerMapper
{
protected:


    list<PlayerObject*> listUnsureAll;          /*!< list for unknown both team and unum */
    list<PlayerObject*> listUnsureTeammates;    /*!< list for unknown unum and teammate */
    list<PlayerObject*> listUnsureOpponents;    /*!< list for unknown unum and opponent */
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

/*!
     Class MinSumMapper implements mapping algorithm for findind
     minimal sum of distances from last position to seen player
*/
class MinSumMapper : public PlayerMapper
{
protected:
    /*!< Represents player in finding algorithm */
    struct PlayerToMap
    {
        PlayerObject *player;   /*!< reference to known player */
        VecPosition pos;        /*!< position of player */
        bool unmapped;          /*!< is player mapped in current step */
        int unknownIndex;       /*!< index of unknown player to map */
    };
    /*!< Represent possible player for unknown  */
    struct PossiblePlayer
    {
        PlayerToMap *ptm;       /*!< reference to player info */
        double dist;            /*!< distance for current pair */
        double sumDist;         /*!< distance for current combination */
    };
    /*!< Represent seen player in algorithm */
    struct Unknown
    {
        PlayerObject *player;   /*!< reference to unknown player */
        vector<PossiblePlayer> possibleTargets; /*!< array of possible known players */
        int curPossible;        /*!< possible index in current step */
        PossiblePlayer *minPossible;        /*!< reference to minimal posssible for step */
        PossiblePlayer *globalMinPossible;  /*!< reference to minimal possible for cycle */
        double sumLeft;         /*!< distance for pair in left team */
        double sumRight;        /*!< distance for pair in right team */
    };

    vector<Unknown> unknowns;           /*!< array of seen players */
    vector<PlayerToMap> unmappedPlayers;/*!< array of unmapped known players */

    double sumDist;     /*!< result distance */
protected:
    vector<PlayerObject> unsureAll;         /*!< array for unknown both team and unum */
    vector<PlayerObject> unsureTeammates;   /*!< array for unknown unum and teammate */
    vector<PlayerObject> unsureOpponents;   /*!< array for unknown unum and opponent */
public:
    MinSumMapper();
    bool   mapInGame(int unknownCount) override;

protected:
    bool mapKnowns(int unknownCount);

    void createListUnmapped();

    virtual void createUnknowns(int unknownCount);

    bool mapUnknowns();
    double mapOne(int curUnknown);
};

/*!<
    Class implements mapping to closest predict position.
*/
class ClosestPredictMapper : public ClosestMapper
{
public:
    ClosestPredictMapper();
protected:
    VecPosition getPos(PlayerObject& player) override;
};

/*!<
    Class implements mapping algorithm for findind
    minimal sum of distances from predicted position
    to seen player.
*/
class MinSumPredictMapper : public MinSumMapper
{
protected:
    VecPosition getPos(PlayerObject& player) override;
    void createUnknowns(int unknownCount) override;
};

#endif // PLAYERMAPPER_H
