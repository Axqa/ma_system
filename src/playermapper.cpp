#include "playermapper.h"
#include "logger.h"

extern Logger Log;

PlayerMapper::PlayerMapper()
{

}

void PlayerMapper::init(Formations *fm, Agent *agent, PlayerObject *teammates, PlayerObject *opponents, PlayerObject *unknownPlayers)
{
    this->fm = fm;
    this->agent = agent;
    this->teammates = teammates;
    this->opponents = opponents;
    this->unknownPlayers = unknownPlayers;
}

SideT PlayerMapper::getSide()
{
    return agentSide;
}

void PlayerMapper::setSide(SideT side)
{
    agentSide = side;
}

int PlayerMapper::getCurrentTime()
{
    return curTime;
}

void PlayerMapper::setCurrentTime(int time)
{
    curTime = time;
}

bool PlayerMapper::mapBeforeKickOff(int unknownCount)
{
    VecPosition pos;
    PlayerObject *p;
    int idxInFormation;

    VisiblePlayer *vp;

    // map only known players first
    for (int i = 0; i < unknownCount; ++i)
    {
        p = &unknownPlayers[i];
        VisiblePlayer &vp = p->getLastVision();

        vp.setTeamConf(1); // always can determine side of player
        if (p->getSide() == SIDE_ILLEGAL)
        {
            if (p->getLastVision().getAbsPos().getX() < 0)
            { // teammate
                p->setSide(getSide());
            }
            else
            { // opponent
                p->setSide(getSide() == SIDE_LEFT ? SIDE_RIGHT : SIDE_LEFT);
            }
        }

        if (p->getSide() == getSide()) // teammate
        {
            vp.setUnumConf(1);
            vp.setAbsPos(fm->getFormation(fm->getFormationIndex()).getNearestPosInFormation(vp.getAbsPos(), &idxInFormation));
            teammates[idxInFormation].addVision(vp);
            continue;
        }

        if (p->getUnum() == -1)
            continue;

        vp.setUnumConf(1);
        opponents[p->getUnum() - 1].addVision(vp);

    }

    // determine unknown opponents
    for (int i = 0; i < unknownCount; ++i)
    {
        p = &unknownPlayers[i];
        VisiblePlayer &vp = p->getLastVision();

        if (p->getSide() != getSide() && p->getUnum() == -1) // opponent
        {
            decideUnknownOpponentBeforeKickOff(vp);
        }

    }
    return true;
}

void PlayerMapper::decideUnknownOpponentBeforeKickOff(VisiblePlayer &vp)
{
    int firstEmpty, closest, closestUnsure;

    closest = findClosestPlayer(opponents, MAX_OPPONENTS, vp.getAbsPos());
    if (closest != -1 && opponents[closest].getLastVision().getAbsPos().getDistanceTo(vp.getAbsPos()) < EPSILON)
    {   // same player
        vp.setUnumConf(opponents[closest].getLastVision().getUnumConf());
        opponents[closest].addVision(vp);
    }
    else
    {
        firstEmpty = findFirstEmptyPlayer(opponents, MAX_OPPONENTS);

        // maybe new player, add to empty
        if (firstEmpty != -1)
        {
            opponents[firstEmpty].addVision(vp);
        }
        else // no empty spots, add to nearest
        {
            closestUnsure = findClosestUnsurePlayer(opponents, MAX_OPPONENTS, vp.getAbsPos());
            if (closestUnsure != -1)
            {
                opponents[closestUnsure].addVision(vp);

            }
            else
            {
                opponents[closest].addVision(vp);

            }
        }
    }
}

int PlayerMapper::findFirstEmptyPlayer(PlayerObject *arr, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].getLastSeeTime() == -1) return i;
    }
    return -1;
}


int PlayerMapper::findClosestPlayer(PlayerObject *arr, int count, VecPosition pos)
{
    int curClosest = -1;
    double minDist = 1000, curDist;
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].getHistory().getSize() == 0) continue;
        curDist = pos.getDistanceTo(arr[i].getLastVision().getAbsPos());
        if (curDist < minDist)
        {
            minDist = curDist;
            curClosest = i;
        }
    }

    return curClosest;
}

int PlayerMapper::findClosestUnsurePlayer(PlayerObject *arr, int count, VecPosition pos)
{
    int curClosest = -1;
    double minDist = 1000, curDist;
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].getHistory().getSize() == 0) continue;
        if (equal(arr[i].getLastVision().getUnumConf(), 1)) continue;

        curDist = pos.getDistanceTo(arr[i].getLastVision().getAbsPos());
        if (curDist < minDist)
        {
            minDist = curDist;
            curClosest = i;
        }
    }

    return curClosest;
}



ClosestMapper::ClosestMapper()
{

}


bool ClosestMapper::mapInGame(int unknownCount)
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

bool ClosestMapper::mapUnsure()
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

bool ClosestMapper::mapTeammates()
{
    bool res = true;
    for (auto p : listUnsureTeammates)
    {
        res &= mapToClosest(p, teammates, MAX_TEAMMATES);
    }
    return res;
}

bool ClosestMapper::mapOpponents()
{
    bool res = true;
    for (auto p : listUnsureOpponents)
    {
        res &= mapToClosest(p, opponents, MAX_OPPONENTS);
    }
    return res;
}


bool   ClosestMapper::mapToClosest(PlayerObject player, PlayerObject* arr, int count)
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

bool ClosestMapper::mapToClosestFromAll(PlayerObject player, PlayerObject *arr, int count)
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

int ClosestMapper::findClosestForLastSee(PlayerObject* arr, int count, VecPosition pos)
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
