#include "worldmodel.h"
#include "parser.h"
#include <string.h>
#include <cstdio>
#include "soccertypes.h"
#include <sys/time.h>
#include "logger.h"

extern Logger Log;

WorldModel::WorldModel(Formations *fm)
    : fm(fm)
{
    pthread_mutex_init( &mutex_newInfo, NULL );
    pthread_cond_init ( &cond_newInfo,  NULL );

    lastSeeTime = -1;
    newInfo = false;
    side = SIDE_ILLEGAL;
    teamName = "";
    playMode = PM_BEFORE_KICK_OFF;

    unsureAll.reserve(MAX_TEAMMATES + MAX_OPPONENTS);
    unsureOpponents.reserve(MAX_OPPONENTS);
    unsureTeammates.reserve(MAX_TEAMMATES);

    for (int i = 0; i < MAX_FLAGS; ++i)
    {
        flags[i].setType(SoccerTypes::getFlagFromIndex(i));
    }
    for (int i = 0; i < MAX_LINES; ++i)
    {
        lines[i].setType(SoccerTypes::getLineFromIndex(i));

    }

    for (int i = 0; i < MAX_TEAMMATES; ++i)
    {
        VisiblePlayer vp;
        vp.setAbsPos(fm->getFormation(fm->getFormationIndex()).getPos(i));
        vp.setTime(-1);
        vp.setUnumConf(1);
        vp.setTeamConf(1);

        teammates[i].addVision(vp);
        teammates[i].setUnum(i+1);
    }

    teammates[0].setIsGoalie(true);

    for (int i = 0; i < MAX_OPPONENTS; ++i)
    {
        opponents[i].setUnum(i+1);
    }
}

void WorldModel::show(ostream &os)
{
    PlayerObject *p;
    VisiblePlayer *vp;
    int size;
    os << "World model:" << '\n';
    os << "Time: " << getCurrentTime() << '\n';
    os << "Unknowns " << unknownCount << ":\n";
    for (int i = 0; i < unknownCount; ++i)
    {
        p = &unknownPlayers[i];
        vp = &(p->getHistory().getLast());
        os << "Unknown " << i << " | ";
        os << " Side: " << SoccerTypes::getSideStr( p->getSide()) << " | ";
        os << " Num: " << p->getUnum() << '\n';
        os << " Last Pos: " << vp->getAbsPos() << '\n';

    }
    os << "\nTeammates: " << "\n";
    for (int i = 0; i < MAX_TEAMMATES; ++i)
    {
        p = &teammates[i];
        os << "Teammate ";
        os << " Side: " << SoccerTypes::getSideStr( p->getSide()) << " | ";
        os << " Num: " << p->getUnum() << '\n';

        size = p->getHistory().getSize();
        for (int j = 0; j < size; ++j)
        {
            vp = &(p->getHistory().getEntry(size - j - 1));
            os << " | Time: " << vp->getTime() << " | ";
            os << "Pos: " << vp->getAbsPos() << " | ";
            os << "Team conf: " << vp->getTeamConf() << " | ";
            os << "Unum conf: " << vp->getUnumConf() << "\n";
        }
    }
    os << "\nOpponents: " << "\n";
    for (int i = 0; i < MAX_OPPONENTS; ++i)
    {
        p = &opponents[i];
        os << "Opponent ";
        os << " Side: " << SoccerTypes::getSideStr( p->getSide()) << " | ";
        os << " Num: " << p->getUnum() << '\n';

        size = p->getHistory().getSize();
        for (int j = 0; j < size; ++j)
        {
            vp = &(p->getHistory().getEntry(size - j - 1));
            os << " | Time: " << vp->getTime() << " | ";
            os << "Pos: " << vp->getAbsPos() << " | ";
            os << "Team conf: " << vp->getTeamConf() << " | ";
            os << "Unum conf: " << vp->getUnumConf() << "\n";
        }
    }
}

void WorldModel::showAgent(ostream &os)
{
    os << "Agent:" <<
          "\nSide: " << agent.getSide() <<
          "\nUnum: " << agent.getUnum() <<
          "\nAbs pos: " << agent.getAbsPos() <<
          "\nAbs Neck angle: " << agent.getAbsNeckAngle() <<
          "\nNeck to body angle: " << agent.getNeckToBodyAngle() << endl;
}

string WorldModel::getExtraInfo()
{
    return extraInfo.str();
}

int WorldModel::getCurrentTime()
{
    return lastSeeTime;
}

Agent &WorldModel::getAgent()
{
    return agent;
}

bool WorldModel::waitNewInfo()
{
    bool bReturn = true;

    if( newInfo == false ) // there hasn't arrived any information yet
    {
        struct timeval now;
        struct timespec timeout;
        gettimeofday(&now, NULL);
        timeout.tv_sec = now.tv_sec + SERVER_TIME_OUT;
        timeout.tv_nsec = now.tv_usec*1000;

        // lock mutex and wait till it is unlocked by handling
        // sense or see message
        pthread_mutex_lock( &mutex_newInfo );
        int ret;
        while( (ret = pthread_cond_timedwait( &cond_newInfo,
                        &mutex_newInfo, &timeout) ) == EINTR )
         printf("(WorldModel::waitNewInfo) failure in loop!!\n");
        if( ret == ETIMEDOUT ) // if no info was received but timer timed out
          bReturn = false;
        pthread_mutex_unlock( &mutex_newInfo );
    }

    newInfo = false;

    return bReturn;
}

void WorldModel::setAgentNum(int num)
{
    agentNum = num;
    agent.setUnum(num);
}

int WorldModel::getAgentNum()
{
    return agentNum;
}

SideT WorldModel::getSide()
{
    return side;
}

void WorldModel::setSide(SideT s)
{
    side = s;
    agent.setSide(s);

    for (int i = 0; i < MAX_TEAMMATES; ++i)
    {
        teammates[i].setSide(s);
    }

    for (int i = 0; i < MAX_OPPONENTS; ++i)
    {
        opponents[i].setSide(s == SIDE_LEFT ? SIDE_RIGHT : SIDE_LEFT);
    }
}

string WorldModel::getTeamName()
{
    return teamName;
}

void WorldModel::setTeamName(string name)
{
    teamName = name;
}

PlayModeT WorldModel::getPlayMode()
{
    return playMode;
}

void WorldModel::setPlayMode(PlayModeT pm)
{
    playMode = pm;
}

bool WorldModel::getNewInfo()
{
    return newInfo;
}

void WorldModel::setNewInfo(bool bInfo)
{
    newInfo = bInfo;
}

void WorldModel::setSeeMessage(char *msg, int time)
{
//    lastSeeMsg = string(msg);
    strcpy(lastSeeMsg, msg);
//    if (time != lastSeeTime)

    lastSeeTime = time;

    newSeeInfo = true;

    pthread_mutex_lock  ( &mutex_newInfo );
    newInfo            = true;
    pthread_cond_signal ( &cond_newInfo );
    pthread_mutex_unlock( &mutex_newInfo );
}

char *WorldModel::getSeeMessage()
{
    return lastSeeMsg;
}

void WorldModel::setSenseMessage(char *msg, int time)
{
    strcpy(lastSenseMsg, msg);
    lastSenseTime = time;

    newSenseInfo = true;

    pthread_mutex_lock  ( &mutex_newInfo );
    newInfo            = true;
    pthread_cond_signal ( &cond_newInfo );
    pthread_mutex_unlock( &mutex_newInfo );
}

char *WorldModel::getSenseMessage()
{
    return lastSenseMsg;
}

bool WorldModel::clearInfo()
{
    unknownCount = 0;

    if (getCurrentTime() == 0)
    {
        for (int i = 0; i < MAX_LINES; ++i)
        {
            lines[i].setTime(-1);
        }
        for (int i = 0; i < MAX_FLAGS; ++i)
        {
            flags[i].setTime(-1);
        }
    }
    unsureAll.clear();
    unsureOpponents.clear();
    unsureTeammates.clear();

    listUnsureAll.clear();
    listUnsureOpponents.clear();
    listUnsureTeammates.clear();

    viewedFlags.clear();
    return true;
}

bool WorldModel::update(bool needMap)
{
    clearInfo();

    if (newSenseInfo)
    {
        newSenseInfo = false;
        processLastSenseMsg();
    }

    if (newSeeInfo)
    {
        newSeeInfo = false;
        processLastSeeMsg();
        calculateAllPos(needMap);

        if (needMap)
            mapUnknownPlayers();
    }
    else
    {
        return false;
    }

    return true;
}

bool WorldModel::processLastSeeMsg()
{
    Log.log(2, "Start WorldModel::processLastSeeMsg");

    char *str = lastSeeMsg;
    double  dDist, dDistChange,    dDirChange,    dPointDir,    dTemp;
    int     iDir;
    AngDeg  angBodyFacingDir, angHeadFacingDir;
    bool    isGoalie, isTackling, isKicking;
    ObjectT o;
    int time = lastSeeTime;

    Parse::parseFirstInt(&str); // skip time

    while(*str != ')')
    {
        dDist = dDistChange = dDirChange = dTemp = dPointDir = UnknownDoubleValue;
        angBodyFacingDir = angHeadFacingDir =         UnknownAngleValue;

        str += 2;          // get the object name
        isTackling = false;
        isKicking = false;

        o = SoccerTypes::getObjectFromStr( &str, &isGoalie, getTeamName().data() );

        dTemp = Parse::parseFirstDouble( &str );   // parse first value
        if ( *str == ')' )                         // if it was the only value
          iDir = (int)dTemp;                          // it was the direction
        else
        {
          dDist = dTemp;                              // else it was distance
          iDir = Parse::parseFirstInt( &str );     // and have to get direction

          double dValues[7];
          int    i = 0;
          if( *str == ' ' )                        // stil not finished
          {                                           // get dist and dir change
            while( *str != ')' && *(str+1) != 't' && *(str+1) != 'k' && i < 7 )
            {
              dValues[i]  = Parse::parseFirstDouble( &str );
              i++;
            }
          }

          switch( i )
          {
            case 0:
              break;
            case 1:
              dPointDir   = dValues[0];
              break;
            case 5:
              dPointDir   = dValues[4]; // fall through
            case 4:
              angBodyFacingDir = dValues[2];
              angHeadFacingDir = dValues[3]; // fall through
            case 2:                        // in case of ball
              dDistChange = dValues[0];
              dDirChange  = dValues[1];
              break;
            default:
              cerr << "(AnalyzeSee) wrong param nr " << i <<endl;

          }

          if( *(str+1) == 't')
            isTackling = true;
          if( *(str+1) == 'k')
            isKicking = true;
        }

        // go to end object information
        // skip ending bracket of object information.
        Parse::gotoFirstOccurenceOf( ')', &str );
        str++;

        // process the parsed information (unread values are Unknown...)
        if (SoccerTypes::isFlag(o))
        {
            processFlagInfo(o, time, dDist, iDir);
        }

        if (SoccerTypes::isPlayer(o))
        {
            processPlayerInfo(o, time, dDist, iDir, dDistChange,
                              dDirChange, angBodyFacingDir, angHeadFacingDir,
                              isGoalie);
        }

        if (SoccerTypes::isLine(o))
        {
            processLineInfo(o, time, dDist, iDir);
        }

    }

    Log.log(2, "End WorldModel::processLastSeeMsg");

    return true;

}

bool WorldModel::processLastSenseMsg()
{
    Log.log(2, "Start WorldModel::processLastSenseMsg");
    char   *strMsg = lastSenseMsg ;

    int cycle = Parse::parseFirstInt( &strMsg );// get time
    strMsg++;                                      // go to ( before view_mode

    Parse::gotoFirstOccurenceOf( ' ', &strMsg );   // skip view_mode
    strMsg++;                                      // skip space

    ViewQualityT vq = SoccerTypes::getViewQualityFromStr( strMsg );// get quality
    Parse::gotoFirstOccurenceOf( ' ', &strMsg );
    strMsg++;                                      // skip space; get view_angle
    ViewAngleT va = SoccerTypes::getViewAngleFromStr( strMsg );
    double dStamina = Parse::parseFirstDouble( &strMsg );  // get stamina
    double dEffort  = Parse::parseFirstDouble( &strMsg );  // get effort
    double dCapacity  = Parse::parseFirstDouble( &strMsg );  // get capacity

    double dSpeed   = Parse::parseFirstDouble( &strMsg );  // get speed
    AngDeg angSpeed = Parse::parseFirstDouble( &strMsg );  // get speed ang

    // minus sign since we store angle between neck and body and not vice versa
    int iHeadAngle = - Parse::parseFirstInt( &strMsg );    // get head_angle

    // set all number of performed commands
    int CMD_KICK       = Parse::parseFirstInt( &strMsg );
    int CMD_DASH       = Parse::parseFirstInt( &strMsg );
    int CMD_TURN       = Parse::parseFirstInt( &strMsg );
    int CMD_SAY        = Parse::parseFirstInt( &strMsg );
    int CMD_TURNNECK   = Parse::parseFirstInt( &strMsg );
    int CMD_CATCH      = Parse::parseFirstInt( &strMsg );
    int CMD_MOVE       = Parse::parseFirstInt( &strMsg );
    int CMD_CHANGEVIEW = Parse::parseFirstInt( &strMsg );

    int    iArmMovable = Parse::parseFirstInt( &strMsg );         // arm movable
    int    iArmExpires = Parse::parseFirstInt( &strMsg );         // arm expires
    double dArmDist    = Parse::parseFirstDouble( &strMsg );      // arm dist
    AngDeg angArm      = Parse::parseFirstDouble( &strMsg );       // arm dir

    int CMD_POINTTO    = Parse::parseFirstInt( &strMsg ); // count

    // focus target can be none (no integer) so check this
    int i = Parse::parseFirstInt( &strMsg );
    char c = (i >= 10 ) ? *(strMsg-4) : *(strMsg-3);
    if( c == 'l' || c == 'r' )    // target l or r
    {
      i = Parse::parseFirstInt( &strMsg );
    }
    int CMD_ATTENTIONTO = i;

    int iTackleExpires = Parse::parseFirstInt( &strMsg );   // tackle expires
    int CMD_TACKLE = Parse::parseFirstInt( &strMsg );

      //----- collision
    Parse::gotoFirstOccurenceOf( '(', &strMsg );   // skip view_mode
    strMsg++;                                      // skip space
    int endStr = Parse::gotoFirstOccurenceOf( '(', &strMsg );
    int cycleLastCollisionPlayer = 0;
    int cycleLastCollisionBall = 0;
      if (endStr != -1)					//there was collision
      {
          strMsg++;                    // skip space
          if (*strMsg == 'p')			//collision player
          {
              cycleLastCollisionPlayer = cycle -1;
          }
          else if(*strMsg == 'b')		//collision ball
          {
              cycleLastCollisionBall = cycle -1;
          }
      }


    angArm = VecPosition::normalizeAngle( angArm );

    agent.setViewAngle(va);
    agent.setNeckToBodyAngle(iHeadAngle);

    Log.log(2, "End WorldModel::processLastSenseMsg");

    return true;
}

bool WorldModel::processFlagInfo(ObjectT flag, int time, double dist, int ang)
{
    flags[SoccerTypes::getIndex(flag)].setRelPos(VecPosition(dist, (double)ang));
    flags[SoccerTypes::getIndex(flag)].setTime(time);

    viewedFlags.push_back(flags[SoccerTypes::getIndex(flag)]);

    return true;
}

bool WorldModel::processPlayerInfo(ObjectT player, int time, double dDist, int iDir, double dDistChange, double dDirChange, AngDeg angRelBodyAng, AngDeg angRelNeckAng, bool isGoalie)
{
    VisiblePlayer cur;
    cur.setTime(time);
    cur.setRelPolar(VecPosition(dDist, (double)iDir));
    cur.setRelBodyAngle(angRelBodyAng);
    cur.setRelNeckAngle(angRelNeckAng);

    unknownPlayers[unknownCount].getHistory().addEntry(cur);

    unknownPlayers[unknownCount].setSide(SoccerTypes::getSideFromPlayer(player, getSide()));
    unknownPlayers[unknownCount].setUnum(SoccerTypes::getNumFromPlayer(player));

    unknownCount++;
    return true;
}

bool WorldModel::processLineInfo(ObjectT line, int time, double dist, int ang)
{
    // angle returned is angle of neck angle with line, convert to angle
    // of neck with orthogonal to line
    ang = ( ang < 0 ) ? (90 + ang ) : - (90 - ang );

    lines[SoccerTypes::getIndex(line)].setRelPos(VecPosition(dist, (double)ang));
    lines[SoccerTypes::getIndex(line)].setTime(time);

    return true;
}

bool WorldModel::calculateAllPos(bool needUpdateOther)
{
    Log.log(2, "Start WorldModel::calculateAllPos");
    calculateAgentPos();

    if (needUpdateOther)
        updateAllUnknown();

    Log.log(2, "End WorldModel::calculateAllPos");
}

bool WorldModel::calculateAgentPos()
{
    Log.log(2, "Start WorldModel::calculateAgentPos");
    FixedObject *flag, *curFlag;
    double minDist = 1000, curDist;
    FixedObject *line = lines;
    int maxTime = -1;
    for (int i = 0; i < MAX_LINES; ++i)
    {
        if (lines[i].getTime() > maxTime)
        {
            maxTime = lines[i].getTime();
            line = &lines[i];
        }
    }

//    for(auto curFlag : viewedFlags)
    for (int i = 0; i < MAX_FLAGS; ++i)
    {
        curFlag = &flags[i];
        if (curFlag->getTime() == getCurrentTime())
        {
            curDist = curFlag->getRelPos().getX();
            if (curDist < minDist)
            {
                minDist = curDist;
                flag = curFlag;
            }
        }
    }

    double globalAngleLine = SoccerTypes::getGlobalAngleLine(line->getType(), getSide());
    double relAngleLine = VecPosition::normalizeAngle( line->getRelPos().getY() );

    double globalNeckAngle = VecPosition::normalizeAngle(globalAngleLine - relAngleLine);

    agent.setAbsNeckAngle(globalNeckAngle);

    double absPolarFlagAngle = VecPosition::normalizeAngle(globalNeckAngle + flag->getRelPos().getY());
    VecPosition agentPos = flag->getGlobalPos(getSide()) - VecPosition::getVecPositionFromPolar(flag->getRelPos().getX(), absPolarFlagAngle);

    agent.setAbsPos(agentPos);

//    extraInfo.str(std::string());
//    extraInfo.clear();

//    char lName[10], fName[10] ;
//    SoccerTypes::getObjectStr(lName, line->getType(), getTeamName().data());
//    SoccerTypes::getObjectStr(fName, flag->getType(), getTeamName().data());
//    extraInfo <<
//            "line: " << lName << " " << line->getType() << "\n | relAngle: " << relAngleLine << '\n' <<
//            "flag: " << fName << " " << flag->getType() << "\n | pos: " << flag->getGlobalPos(getSide()) << "\n | relPos: " << flag->getRelPos() <<
//            "\n | absAngle: " << absPolarFlagAngle <<
//            "\n | relPos: " << VecPosition::getVecPositionFromPolar(flag->getRelPos().getX(), absPolarFlagAngle) << '\n' << endl;

    Log.log(2, "End WorldModel::calculateAgentPos");

    return true;
}

void WorldModel::updateAllUnknown()
{
    Log.log(2, "Start WorldModel::updateAllUnknown");

    extraInfo.str(std::string());
    extraInfo.clear();

    VecPosition pos;
    for (int i = 0; i < unknownCount; ++i)
    {
        VisiblePlayer &p = unknownPlayers[i].getHistory().getLast();

        pos = p.getRelPolar();
        pos.setY(pos.getY() + agent.getAbsNeckAngle());

        p.setAbsPolar(pos);

        pos = agent.getAbsPos() + VecPosition::getVecPositionFromPolar(pos.getX(), pos.getY());
        p.setAbsPos(pos);

        extraInfo << "\nunknown " << i << "\n";
        extraInfo << " | relPolar: " << p.getRelPolar() << "\n";
        extraInfo << " | absPolar: " << p.getAbsPolar() << "\n";
        extraInfo << " | absPos:   " << p.getAbsPos() << "\n";
    }
    Log.log(2, "End WorldModel::updateAllUnknown");

}

bool WorldModel::mapUnknownPlayers()
{

    VecPosition pos;
    PlayerObject *p;

    if (getPlayMode() == PM_BEFORE_KICK_OFF)
    {
        return mapBeforeKickOff();
    }

    return mapInGame();
}

bool WorldModel::mapBeforeKickOff()
{
    Log.log(2, "Start WorldModel::mapBeforeKickOff");

    VecPosition pos;
    PlayerObject *p;
    int idxInFormation;

    VisiblePlayer *vp;

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
            vp.setAbsPos(fm->getFormation(fm->getFormationIndex()).getNearestPosInFormation(vp.getAbsPos(), &idxInFormation));
            teammates[idxInFormation].addVision(vp);
        }
        else // opponent
        {
            if (p->getUnum() != -1) // known player
            {
                vp.setUnumConf(1);
                if (opponents[p->getUnum() - 1].getLastSeeTime() != -1 && opponents[p->getUnum() - 1].getLastVision().getUnumConf() == 0)
                { // if we previously set unknown player as this, then set prev vision as unknown again
                    VisiblePlayer &oldVp = opponents[p->getUnum() - 1].getLastVision();

                    opponents[p->getUnum() - 1].addVision(vp);

                    decideUnknownOpponentBeforeKickOff(oldVp);
                }
                else
                {
                    opponents[p->getUnum() - 1].addVision(vp);
                }
            }
            else // opponent's num undefined
            {
                decideUnknownOpponentBeforeKickOff(vp);
            }
        }

    }
    Log.log(2, "End WorldModel::mapBeforeKickOff");

    return true;
}

void WorldModel::decideUnknownOpponentBeforeKickOff(VisiblePlayer &vp)
{
    int firstEmpty, closest;

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
        else // no empty spots, add to nearest if it not same
        {
            opponents[closest].addVision(vp);
        }
    }
}

int WorldModel::findFirstEmptyPlayer(PlayerObject *arr, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].getLastSeeTime() == -1) return i;
    }
    return -1;
}


int WorldModel::findClosestPlayer(PlayerObject *arr, int count, VecPosition pos)
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
