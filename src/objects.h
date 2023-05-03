#ifndef OBJECTS_H
#define OBJECTS_H

#include "utilities.h"
#include <deque>

using namespace std;

class Object {
protected:
    VecPosition absPos;     /*!< Absolute cartesian coordinates */
    AngDeg      absBodyAngle; /*!< Global body angle */
    AngDeg      absNeckAngle; /*!< Global neck angle */
public:
    Object();

    VecPosition getAbsPos();
    void        setAbsPos(VecPosition pos);

    AngDeg      getAbsBodyAngle();
    void        setAbsBodyAngle(AngDeg angle);
    AngDeg      getAbsNeckAngle();
    void        setAbsNeckAngle(AngDeg angle);
};

class VisiblePlayer : public Object {
protected:
    VecPosition absPolar;   /*!< Absolute polar coordinates */
    VecPosition relPolar;   /*!< Relative polar coordinates */

    AngDeg      relBodyAngle; /*!< Relative body angle to agent */
    AngDeg      relNeckAngle; /*!< Relative neck angle to agent */

    double unumConf;        /*!< Confidence in player num */
    double teamConf;        /*!< Confidence in player team */

    int    time;            /*!< Time cycle of this information */


public:
    VisiblePlayer();

    VecPosition getAbsPolar();
    void        setAbsPolar(VecPosition pos);
    VecPosition getRelPolar();
    void        setRelPolar(VecPosition pos);

    double      getUnumConf();
    void        setUnumConf(double conf);
    double      getTeamConf();
    void        setTeamConf(double conf);

    AngDeg      getRelBodyAngle();
    void        setRelBodyAngle(AngDeg angle);
    AngDeg      getRelNeckAngle();
    void        setRelNeckAngle(AngDeg angle);

    int         getTime();
    void        setTime(int t);
};


class History {
    deque<VisiblePlayer> history;
    int maxSize;

public:
    History(int maxSize);

    int         getSize();
    int         getMaxSize();
    void        setMaxSize(int size);

    void          addEntry(VisiblePlayer entry);
    VisiblePlayer& getEntry(int index);
    VisiblePlayer& getLast();

};

class PlayerInfo {
protected:
    int     unum;       /*!< Player number */

    SideT   side;       /*!< Player side */
    bool    isGoalie;   /*!< Is player a goalie */

public:
    PlayerInfo();

    int     getUnum();
    void    setUnum(int num);
    SideT   getSide();
    void    setSide(SideT s);
    bool    getIsGoalie();
    void    setIsGoalie(bool goalie);

};

class PlayerObject : public PlayerInfo {
    History history;    /*!< Vision history */

    VecPosition predictedPos;  /*!< Predicted position of player */
public:
    PlayerObject(int maxHistory = HISTORY_SIZE);

    History& getHistory();

    int getLastSeeTime();
    VisiblePlayer& getLastVision();

    VisiblePlayer& getPrevVision();

    void addVision(VisiblePlayer vp);

    VecPosition getPredictedPos();
    void setPredictedPos(VecPosition pos);
};

class Agent : public Object, public PlayerInfo {
protected:

    ViewAngleT   viewAngle;             /*!< View angle of this agent          */
    AngDeg       neckToBody;            /*!< Neck to body angle                */
public:
    Agent();

    ViewAngleT getViewAngle();
    void       setViewAngle(ViewAngleT view);

    AngDeg     getNeckToBodyAngle();
    void       setNeckToBodyAngle(AngDeg ang);
};

class FixedObject
{
    ObjectT type;

    VecPosition relPos;
    int time;
public:
    FixedObject();

    VecPosition getGlobalPos(SideT side = SIDE_LEFT);

    ObjectT getType();
    void setType(ObjectT o);

    VecPosition getRelPos();
    void setRelPos(VecPosition pos);

    int getTime();
    void setTime(int t);
};

#endif // OBJECTS_H
