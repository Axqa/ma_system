#include "worldmodel.h"
#include "parser.h"
#include <string.h>
#include <cstdio>
#include "soccertypes.h"
#include <sys/time.h>
#include "logger.h"

extern Logger Log;

bool WorldModel::mapInGame()
{

    VecPosition pos;
    PlayerObject *p;

    for (int i = 0; i < unknownCount; ++i)
    {
        p = &unknownPlayers[i];
        VisiblePlayer &vp = p->getLastVision();

        if (p->getUnum() != -1) // known player
        {
            vp.setUnumConf(1);
            vp.setTeamConf(1);

            if (p->getSide() == getSide()) // teammate
            {
                teammates[p->getUnum()-1].addVision(vp);
            }
            else // opponent
            {
                opponents[p->getUnum()-1].addVision(vp);
            }
        }
        else // unknown unum
        {
            if (p->getSide() != SIDE_ILLEGAL)
            {
                vp.setTeamConf(1);
                if (p->getSide() == getSide()) // teammate
                {
                    listUnsureTeammates.push_back(*p);
                }
                else // opponent
                {
                    listUnsureOpponents.push_back(*p);
                }
            }
            else
            {
                listUnsureAll.push_back(*p);
            }
        }
    }

    if (listUnsureAll.size() != 0 ||
            listUnsureOpponents.size() != 0 ||
            listUnsureTeammates.size() != 0)
    {
        return mapUnsure();
    }

    return true;
}

bool WorldModel::mapUnsure()
{
    bool res = true;
    if (listUnsureTeammates.size() != 0)
    {
        res &= mapTeammates();
    }

    if (listUnsureOpponents.size() != 0)
    {
        res &= mapOpponents();
    }

    for (auto p : listUnsureAll)
    {
        res &= mapToClosestFromAll(p, teammates, MAX_TEAMMATES+MAX_OPPONENTS);
    }
    return res;
}

bool WorldModel::mapTeammates()
{
    bool res = true;
    for (auto p : listUnsureTeammates)
    {
        res &= mapToClosest(p, teammates, MAX_TEAMMATES);
    }
    return res;
}

bool WorldModel::mapOpponents()
{
    bool res = true;
    for (auto p : listUnsureOpponents)
    {
        res &= mapToClosest(p, opponents, MAX_OPPONENTS);
    }
    return res;
}


bool   WorldModel::mapToClosest(PlayerObject player, PlayerObject* arr, int count)
{
    double sum = 0, e = 0, minE = 0, dist, minDist = 1000;
    int minIdx = -1;
    VecPosition pos;
    VisiblePlayer vp = player.getLastVision();
    VecPosition playerPos = vp.getAbsPos();
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].getLastSeeTime() == getCurrentTime()) continue;
        pos = arr[i].getLastVision().getAbsPos();

        dist = pos.getDistanceTo(playerPos);

        dist = max(dist, MIN_DIST);

        e = exp(1/dist);
        sum += e;

        if (dist < minDist)
        {
            minE = e;
            minIdx = i;
            minDist = dist;
        }
    }

    if (minIdx == -1) return false;

    vp.setUnumConf(minE / max(sum, MIN_DIST));
    arr[minIdx].addVision(vp);

    return true;
}

bool WorldModel::mapToClosestFromAll(PlayerObject player, PlayerObject *arr, int count)
{
    double sum = 0, e = 0, minE = 0, dist, minDist = 1000;
    int minIdx = -1;
    VecPosition pos;
    double t1 = 0, t2 = 0;
    VisiblePlayer vp = player.getLastVision();
    VecPosition playerPos = vp.getAbsPos();
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].getLastSeeTime() == getCurrentTime()) continue;
        pos = arr[i].getLastVision().getAbsPos();

        dist = pos.getDistanceTo(playerPos);

        dist = max(dist, MIN_DIST);

        e = exp(1/dist);
        sum += e;

        if (i < count / 2)
            t1 += e;
        else
            t2 += e;

        if (dist < minDist)
        {
            minE = e;
            minIdx = i;
            minDist = dist;
        }
    }

    if (minIdx == -1) return false;

    vp.setTeamConf(minIdx < count/2 ? t1 / (t1+t2) : t2 / (t1+t2));
    vp.setUnumConf(minE / max(sum, MIN_DIST));
    arr[minIdx].addVision(vp);

    return true;
}

int WorldModel::findClosestForLastSee(PlayerObject* arr, int count, VecPosition pos)
{
    int curClosest = -1;
    double minDist = 1000, curDist;
    VisiblePlayer vp;
    for (int i = 0; i < count; ++i)
    {
        vp = arr[i].getLastVision();
        if (vp.getTime() == getCurrentTime())
            vp = arr[i].getPrevVision();

        curDist = pos.getDistanceTo(vp.getAbsPos());
        if (curDist < minDist)
        {
            minDist = curDist;
            curClosest = i;
        }
    }

    return curClosest;
}
