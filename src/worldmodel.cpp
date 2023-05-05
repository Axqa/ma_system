#include "worldmodel.h"
#include "parser.h"
#include <string.h>
#include <cstdio>
#include "soccertypes.h"
#include <sys/time.h>
#include "logger.h"
#include <numeric>
#include <cmath>
#include <chrono>
extern Logger Log;
extern PredT PREDICT_TYPE;

WorldModel::WorldModel(Formations *fm, PlayerMapper* pm, Strategy *st)
    : fm(fm), pm(pm), st(st)
{
    pthread_mutex_init( &mutex_newInfo, NULL );
    pthread_cond_init ( &cond_newInfo,  NULL );

    lastSeeTime = -1;
    newInfo = false;
    side = SIDE_ILLEGAL;
    teamName = "";
    playMode = PM_BEFORE_KICK_OFF;


    for (int i = 0; i < MAX_FLAGS; ++i)
    {
        flags[i].setType(SoccerTypes::getFlagFromIndex(i));
        flags[i].setTime(-1);
    }
    for (int i = 0; i < MAX_LINES; ++i)
    {
        lines[i].setType(SoccerTypes::getLineFromIndex(i));
        lines[i].setTime(-1);
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

    pm->init(fm,
             &agent,
             teammates,
             opponents,
             unknownPlayers);
}

void WorldModel::show(ostream &os)
{
    PlayerObject *p;
    VisiblePlayer *vp;
    int size;
    int invert = getSide() == SIDE_LEFT ? 1 : -1;
    os << "World model:" << '\n';
    os << "Time: " << getCurrentTime() << '\n';
    os << "Play mode: " << SoccerTypes::getPlayModeStr(getPlayMode()) << "\n";
    os << "Unknowns " << unknownCount << ":\n";
    for (int i = 0; i < unknownCount; ++i)
    {
        p = &unknownPlayers[i];
        vp = &(p->getHistory().getLast());
        os << "Unknown " << i << " | ";
        os << " Side: " << SoccerTypes::getSideStr( p->getSide()) << " | ";
        os << " Num: " << p->getUnum() << " | ";
        os << " Last Pos: " << vp->getAbsPos() * invert << '\n';

    }
    os << "\nTeammates: " << "\n";
    for (int i = 0; i < MAX_TEAMMATES; ++i)
    {
        p = &teammates[i];
        os << "Teammate ";
        os << " Side: " << SoccerTypes::getSideStr( p->getSide()) << " | ";
        os << " Num: " << p->getUnum() << '\n';

        os << " | Predicted: " << p->getPredictedPos() * invert << "\n";

        size = p->getHistory().getSize();
        for (int j = 0; j < size; ++j)
        {
            vp = &(p->getHistory().getEntry(size - j - 1));
            os << " | Time: " << vp->getTime() << " | ";
            os << "Pos: " << vp->getAbsPos() * invert << " | ";
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

        os << " | Predicted: " << p->getPredictedPos() * invert << "\n";

        size = p->getHistory().getSize();
        for (int j = 0; j < size; ++j)
        {
            vp = &(p->getHistory().getEntry(size - j - 1));
            os << " | Time: " << vp->getTime() << " | ";
            os << "Pos: " << vp->getAbsPos() * invert << " | ";
            os << "Team conf: " << vp->getTeamConf() << " | ";
            os << "Unum conf: " << vp->getUnumConf() << "\n";
        }
    }
    PlayerObject *player, *unknown;
    os << "Mapping: \n";
    for (auto i : pm->getMapTo())
    {
        player = i.first;
        unknown = i.second;

        os << "[" << unknown->getUnum() << " | " << SoccerTypes::getSideStr(unknown->getSide()) << " | " <<
                unknown->getLastVision().getAbsPos() * invert << "] as [" <<
                player->getUnum() << " (" << unknown->getLastVision().getUnumConf() << ")" <<
              " | " << SoccerTypes::getSideStr(player->getSide()) <<
              " (" << unknown->getLastVision().getTeamConf() << ")]" << "\n";
    }
}

void WorldModel::showAgent(ostream &os)
{
    os << "Agent:" <<
          "\nSide: " << agent.getSide() <<
          "\nUnum: " << agent.getUnum() <<
          "\nAbs pos: " << agent.getAbsPos() <<
          "\nAbs Neck angle: " << agent.getAbsNeckAngle() <<
          "\nAbs Body angle: " << agent.getAbsBodyAngle() <<
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

void WorldModel::setCurrentTime(int time)
{
    lastSeeTime = time;

    pm->setCurrentTime(time);
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

FixedObject *WorldModel::getLines()
{
    return lines;
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

    pm->setSide(s);
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

    setCurrentTime(time);

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

void WorldModel::updateFormationToSide()
{

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
        {

            updatePredictions();
            auto start = std::chrono::high_resolution_clock::now();
            if (mapUnknownPlayers() == false)
            {
               Log.log(3, "Can't map players!!!");
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

//            std::cout << "Time: " << duration.count() << " mcs" << std::endl;

            Log.getOutputStream() << "Time for mapping: " << duration.count() << " mcs" << endl;

            show(Log.getOutputStream());
        }
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
    int time = getCurrentTime();

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

    agent.setAbsBodyAngle(globalNeckAngle + agent.getNeckToBodyAngle());
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
    if (getPlayMode() == PM_BEFORE_KICK_OFF)
    {
        return pm->mapBeforeKickOff(unknownCount);
    }

    return pm->mapInGame(unknownCount);
}

// returns pair <slope, intercept>
pair<double, double> linear_regression(vector<double>& x, vector<double>& y) {
    int n = x.size();
    double sum_x = accumulate(x.begin(), x.end(), 0.0);
    double sum_y = accumulate(y.begin(), y.end(), 0.0);
    double mean_x = sum_x / n;
    double mean_y = sum_y / n;

    double cov = 0.0;  // covariance
    double var_x = 0.0;  // variance x
    for (int i = 0; i < n; i++) {
        cov += (x[i] - mean_x) * (y[i] - mean_y);
        var_x += pow(x[i] - mean_x, 2);
    }

    double slope = cov / var_x;
    if (isnan(slope)) slope = 0;
    double intercept = mean_y - slope * mean_x;

    return {slope, intercept};
}

bool WorldModel::updatePredictions()
{
    vector<double> xs, ys, ts;
    VisiblePlayer vp;
    for (int i = 0; i < MAX_TEAMMATES; ++i)
    {
        if (agent.getUnum() == i-1)
        {
            teammates[i].setPredictedPos(agent.getAbsPos());
            continue;
        }
        if (PREDICT_TYPE == PRED_LS_WITH_TEAM)
            predictTeammateWithStrategy(&teammates[i]);
        else
            calcPrediction(&teammates[i]);

    }
    for (int i = 0; i < MAX_OPPONENTS; ++i)
    {
        calcPrediction(&opponents[i]);
    }
    return true;
}

void WorldModel::calcPrediction(PlayerObject *player)
{
    vector<double> xs, ys, ts;
    VisiblePlayer vp;

    xs.clear();
    ys.clear();
    ts.clear();
    History h = player->getHistory();
    for (int j = 0; j < h.getSize(); ++j)
    {
        vp = h.getEntry(j);
        xs.push_back(vp.getAbsPos().getX());
        ys.push_back(vp.getAbsPos().getY());
        ts.push_back(vp.getTime());
    }
    auto xCoef = linear_regression(ts,xs);
    auto yCoef = linear_regression(ts,ys);

    player->setPredictedPos(VecPosition(
                                     xCoef.first * getCurrentTime() + xCoef.second,
                                yCoef.first * getCurrentTime() + yCoef.second));
}

void WorldModel::predictTeammateWithStrategy(PlayerObject *player)
{
    VecPosition target = st->getStrategyTargetForTime(player->getUnum(), getCurrentTime());
//    VecPosition prevTarget = (getCurrentTime() < st->getTimeForTarget()) ?
//                fm->getPlayerPos(player->getUnum()) :
//                st->getStrategyTargetForTime(player->getUnum(), getCurrentTime() - st->getTimeForTarget());


    VecPosition lastPos = player->getLastVision().getAbsPos();
    int lastTime = player->getLastSeeTime();

    VecPosition pred = lastPos + (target - lastPos).normalize() * (getCurrentTime() - lastTime) * PLAYER_SPEED_MAX * 0.9;

    if (vecInField(pred) == false)
    {
        calcPrediction(player);
        return;
    }

    player->setPredictedPos(pred);
}
