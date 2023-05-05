#include "strategy.h"
#include "parser.h"
#include <fstream>
#include <iostream>

Strategy::Strategy(const char* filename)
{
    if (readFile(filename) == false)
    {
        cerr << "Error while reading strategy file " << filename << endl;
    }

    timeForTarget = 50;
}

int Strategy::getTimeForTarget()
{
    return timeForTarget;
}

void Strategy::setTimeForTarget(int time)
{
    timeForTarget = time;
}

VecPosition Strategy::getStrategyTargetForTime(int unum, int time)
{
    int curTargetNum = (time / getTimeForTarget()) % targets[unum-1].size();

    return targets[unum-1][curTargetNum];
}

bool Strategy::readFile(const char *filename)
{
    ifstream in(filename);

    if( !in )
    {
      cerr << "Could not open file '" <<
        filename << "'" << endl;
      return false;
    }

    char line[400], *str;
    int curX, curY;
    bool ret = true;

    int curLine = -1;

    while(ret && in.getline(line, 400))
    {
        str = &line[0];
        // comment and empty lines should be skipped
        if( !(line[0] == '\n' || line[0] == '#' || line[0]=='\0' ||
              Parse::gotoFirstNonSpace( &str ) == '\0' ) )
        {
            curLine++;
            while (Parse::gotoFirstOccurenceOf('(', &str) != -1)
            {
                curX = Parse::parseFirstInt(&str);
                curY = Parse::parseFirstInt(&str);
                targets[curLine].push_back(VecPosition(curX, curY));
            }
        }
    }

    in.close();

    return ret;
}
