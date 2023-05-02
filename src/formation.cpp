#include "formation.h"
#include <fstream>
#include <iostream>
#include "parser.h"
#include <unistd.h>
using namespace std;

FormationInfo::FormationInfo()
{

}

void FormationInfo::setPos(VecPosition p, int idx)
{
    pos[idx] = p;
}

void FormationInfo::setX(double x, int idx)
{
    pos[idx].setX(x);
}

void FormationInfo::setY(double y, int idx)
{
    pos[idx].setY(y);
}

VecPosition FormationInfo::getPos(int idx)
{
    return pos[idx];
}

VecPosition FormationInfo::getNearestPosInFormation(VecPosition pos, int *idx )
{
    double minDist = 1000, curDist;
    int minIdx;
    VecPosition min;
    for (int i = 0; i < MAX_TEAMMATES; ++i)
    {
        curDist = pos.getDistanceTo(getPos(i));
        if (curDist < minDist)
        {
            min = getPos(i);
            minDist = curDist;
            minIdx = i;
        }
    }

    if (idx != nullptr) *idx = minIdx;
    return min;
}

void FormationInfo::show(ostream &os)
{
    for (int i = 0; i < MAX_TEAMMATES; ++i)
    {
        os << i << " " << pos[i] << '\n';
    }
}

Formations::Formations(const char* filename, int playerNum)
    : playerNum(playerNum)
{
    if (readFile(filename) == false)
    {
        cerr << "Error while reading formation file " << filename << endl;
    }
    formationIndex = 0;

    // wait to set right order of players
    usleep(playerNum * 500000);
}

void Formations::show( ostream &os )
{
    for( int i = 0 ; i < MAX_FORMATIONS; ++i )
    {
        os << "Formation #" << i << '\n';
        formations[i].show( os );
    }
}

bool Formations::readFile(const char *filename)
{
    ifstream in(filename);
    if( !in )
    {
      cerr << "Could not open file '" <<
        filename << "'" << endl;
      return false;
    }

    bool ret = true;
    char strLine[256], *str;
    int formCount = 0;
    int pNum, pIdx = 0;
    // read all
    while(ret && in.getline(strLine, 256))
    {
        str = &strLine[0];

        // comment and empty lines should be skipped
        if( !(strLine[0] == '\n' || strLine[0] == '#' || strLine[0]=='\0' ||
              Parse::gotoFirstNonSpace( &str ) == '\0' ) )
        {

            pNum = Parse::parseFirstDouble(&str);
            formations[formCount].setX(Parse::parseFirstDouble(&str), pIdx);
            formations[formCount].setY(Parse::parseFirstDouble(&str), pIdx);

            pIdx++;

            if (pIdx == MAX_TEAMMATES)
            {
                pIdx = 0;
                formCount++;
            }
        }
    }

    in.close();


    return ret;
}

void Formations::setFormationIndex(int idx)
{
    formationIndex = idx;
}

int Formations::getFormationIndex()
{
    return formationIndex;
}

FormationInfo Formations::getFormation(int idx)
{
    return formations[idx];
}

int Formations::getPlayerNum()
{
    return playerNum;
}

VecPosition Formations::getAgentPos()
{
    return getFormation(formationIndex).getPos(playerNum-1);
}

