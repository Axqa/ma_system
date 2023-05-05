#include "playermapper.h"
#include "logger.h"
#include "soccertypes.h"
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

vector<pair<PlayerObject *, PlayerObject *> > PlayerMapper::getMapTo()
{
    return mapTo;
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
    mapTo.clear();

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
//            teammates[idxInFormation].addVision(vp);
            addToMap(&teammates[idxInFormation], p);
            continue;
        }

        if (p->getUnum() == -1)
            continue;

        vp.setUnumConf(1);
//        opponents[p->getUnum() - 1].addVision(vp);
        addToMap(&opponents[p->getUnum() - 1], p);

    }

    // determine unknown opponents
    for (int i = 0; i < unknownCount; ++i)
    {
        p = &unknownPlayers[i];

        if (p->getSide() != getSide() && p->getUnum() == -1) // opponent
        {
            decideUnknownOpponentBeforeKickOff(p);
        }

    }
    return true;
}

bool PlayerMapper::provideMap()
{
    for (auto i : mapTo)
    {
        i.first->addVision(i.second->getLastVision());
    }
}

VecPosition PlayerMapper::getPos(PlayerObject &player)
{

    return player.getLastVision().getAbsPos();

}

void PlayerMapper::addToMap(PlayerObject *player, PlayerObject *unknown)
{
    mapTo.push_back({player, unknown});
    player->addVision(unknown->getLastVision());
}

void PlayerMapper::decideUnknownOpponentBeforeKickOff(PlayerObject *p)
{
    VisiblePlayer &vp = p->getLastVision();
    int firstEmpty, closest, closestUnmapped;

    closest = findClosestPlayer(opponents, MAX_OPPONENTS, vp.getAbsPos());
    if (closest != -1 && opponents[closest].getLastVision().getAbsPos().getDistanceTo(vp.getAbsPos()) < EPSILON)
    {
        if (isMapped(&opponents[closest]))
        {
            closestUnmapped = findClosestUnmapped(opponents, MAX_OPPONENTS, vp.getAbsPos());
//            vp.setUnumConf(opponents[closestUnmapped].getLastVision().getUnumConf());
    //        opponents[closest].addVision(vp);
            addToMap(&opponents[closestUnmapped], p);
        }
        else // same player
        {
            vp.setUnumConf(opponents[closest].getLastVision().getUnumConf());
    //        opponents[closest].addVision(vp);
            addToMap(&opponents[closest], p);
        }
    }
    else
    {
        firstEmpty = findFirstEmptyPlayer(opponents, MAX_OPPONENTS);

        // maybe new player, add to empty
        if (firstEmpty != -1)
        {
//            opponents[firstEmpty].addVision(vp);
            addToMap(&opponents[firstEmpty], p);
        }
        else // no empty spots, add to nearest
        {

            closestUnmapped = findClosestUnmapped(opponents, MAX_OPPONENTS, vp.getAbsPos());
            if (closestUnmapped != -1)
            {
//                opponents[closestUnsure].addVision(vp);
                addToMap(&opponents[closestUnmapped], p);
            }
            else
            {
//                vp.setUnumConf(opponents[closest].getLastVision().getUnumConf());
//                opponents[closest].addVision(vp);
                addToMap(&opponents[closest], p);
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

int PlayerMapper::findClosestUnmapped(PlayerObject *arr, int count, VecPosition pos)
{
    int curClosest = -1;
    double minDist = 1000, curDist;
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].getHistory().getSize() == 0) continue;

        if (isMapped(&arr[i])) continue;

        curDist = pos.getDistanceTo(arr[i].getLastVision().getAbsPos());
        if (curDist < minDist)
        {
            minDist = curDist;
            curClosest = i;
        }
    }

    return curClosest;
}

bool PlayerMapper::isMapped(PlayerObject *player)
{
    if (player->getUnum() == agent->getUnum()) return true;

    for (auto i : mapTo)
    {
        if (i.first == player)
        {
            return true;
        }
    }
    return false;
}

double PlayerMapper::confFromDist(double dist)
{
    return exp(1/max(dist, MIN_DIST));
}



ClosestMapper::ClosestMapper()
{

}


bool ClosestMapper::mapInGame(int unknownCount)
{
    mapTo.clear();

    listUnsureAll.clear();
    listUnsureOpponents.clear();
    listUnsureTeammates.clear();

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
//                teammates[p->getUnum()-1].addVision(vp);
                addToMap(&teammates[p->getUnum()-1], p);
            }
            else // opponent
            {
//                opponents[p->getUnum()-1].addVision(vp);
                addToMap(&opponents[p->getUnum()-1], p);

            }
        }
        else // unknown unum
        {
            if (p->getSide() != SIDE_ILLEGAL)
            {
                vp.setTeamConf(1);
                if (p->getSide() == getSide()) // teammate
                {
                    listUnsureTeammates.push_back(p);
                }
                else // opponent
                {
                    listUnsureOpponents.push_back(p);
                }
            }
            else
            {
                listUnsureAll.push_back(p);
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


bool   ClosestMapper::mapToClosest(PlayerObject *player, PlayerObject* arr, int count)
{
    double sum = 0, e = 0, minE = 0, dist, minDist = 1000;
    int minIdx = -1;
    VecPosition pos;
    VisiblePlayer &vp = player->getLastVision();
    VecPosition playerPos = vp.getAbsPos();
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].getLastSeeTime() == getCurrentTime()) continue;
        pos = getPos(arr[i]);

        dist = pos.getDistanceTo(playerPos);

        dist = max(dist, MIN_DIST);

        e = confFromDist(dist);
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

//    arr[minIdx].addVision(vp);
    addToMap(&arr[minIdx], player);
    return true;
}

bool ClosestMapper::mapToClosestFromAll(PlayerObject *player, PlayerObject *arr, int count)
{
    double sum = 0, e = 0, minE = 0, dist, minDist = 1000;
    int minIdx = -1;
    VecPosition pos;
    double t1 = 0, t2 = 0;
    VisiblePlayer &vp = player->getLastVision();
    VecPosition playerPos = vp.getAbsPos();
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].getLastSeeTime() == getCurrentTime()) continue;
        pos = getPos(arr[i]);

        dist = pos.getDistanceTo(playerPos);

        dist = max(dist, MIN_DIST);

        e = confFromDist(dist);
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
//    arr[minIdx].addVision(vp);
    addToMap(&arr[minIdx], player);
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

MinSumMapper::MinSumMapper()
{

}

bool MinSumMapper::mapInGame(int unknownCount)
{
    mapTo.clear();

    unknowns.clear();
    unmappedPlayers.clear();

    unsureAll.clear();
    unsureOpponents.clear();
    unsureTeammates.clear();

//    cout << "Start mapping with " << unknownCount <<  endl;
//    for (int i = 0; i < unknownCount; ++i)
//    {
//        cout << unknownPlayers[i].getUnum() << " | " << SoccerTypes::getSideStr(unknownPlayers[i].getSide()) << " | " <<  getPos(unknownPlayers[i]) << endl;
//    }

    mapKnowns(unknownCount);

    createListUnmapped();

    createUnknowns(unknownCount);

    return mapUnknowns();
}

bool MinSumMapper::mapKnowns(int unknownCount)
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
//                teammates[p->getUnum()-1].addVision(vp);
                addToMap(&teammates[p->getUnum()-1], p);
            }
            else // opponent
            {
//                opponents[p->getUnum()-1].addVision(vp);
                addToMap(&opponents[p->getUnum()-1], p);
            }
        }
        else
        {
            if (p->getSide() != SIDE_ILLEGAL)
            {
                vp.setTeamConf(1);
            }
        }
    }

    return true;
}

void MinSumMapper::createListUnmapped()
{
    unmappedPlayers.clear();

    for (int i = 0; i < MAX_TEAMMATES; ++i)
    {
        PlayerObject *p = &teammates[i];
        if (isMapped(p)) continue;

        PlayerToMap ptm;
        ptm.player = p;
        ptm.pos = getPos(*ptm.player);
        ptm.unmapped = true;
        ptm.unknownIndex = -1;

        unmappedPlayers.push_back(ptm);
    }

    for (int i = 0; i < MAX_OPPONENTS; ++i)
    {
        PlayerObject *p = &opponents[i];
        if (isMapped(p)) continue;

        PlayerToMap ptm;
        ptm.player = p;
        ptm.pos = getPos(*ptm.player);
        ptm.unmapped = true;
        ptm.unknownIndex = -1;

        unmappedPlayers.push_back(ptm);
    }
}

void MinSumMapper::createUnknowns(int unknownCount)
{
    unknowns.clear();

    VecPosition pos;
    PlayerObject *p;
    double dist;
    for (int i = 0; i < unknownCount; ++i)
    {
        p = &unknownPlayers[i];
        VisiblePlayer &vp = p->getLastVision();

        if (p->getUnum() == -1) // unknown unum
        {
            Unknown u;

            u.curPossible = 0;
            u.player = p;
            u.sumLeft = u.sumRight = 0;

            for (int j = 0; j < unmappedPlayers.size(); ++j)
            {
                // if we know side of unknown and it is not same with player's side, then skip
                if (p->getSide() != SIDE_ILLEGAL && p->getSide() != unmappedPlayers[j].player->getSide())
                    continue;

                dist = unmappedPlayers[j].pos.getDistanceTo(getPos(*p));

                // if player too far from unknown, then skip
                if (dist > PLAYER_SPEED_MAX * (getCurrentTime() - unmappedPlayers[j].player->getLastSeeTime()) * 2)
                    continue;

                u.possibleTargets.push_back(
                    {&unmappedPlayers[j], dist, 0}
                );
            }
            unknowns.push_back(u);
        }
    }
}

bool MinSumMapper::mapUnknowns()
{
    if (unknowns.size() == 0) return true;

    sumDist = 0;
    Log.log(3, "Start mapping with %d unknowns", unknowns.size());
    double minDist = mapOne(0);

    if (minDist < 0)
    {
        Log.log(3, "Unsuccessful end of mapping with min dist %f", minDist);
        return false;
    }


//    unknowns[0].globalMinPossible = unknowns[0].minPossible;
    for (auto &unknown : unknowns)
    {
        VisiblePlayer &vp = unknown.player->getLastVision();
        vp.setUnumConf(unknown.minPossible->sumDist / sumDist);

        if (vp.getTeamConf() != 1)
        {
            if (unknown.minPossible->ptm->player->getSide() == SIDE_LEFT)
                vp.setTeamConf((unknown.sumLeft) / ((unknown.sumLeft)+(unknown.sumRight)));
            else
                vp.setTeamConf((unknown.sumRight) / ((unknown.sumLeft)+(unknown.sumRight)));
        }

        addToMap(unknown.minPossible->ptm->player, unknown.player);

//        if (vp.getUnumConf() != 1)
//        {
//            cout << "Halliluya" << endl;
//        }

    }

    Log.log(3, "Successful end of mapping with min dist %f", minDist);

    return true;
}

double MinSumMapper::mapOne(int curUnknown)
{
    PossiblePlayer* minCombination[unknowns.size()];
    if (curUnknown == unknowns.size())
    {
        double conf, dist = 0;
        for (auto &unknown : unknowns)
        {
            dist += unknown.possibleTargets[unknown.curPossible].dist;

        }
        conf = confFromDist(dist);
        sumDist += conf;

        for (auto &unknown : unknowns)
        {
            unknown.possibleTargets[unknown.curPossible].sumDist += conf;


            if (unknown.possibleTargets[unknown.curPossible].ptm->player->getSide() == SIDE_LEFT)
            {
                unknown.sumLeft += conf;
            }
            else
            {
                unknown.sumRight += conf;
            }
        }
        return 0;
    }

    bool notFound = true;
    double minDist = 10000, curDist = 0;

    Log.log(3, " Start mapOne for %d (%f, %f) with %d possible targets",
            curUnknown,
            unknowns[curUnknown].player->getLastVision().getAbsPos().getX(),
            unknowns[curUnknown].player->getLastVision().getAbsPos().getY(),
            unknowns[curUnknown].possibleTargets.size());

//    for (auto &possible : unknowns[curUnknown].possibleTargets)
    for (int curP = 0; curP < unknowns[curUnknown].possibleTargets.size(); ++ curP)
    {
        auto &possible = unknowns[curUnknown].possibleTargets[curP];
        Log.log(3, "  Possible target: (%f, %f) with index %d and dist %f",
                possible.ptm->pos.getX(),
                possible.ptm->pos.getY(),
                possible.ptm->unknownIndex,
                possible.dist);

        // if target not occupied
        if (possible.ptm->unknownIndex == -1)
        {
            // set this player as current unknown
            possible.ptm->unknownIndex = curUnknown;

            unknowns[curUnknown].curPossible = curP;

            // calc min dist for rest of unknowns
            curDist = mapOne(curUnknown+1);

            // if dist not found, skip this combination
            if (curDist < -0.5)
            {
                possible.ptm->unknownIndex = -1;
                continue;
            }

            // if dist exists, add dist for current unknown and set flag to false
            curDist += possible.dist;

            // found at least one combination
            notFound = false;

            // if dist is minimal for this unknown, remember combination
            if (curDist < minDist)
            {
                Log.log(3, "   Min dist updated");

                minDist = curDist;

                unknowns[curUnknown].minPossible = &possible;

//                unknowns[curUnknown+1].globalMinPossible = unknowns[curUnknown+1].minPossible;
                for (int unk = curUnknown; unk < unknowns.size(); ++unk)
                {
                    minCombination[unk] = unknowns[unk].minPossible;
//                    unknowns[unk].globalMinPossible = unknowns[unk].minPossible;
                }
            }

            // free this possible player for next iteration
            possible.ptm->unknownIndex = -1;
        }
    }
    Log.log(3, " End of mapOne with notFound=%d, minDist=%f", notFound, minDist);

    // if dist can't be found, return -1
    if (notFound) return -1;

    // set min combination
    for (int unk = curUnknown; unk < unknowns.size(); ++unk)
    {
        unknowns[unk].minPossible = minCombination[unk];
    }

    // else return minimal distance
    return minDist;

}


ClosestPredictMapper::ClosestPredictMapper()
{

}

VecPosition ClosestPredictMapper::getPos(PlayerObject &player)
{
    VecPosition pos = player.getPredictedPos();
    if (vecInField(pos)) return pos;

    return PlayerMapper::getPos(player);
}

VecPosition MinSumPredictMapper::getPos(PlayerObject &player)
{
    VecPosition pos = player.getPredictedPos();
    if (vecInField(pos)) return pos;

    return PlayerMapper::getPos(player);
}

void MinSumPredictMapper::createUnknowns(int unknownCount)
{
    unknowns.clear();

    VecPosition pos;
    PlayerObject *p;
    double dist;
    for (int i = 0; i < unknownCount; ++i)
    {
        p = &unknownPlayers[i];
        VisiblePlayer &vp = p->getLastVision();

        if (p->getUnum() == -1) // unknown unum
        {
            Unknown u;

            u.curPossible = 0;
            u.player = p;
            u.sumLeft = u.sumRight = 0;

            for (int j = 0; j < unmappedPlayers.size(); ++j)
            {
                // if we know side of unknown and it is not same with player's side, then skip
                if (p->getSide() != SIDE_ILLEGAL && p->getSide() != unmappedPlayers[j].player->getSide())
                    continue;

//                dist = unmappedPlayers[j].pos.getDistanceTo(getPos(*p));

                dist = vp.getAbsPos().getDistanceTo(getPos(*unmappedPlayers[j].player));

                // if player too far from unknown, then skip
                if (unmappedPlayers[j].player->couldGetToInTime(vp.getAbsPos(), getCurrentTime()) == false)
                    continue;

                u.possibleTargets.push_back(
                    {&unmappedPlayers[j], dist}
                );
            }
            unknowns.push_back(u);
        }
    }
}
