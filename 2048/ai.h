#ifndef AI_H
#define AI_H

#include <QList>
#include "settings.h"

class AI
{
public:
    AI();

    Direction getBestDirection(const QVector<QVector<int>>& map);
    Direction getBestSmoothnessDirection(const QVector<QVector<int>>& map);
    Direction getBestSmoothness2Direction(const QVector<QVector<int>>& map);
};

#endif // AI_H
