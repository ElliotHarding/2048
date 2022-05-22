#ifndef AI_H
#define AI_H

#include <QList>
#include "settings.h"

class AI//Probably could make a static function for AI
{
public:
    AI();

    Direction getBestDirection(const QVector<QVector<int>>& map);
    Direction getBestDirection_monoicity(const QVector<QVector<int>>& map);
    Direction getBestDirection_cache(const QVector<QVector<int>>& map);
    Direction getBestDirection_monoicity_cache(const QVector<QVector<int>>& map);

    Direction getBestSmoothnessDirection(const QVector<QVector<int>>& map);
    Direction getBestSmoothness2Direction(const QVector<QVector<int>>& map);

    int runTests();
};

#endif // AI_H
