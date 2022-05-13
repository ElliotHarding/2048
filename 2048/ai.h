#ifndef AI_H
#define AI_H

#include <QList>
#include "settings.h"

class AI
{
public:
    AI();

    Vector2 getBestDirection(const QVector<QVector<int>>& map);
};

#endif // AI_H
