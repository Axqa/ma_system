#ifndef STRATEGY_H
#define STRATEGY_H
#include "utilities.h"
#include <vector>

class Strategy
{
    vector<VecPosition> targets[MAX_TEAMMATES];
    int timeForTarget;
public:
    Strategy(const char* filename);

    int getTimeForTarget();
    void setTimeForTarget(int time);

    VecPosition getStrategyTargetForTime(int unum, int time);
private:
    bool readFile(const char* filename);
};

#endif // STRATEGY_H
