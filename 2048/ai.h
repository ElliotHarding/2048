#ifndef AI_H
#define AI_H

#include <QList>
#include "settings.h"

class AI
{
public:
    AI();

    //Main algorithm - got 2048
    Direction getBestDirection(const QVector<QVector<int>>& map);
    int runTests();

private:
    int gameStateScore(const QVector<QVector<int>>& map, const int& sumMerges);
    void getHighestScore(const QVector<QVector<int>>& map, int& highScore, int depth,
                         QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState);

    ///Reuseable memory for depth search
    QVector<QVector<int>> m_spawnStateMem;
    QVector<QVector<int>> m_movedSpawnStateMem;
    QVector<QVector<int>> m_moveMap;
    QMap<QVector<QVector<int>>, int> m_cacheGameStates;

    ///Snake AI
    QVector<QVector<int>> m_snakeGrid;

    ///Map size
    int m_width;
    int m_height;
};

#endif // AI_H
