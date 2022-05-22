#ifndef AI_H
#define AI_H

#include <QList>
#include "settings.h"

class AI//Probably could make a static function for AI
{
public:
    AI();

    Direction getBestDirection(const QVector<QVector<int>>& map);
    int runTests();
};

#endif // AI_H
