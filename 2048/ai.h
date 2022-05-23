#ifndef AI_H
#define AI_H

#include <QList>
#include "settings.h"

class AI//Probably could make a static function for AI
{
public:
    AI();

    //Main algorithm - got 2048
    Direction getBestDirection(const QVector<QVector<int>>& map);
    int runTests();


    //Other algorithms
    Direction getBestDirection_snake(const QVector<QVector<int>>& map);
    Direction getBestSmoothnessDirection(const QVector<QVector<int>>& map);
    Direction getBestSmoothness2Direction(const QVector<QVector<int>>& map);

private:
    int gameStateScore(const QVector<QVector<int>>& map, const int& sumMerges);
    void getHighestScore(const QVector<QVector<int>>& map, int& highScore, int depth,
                         QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState);

    ///Reuseable memory for depth search
    QVector<QVector<int>> m_spawnStateMem;
    QVector<QVector<int>> m_movedSpawnStateMem;
    QVector<QVector<int>> m_moveMap;

    QVector<QVector<int>> m_snakeGrid;

    QMap<QVector<QVector<int>>, int> m_cacheGameStates;

    ///Map size
    int m_width;
    int m_height;
};

#endif // AI_H
