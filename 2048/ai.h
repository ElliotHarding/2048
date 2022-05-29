#ifndef AI_H
#define AI_H

#include <QList>
#include "settings.h"

class AI
{
public:
    AI();

    //Main algorithm - got 2048
    Direction getBestDirection(const std::vector<std::vector<int>>& map);
    int runTests();

private:
    int gameStateScore(const std::vector<std::vector<int>>& map, const int& sumMerges);
    void getHighestScore(int& highScore, int depth);
    void getSpawnStateHighestScore(int& highScore, int depth, const Direction& direction, const double& spawnBlockRatio);

    ///Reuseable memory for depth search
    std::vector<std::vector<std::vector<int>>> m_mapsAtDepths;
    QMap<std::vector<std::vector<int>>, int> m_cacheGameStates;

    ///Snake AI
    std::vector<std::vector<int>> m_snakeGrid;

    ///Map size
    int m_width;
    int m_height;
};

#endif // AI_H
