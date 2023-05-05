#ifndef FORMATION_H
#define FORMATION_H
#include "utilities.h"
#include <iostream>

class FormationInfo
{
    VecPosition pos[MAX_TEAMMATES];

public:
    FormationInfo();

    void setPos(VecPosition p, int idx);
    void setX(double x, int idx);
    void setY(double y, int idx);

    VecPosition getPos(int idx);

    VecPosition getNearestPosInFormation(VecPosition pos, int *idx = nullptr);

    void show(ostream &os = cout);
};

class Formations
{
    FormationInfo formations[MAX_FORMATIONS];
    int formationIndex;
    int playerNum;
public:
    Formations(const char* filename, int playerNum);

    void show( ostream &os = cout );

    void setFormationIndex(int idx);
    int  getFormationIndex();

    FormationInfo getFormation(int idx);

    int getPlayerNum();

    VecPosition getAgentPos();
    VecPosition getPlayerPos(int unum);
private:
    bool readFile(const char* filename);
};

#endif // FORMATION_H
