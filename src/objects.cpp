#include "objects.h"
#include "soccertypes.h"

Object::Object()
{
    absBodyAngle = UnknownAngleValue;
    absNeckAngle = UnknownAngleValue;
}

VecPosition Object::getAbsPos()
{
    return absPos;
}

void Object::setAbsPos(VecPosition pos)
{
    absPos = pos;
}

AngDeg Object::getAbsBodyAngle()
{
    return absBodyAngle;
}

void Object::setAbsBodyAngle(AngDeg angle)
{
    absBodyAngle = angle;
}

AngDeg Object::getAbsNeckAngle()
{
    return absNeckAngle;
}

void Object::setAbsNeckAngle(AngDeg angle)
{
    absNeckAngle = angle;
}

VisiblePlayer::VisiblePlayer()
{
    relBodyAngle = UnknownAngleValue;
    relNeckAngle = UnknownAngleValue;

    unumConf = 0;
    teamConf = 0;

    time = -1;
}

VecPosition VisiblePlayer::getAbsPolar()
{
    return absPolar;
}

void VisiblePlayer::setAbsPolar(VecPosition pos)
{
    absPolar = pos;
}

VecPosition VisiblePlayer::getRelPolar()
{
    return relPolar;
}

void VisiblePlayer::setRelPolar(VecPosition pos)
{
    relPolar = pos;
}

double VisiblePlayer::getUnumConf()
{
    return unumConf;
}

void VisiblePlayer::setUnumConf(double conf)
{
    unumConf = conf;
}

double VisiblePlayer::getTeamConf()
{
    return teamConf;
}

void VisiblePlayer::setTeamConf(double conf)
{
    teamConf = conf;
}

int VisiblePlayer::getTime()
{
    return time;
}

void VisiblePlayer::setTime(int t)
{
    time = t;
}


History::History(int maxSize)
    : maxSize(maxSize)
{
}

int History::getSize()
{
    return history.size();
}

int History::getMaxSize()
{
    return maxSize;
}

void History::setMaxSize(int size)
{
    if (size < 0) return;
    maxSize = size;
    while (maxSize > history.size()) {
        history.pop_front();
    }
}

void History::addEntry(VisiblePlayer entry)
{
    history.push_back(entry);
    if (history.size() > maxSize)
        history.pop_front();
}

VisiblePlayer& History::getEntry(int index)
{
    return history[index];
}

VisiblePlayer& History::getLast()
{
    return getEntry(history.size()-1);
}



PlayerObject::PlayerObject(int maxHistory)
    : history(maxHistory)
{
}

History &PlayerObject::getHistory()
{
    return history;
}

int PlayerObject::getLastSeeTime()
{
    if (history.getSize() == 0) return -1;

    return history.getLast().getTime();
}

VisiblePlayer& PlayerObject::getLastVision()
{
    return history.getLast();
}

VisiblePlayer &PlayerObject::getPrevVision()
{
    return history.getEntry(history.getSize()-1 - 1);
}

void PlayerObject::addVision(VisiblePlayer vp)
{
    history.addEntry(vp);
}

VecPosition PlayerObject::getPredictedPos()
{
    return predictedPos;
}

void PlayerObject::setPredictedPos(VecPosition pos)
{
    predictedPos = pos;
}

AngDeg VisiblePlayer::getRelBodyAngle()
{
    return relBodyAngle;
}

void VisiblePlayer::setRelBodyAngle(AngDeg angle)
{
    relBodyAngle = angle;
}

AngDeg VisiblePlayer::getRelNeckAngle()
{
    return relNeckAngle;
}

void VisiblePlayer::setRelNeckAngle(AngDeg angle)
{
    relNeckAngle = angle;
}

PlayerInfo::PlayerInfo()
{
    unum = -1;
    side = SIDE_ILLEGAL;
    isGoalie = false;
}

int PlayerInfo::getUnum()
{
    return unum;
}

void PlayerInfo::setUnum(int num)
{
    unum = num;
}

SideT PlayerInfo::getSide()
{
    return side;
}

void PlayerInfo::setSide(SideT s)
{
    side = s;
}

bool PlayerInfo::getIsGoalie()
{
    return isGoalie;
}

void PlayerInfo::setIsGoalie(bool goalie)
{
    isGoalie = goalie;
}

Agent::Agent()
{
    viewAngle = VA_ILLEGAL;
}

ViewAngleT Agent::getViewAngle()
{
    return viewAngle;
}

void Agent::setViewAngle(ViewAngleT view)
{
    viewAngle = view;
}

AngDeg Agent::getNeckToBodyAngle()
{
    return neckToBody;
}

void Agent::setNeckToBodyAngle(AngDeg ang)
{
    neckToBody = ang;
}

FixedObject::FixedObject()
{
    type = OBJECT_ILLEGAL;
    time = -1;
}

VecPosition FixedObject::getGlobalPos(SideT side)
{
    return SoccerTypes::getGlobalPositionFlag(type, side);
}

ObjectT FixedObject::getType()
{
    return type;
}

void FixedObject::setType(ObjectT o)
{
    type = o;
}

VecPosition FixedObject::getRelPos()
{
    return relPos;
}

void FixedObject::setRelPos(VecPosition pos)
{
    relPos = pos;
}

int FixedObject::getTime()
{
    return time;
}

void FixedObject::setTime(int t)
{
    time = t;
}
