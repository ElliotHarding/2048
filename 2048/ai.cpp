#include "ai.h"
#include <QDebug>
#include <QRandomGenerator>
#include <math.h>
const double Log2 = log(2);

AI::AI()
{
}

void debugMap(const QVector<QVector<int>>& map)
{
    qDebug() << "----- Map ------";
    for(int y = 0; y < map[0].size(); y++)
    {
        QString row = "";
        for(int x = 0; x < map.size(); x++)
        {
            row+=QString::number(map[x][y])+":";
        }
        qDebug() << row;
    }
}

void debugMap(const QVector<QVector<double>>& map)
{
    qDebug() << "----- Map ------";
    for(int y = 0; y < map[0].size(); y++)
    {
        QString row = "";
        for(int x = 0; x < map.size(); x++)
        {
            row+=QString::number(map[x][y])+":";
        }
        qDebug() << row;
    }
}

void debugDirection(const Direction& direction)
{
    switch (direction)
    {
        case UP:
            qDebug("Direction: UP");
            break;
        case DOWN:
            qDebug("Direction: DOWN");
            break;
        case LEFT:
            qDebug("Direction: LEFT");
            break;
        case RIGHT:
            qDebug("Direction: RIGHT");
            break;
    }
}

//Returns true if anything moved
bool mapMove(QVector<QVector<int>>& map, const Direction& direction, int& sumMerges, const int& width, const int& height)
{
    bool anyMoved = false;
    if(direction == RIGHT)
    {
        for(int moveCount = 0; moveCount < width-1; moveCount++)
        {
            bool moved = false;
            for(int x = width-2; x > -1; x--)
            {
                for(int y = 0; y < height; y++)
                {
                    if(map[x][y] != 0)
                    {
                        if(map[x+1][y] == 0)
                        {
                            map[x+1][y] = map[x][y];
                            map[x][y] = 0;
                            moved = true;
                        }
                        else if(map[x+1][y] == map[x][y])
                        {
                            map[x+1][y] *= 2;
                            map[x][y] = 0;
                            moved = true;
                            sumMerges += map[x+1][y];
                        }
                    }
                }
            }
            if(moved)
            {
                anyMoved = true;
            }
            else
            {
                return anyMoved;
            }
        }
    }
    else if(direction == LEFT)
    {
        for(int moveCount = 0; moveCount < width-1; moveCount++)
        {
            bool moved = false;
            for(int x = 1; x < width; x++)
            {
                for(int y = 0; y < height; y++)
                {
                    if(map[x][y] != 0)
                    {
                        if(map[x-1][y] == 0)
                        {
                            map[x-1][y] = map[x][y];
                            map[x][y] = 0;
                            moved = true;
                        }
                        else if(map[x-1][y] == map[x][y])
                        {
                            map[x-1][y] *= 2;
                            map[x][y] = 0;
                            moved = true;
                            sumMerges += map[x-1][y];
                        }
                    }
                }
            }
            if(moved)
            {
                anyMoved = true;
            }
            else
            {
                return anyMoved;
            }
        }
    }
    else if(direction == DOWN)
    {
        for(int moveCount = 0; moveCount < height-1; moveCount++)
        {
            bool moved = false;
            for(int x = 0; x < width; x++)
            {
                for(int y = height-2; y > -1; y--)
                {
                    if(map[x][y] != 0)
                    {
                        if(map[x][y+1] == 0)
                        {
                            map[x][y+1] = map[x][y];
                            map[x][y] = 0;
                            moved = true;
                        }
                        else if(map[x][y+1] == map[x][y])
                        {
                            map[x][y+1] *= 2;
                            map[x][y] = 0;
                            moved = true;
                            sumMerges += map[x][y+1];
                        }
                    }
                }
            }
            if(moved)
            {
                anyMoved = true;
            }
            else
            {
                return anyMoved;
            }
        }
    }
    else if(direction == UP)
    {
        for(int moveCount = 0; moveCount < height-1; moveCount++)
        {
            bool moved = false;
            for(int x = 0; x < width; x++)
            {
                for(int y = 1; y < height; y++)
                {
                    if(map[x][y] != 0)
                    {
                        if(map[x][y-1] == 0)
                        {
                            map[x][y-1] = map[x][y];
                            map[x][y] = 0;
                            moved = true;
                        }
                        else if(map[x][y-1] == map[x][y])
                        {
                            map[x][y-1] *= 2;
                            map[x][y] = 0;
                            moved = true;
                            sumMerges += map[x][y-1];
                        }
                    }
                }
            }
            if(moved)
            {
                anyMoved = true;
            }
            else
            {
                return anyMoved;
            }
        }
    }
    return anyMoved;
}

int smoothGameStateScore(const QVector<QVector<int>>& map)
{
    int smoothness = 0;
    for(int x = 1; x < map.size()-1; x++)
    {
        for(int y = 1; y < map[x].size()-1; y++)
        {
            const int mapVal = map[x][y];
            smoothness -= abs(mapVal - map[x+1][y]);
            smoothness -= abs(mapVal - map[x][y+1]);
            smoothness -= abs(mapVal - map[x-1][y]);
            smoothness -= abs(mapVal - map[x][y-1]);
        }
    }
    return smoothness;
}

int smooth2GameStateScore(const QVector<QVector<int>>& map)
{
    int smoothness = 0;
    for(int x = 0; x < map.size(); x++)
    {
        for(int y = 0; y < map[x].size(); y++)
        {
            const int mapVal = map[x][y];
            if(x < map.size()-1)
                smoothness -= abs(mapVal - map[x+1][y]);
            if(y < map[x].size()-1)
                smoothness -= abs(mapVal - map[x][y+1]);
            if(x > 0)
                smoothness -= abs(mapVal - map[x-1][y]);
            if(y > 0)
                smoothness -= abs(mapVal - map[x][y-1]);
        }
    }
    return smoothness;
}

int monotonicity(const QVector<QVector<double>>& map, const int& width, const int& height)
{
    double totals[4] = {0,0,0,0};

    int i;
    int iNext;
    double val;
    double valNext;

    //Cols
    for(int x = 0; x < width; x++)
    {
        i = 0;
        iNext = 1;
        while(iNext < height)
        {
            val = map[x][i];

            //Find next value along col
            while(iNext < height && map[x][iNext] == 0)
            {
                iNext++;
            }

            //If found no next value, continue to next row
            if(iNext > height-1)
            {
                totals[0] -= val;
                continue;
            }

            valNext = map[x][iNext];

            if(val > valNext)
            {
                totals[0] += valNext - val;
            }
            else
            {
                totals[1] += val - valNext;
            }

            i = iNext;
            iNext++;
        }
    }

    //Rows
    for(int y = 0; y < height; y++)
    {
        i = 0;
        iNext = 1;
        while(iNext < width)
        {
            val = map[i][y];

            //Find next value along row
            while(iNext < width && map[iNext][y] == 0)
            {
                iNext++;
            }

            //If found no next value, continue to next row
            if(iNext > width-1)
            {
                totals[2] -= val;
                continue;
            }

            valNext = map[iNext][y];

            if(val > valNext)
            {
                totals[2] += valNext - val;
            }
            else
            {
                totals[3] += val - valNext;
            }

            i = iNext;
            iNext++;
        }
    }

    return std::max(totals[0], totals[1]) + std::max(totals[2], totals[3]);
}

int gameStateScore(const QVector<QVector<int>>& map, const int& sumMerges, const int& width, const int& height)
{
    int numZeroBlocks = 0;
    int highestNumber = 0;
    int smoothness = 0;
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            const int mapVal = map[x][y];
            if(x < width-1)
                smoothness -= abs(mapVal - map[x+1][y]);
            if(y < height-1)
                smoothness -= abs(mapVal - map[x][y+1]);

            if(mapVal == 0)
            {
                numZeroBlocks++;
            }
            else if(highestNumber < mapVal)
            {
                highestNumber = mapVal;
            }
        }
    }

    const int maxBlocks = width * height;
    if(numZeroBlocks == maxBlocks)
    {
        numZeroBlocks--;
    }
    else if(numZeroBlocks == 0)
    {
        return 0;
    }

    //Number of merges adds to score
    int score = sumMerges * Constants::ScoreWeightSumMerges;

    //Smoothness
    score += (smoothness * Constants::ScoreWeightSmoothness)/(width*height-numZeroBlocks);

    //Highest number created
    score += highestNumber * Constants::ScoreWeightHighestNumber;

    //Add to score for number of 0 blocks
    score += numZeroBlocks * Constants::ScoreWeightNumberEmptySpots;

    return score > 0 ? score : 0;
}

int gameStateScore_snake(const QVector<QVector<int>>& map, const QVector<QVector<int>>& snakeGrid, const int& width, const int& height)
{
    int score = 0;
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            score += map[x][y] * snakeGrid[x][y];
        }
    }
    return score;
}

int gameStateScore_cache(const QVector<QVector<int>>& map, const int& sumMerges, const int& width, const int& height, QMap<QVector<QVector<int>>, int>& cacheValues)
{
    QMap<QVector<QVector<int>>, int>::ConstIterator it = cacheValues.find(map);
    if(it != cacheValues.end())
    {
        return it.value();
    }

    int numZeroBlocks = 0;
    int highestNumber = 0;
    int smoothness = 0;
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            const int mapVal = map[x][y];
            if(x < width-1)
                smoothness -= abs(mapVal - map[x+1][y]);
            if(y < height-1)
                smoothness -= abs(mapVal - map[x][y+1]);

            if(mapVal == 0)
            {
                numZeroBlocks++;
            }
            else if(highestNumber < mapVal)
            {
                highestNumber = mapVal;
            }
        }
    }

    const int maxBlocks = width * height;
    if(numZeroBlocks == maxBlocks)
    {
        numZeroBlocks--;
    }
    else if(numZeroBlocks == 0)
    {
        return 0;
    }

    //Number of merges adds to score
    int score = sumMerges * Constants::ScoreWeightSumMerges;

    //Smoothness
    score += (smoothness * Constants::ScoreWeightSmoothness)/(width*height-numZeroBlocks);

    //Highest number created
    score += highestNumber * Constants::ScoreWeightHighestNumber;

    //Add to score for number of 0 blocks
    score += numZeroBlocks * Constants::ScoreWeightNumberEmptySpots;

    const int finalScore = score > 0 ? score : 0;
    cacheValues.insert(map, finalScore);
    return finalScore;
}

//Returns map values after --> log(value)/log(2)
QVector<QVector<double>> log2Map(const QVector<QVector<int>>& map, const int& width, const int& height)
{
    QVector<QVector<double>> returnMap(width, QVector<double>(height, 0));;
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            if(map[x][y] != 0)
            {
                returnMap[x][y] = log(map[x][y])/Log2;
            }
        }
    }

    return returnMap;
}

int gameStateScore_monoicity(const QVector<QVector<int>>& map, const int&/*Dont do anything with merges atm*/, const int& width, const int& height)
{
    //Returns map values after --> log(value)/log(2)
    QVector<QVector<double>> logMap = log2Map(map, width, height);

    int numZeroBlocks = 0;
    double highestNumber = 0;
    double smoothness = 0;
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            const int mapVal = logMap[x][y];
            if(mapVal == 0)
            {
                numZeroBlocks++;
            }
            else
            {
                if(highestNumber < mapVal)
                {
                    highestNumber = mapVal;
                }

                if(x < width-1 && logMap[x+1][y] != 0)
                    smoothness -= abs(mapVal - logMap[x+1][y]);
                if(y < height-1 && logMap[x][y+1] != 0)
                    smoothness -= abs(mapVal - logMap[x][y+1]);
            }
        }
    }

    int score = Constants::BaseScore_Monoicity;

    //Smoothness
#ifdef AI_DEBUG
    const int smooth = smoothness * Constants::ScoreWeight_Monoicity_Smoothness;
#endif
    score += smoothness * Constants::ScoreWeight_Monoicity_Smoothness;

    //Highest number created
    score += highestNumber * Constants::ScoreWeight_Monoicity_HighestNumber;

    //Add to score for number of 0 blocks
    score += log(numZeroBlocks) * Constants::ScoreWeight_Monoicity_NumberEmptySpots;

    //Monoicity
#ifdef AI_DEBUG
    const int monoton = monotonicity(logMap, width, height) * Constants::ScoreWeight_Monoicity;
#endif
    score += monotonicity(logMap, width, height) * Constants::ScoreWeight_Monoicity;

#ifdef AI_DEBUG
    if(score < 0)
    {
        qDebug() << "gameStateScore_monoicity: score: " << score;
        qDebug() << "Smooth: " << smooth;
        qDebug() << "Monton: " << monoton;
    }
#endif

    return score > 0 ? score : 0;
}

int gameStateScore_monoicity_cache(const QVector<QVector<int>>& map, const int&/*Dont do anything with merges atm*/, const int& width, const int& height, QMap<QVector<QVector<int>>, int>& cacheValues)
{
    QMap<QVector<QVector<int>>, int>::ConstIterator it = cacheValues.find(map);
    if(it != cacheValues.end())
    {
        return it.value();
    }

    //Returns map values after --> log(value)/log(2)
    QVector<QVector<double>> logMap = log2Map(map, width, height);

    int numZeroBlocks = 0;
    double highestNumber = 0;
    double smoothness = 0;
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            const int mapVal = logMap[x][y];
            if(mapVal == 0)
            {
                numZeroBlocks++;
            }
            else
            {
                if(highestNumber < mapVal)
                {
                    highestNumber = mapVal;
                }

                if(x < width-1 && logMap[x+1][y] != 0)
                    smoothness -= abs(mapVal - logMap[x+1][y]);
                if(y < height-1 && logMap[x][y+1] != 0)
                    smoothness -= abs(mapVal - logMap[x][y+1]);
            }
        }
    }

    int score = Constants::BaseScore_Monoicity;

    //Smoothness
#ifdef AI_DEBUG
    const int smooth = smoothness * Constants::ScoreWeight_Monoicity_Smoothness;
#endif
    score += smoothness * Constants::ScoreWeight_Monoicity_Smoothness;

    //Highest number created
    score += highestNumber * Constants::ScoreWeight_Monoicity_HighestNumber;

    //Add to score for number of 0 blocks
    score += log(numZeroBlocks) * Constants::ScoreWeight_Monoicity_NumberEmptySpots;

    //Monoicity
#ifdef AI_DEBUG
    const int monoton = monotonicity(logMap, width, height) * Constants::ScoreWeight_Monoicity;
#endif
    score += monotonicity(logMap, width, height) * Constants::ScoreWeight_Monoicity;

#ifdef AI_DEBUG
    if(score < 0)
    {
        qDebug() << "gameStateScore_monoicity: score: " << score;
        qDebug() << "Smooth: " << smooth;
        qDebug() << "Monton: " << monoton;
    }
#endif

    const int finalScore = score > 0 ? score : 0;
    cacheValues.insert(map, finalScore);
    return finalScore;
}

void getHighestScore(const QVector<QVector<int>>& map, int& highScore, int depth,
                     QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState,
                     const int& width, const int& height)
{
    if(depth == 0)
    {
        return;
    }

    //Game state evaluation vars
    int sumMerges;
#ifdef NO_SUM_SCORES
    int score;
#endif

    //Loop through map, if find an empty spot add a 2 & 4 to that spot (spawnState)
    // then evaluate spawnState (to a set depth of moves)
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            if(map[x][y] == 0)
            {
                spawnState = map;

                //Add 2 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 2;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
#ifdef NO_SUM_SCORES
                        score = gameStateScore(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn2Block / (depth * Constants::DepthMultiplier);
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += gameStateScore(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn2Block / (depth * 0.1);
#endif
                        getHighestScore(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height);
                    }
                }

                //Add 4 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 4;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
#ifdef NO_SUM_SCORES
                        score = gameStateScore(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn2Block / (depth * Constants::DepthMultiplier);
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += gameStateScore(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn4Block / (depth * 0.1);
#endif
                        getHighestScore(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height);
                    }
                }
            }
        }
    }
}

void getHighestScore_snake(const QVector<QVector<int>>& map, int& highScore, int depth,
                     QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState,
                     const int& width, const int& height, const QVector<QVector<int>>& snakeGrid)
{
    if(depth == 0)
    {
        return;
    }

    //Game state evaluation vars
    int sumMerges;
#ifdef NO_SUM_SCORES
    int score;
#endif

    //Loop through map, if find an empty spot add a 2 & 4 to that spot (spawnState)
    // then evaluate spawnState (to a set depth of moves)
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            if(map[x][y] == 0)
            {
                spawnState = map;

                //Add 2 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 2;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
#ifdef NO_SUM_SCORES
                        score = gameStateScore_snake(movedSpawnState, snakeGrid, width, height) * Constants::RatioSpawn2Block;
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += gameStateScore_snake(movedSpawnState, snakeGrid, width, height) * Constants::RatioSpawn2Block;
#endif
                        getHighestScore_snake(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height, snakeGrid);
                    }
                }

                //Add 4 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 4;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
#ifdef NO_SUM_SCORES
                        score = gameStateScore_snake(movedSpawnState, snakeGrid, width, height) * Constants::RatioSpawn2Block;
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += gameStateScore_snake(movedSpawnState, snakeGrid, width, height) * Constants::RatioSpawn4Block;
#endif
                        getHighestScore_snake(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height, snakeGrid);
                    }
                }
            }
        }
    }
}

void getHighestScore_monoicity(const QVector<QVector<int>>& map, int& highScore, int depth,
                     QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState,
                     const int& width, const int& height)
{
    if(depth == 0)
    {
        return;
    }

    //Game state evaluation vars
    int sumMerges;
#ifdef NO_SUM_SCORES
    int score;
#endif

    //Loop through map, if find an empty spot add a 2 & 4 to that spot (spawnState)
    // then evaluate spawnState (to a set depth of moves)
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            if(map[x][y] == 0)
            {
                spawnState = map;

                //Add 2 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 2;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
#ifdef NO_SUM_SCORES
                        score = gameStateScore_monoicity(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn2Block;
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += gameStateScore_monoicity(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn2Block;
#endif
                        getHighestScore_monoicity(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height);
                    }
                }

                //Add 4 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 4;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
#ifdef NO_SUM_SCORES
                        score = gameStateScore_monoicity(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn4Block;
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += gameStateScore_monoicity(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn4Block;
#endif
                        getHighestScore_monoicity(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height);
                    }
                }
            }
        }
    }
}

void getHighestScore_cache(const QVector<QVector<int>>& map, int& highScore, int depth,
                     QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState,
                     const int& width, const int& height, QMap<QVector<QVector<int>>, int>& cacheValues)
{
    if(depth == 0)
    {
        return;
    }

    //Game state evaluation vars
    int sumMerges;
    int score;

    //Loop through map, if find an empty spot add a 2 & 4 to that spot (spawnState)
    // then evaluate spawnState (to a set depth of moves)
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            if(map[x][y] == 0)
            {
                spawnState = map;

                //Add 2 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 2;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
                        score = gameStateScore_cache(movedSpawnState, sumMerges, width, height, cacheValues) * Constants::RatioSpawn2Block;
#ifdef NO_SUM_SCORES
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += score;
#endif
                        getHighestScore_cache(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height, cacheValues);
                    }
                }

                //Add 4 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 4;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
                        score = gameStateScore_cache(movedSpawnState, sumMerges, width, height, cacheValues) * Constants::RatioSpawn4Block;
#ifdef NO_SUM_SCORES
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += score;
#endif
                        getHighestScore_cache(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height, cacheValues);
                    }
                }
            }
        }
    }
}

void getHighestScore_monoicity_cache(const QVector<QVector<int>>& map, int& highScore, int depth,
                     QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState,
                     const int& width, const int& height, QMap<QVector<QVector<int>>, int>& cacheValues)
{
    if(depth == 0)
    {
        return;
    }

    //Game state evaluation vars
    int sumMerges;
    int score;

    //Loop through map, if find an empty spot add a 2 & 4 to that spot (spawnState)
    // then evaluate spawnState (to a set depth of moves)
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            if(map[x][y] == 0)
            {
                spawnState = map;

                //Add 2 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 2;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
                        score = gameStateScore_monoicity_cache(movedSpawnState, sumMerges, width, height, cacheValues) * Constants::RatioSpawn2Block;
#ifdef NO_SUM_SCORES
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += score;
#endif
                        getHighestScore_monoicity_cache(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height, cacheValues);
                    }
                }

                //Add 4 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 4;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    sumMerges = 0;
                    if(mapMove(movedSpawnState, direction, sumMerges, width, height))
                    {
                        score = gameStateScore_monoicity_cache(movedSpawnState, sumMerges, width, height, cacheValues) * Constants::RatioSpawn4Block;
#ifdef NO_SUM_SCORES
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += score;
#endif
                        getHighestScore_monoicity_cache(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height, cacheValues);
                    }
                }
            }
        }
    }
}

Direction AI::getBestDirection(const QVector<QVector<int>>& map)
{
    Direction chosenDirection = Constants::PossibleMoveDirections[0];

    //Size of map, quicker to retrieve from const
    const int width = map.size();
    const int height = map[0].size();

    //Reuseable memory for depth search
    QVector<QVector<int>> spawnStateMem = map;
    QVector<QVector<int>> movedSpawnStateMem = map;
    QVector<QVector<int>> moveMap = map;

    //Game state evaluation vars
    int score = 0;
    int mapScore;
    int sumMerges;

    //For each direction, evaluate its score (to a set depth of moves), choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        moveMap = map;
        sumMerges = 0;
        if(mapMove(moveMap, direction, sumMerges, width, height))
        {
            mapScore = gameStateScore(moveMap, sumMerges, width, height);
            getHighestScore(moveMap, mapScore, Constants::DirectionChoiceDepth, spawnStateMem, movedSpawnStateMem, width, height);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }

    return chosenDirection;
}

Direction AI::getBestDirection_monoicity(const QVector<QVector<int>>& map)
{
    Direction chosenDirection = Constants::PossibleMoveDirections[0];

    //Size of map, quicker to retrieve from const
    const int width = map.size();
    const int height = map[0].size();

    //Reuseable memory for depth search
    QVector<QVector<int>> spawnStateMem = map;
    QVector<QVector<int>> movedSpawnStateMem = map;
    QVector<QVector<int>> moveMap = map;

    //Game state evaluation vars
    int score = 0;
    int mapScore;
    int sumMerges;

    //For each direction, evaluate its score (to a set depth of moves), choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        moveMap = map;
        sumMerges = 0;
        if(mapMove(moveMap, direction, sumMerges, width, height))
        {
            mapScore = gameStateScore_monoicity(moveMap, sumMerges, width, height);
            getHighestScore_monoicity(moveMap, mapScore, Constants::DirectionChoiceDepth, spawnStateMem, movedSpawnStateMem, width, height);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }

    return chosenDirection;
}

Direction AI::getBestDirection_cache(const QVector<QVector<int> > &map)
{
    Direction chosenDirection = Constants::PossibleMoveDirections[0];

    //Size of map, quicker to retrieve from const
    const int width = map.size();
    const int height = map[0].size();

    //Reuseable memory for depth search
    QVector<QVector<int>> spawnStateMem = map;
    QVector<QVector<int>> movedSpawnStateMem = map;
    QVector<QVector<int>> moveMap = map;

    //Previously evaluated game states
    QMap<QVector<QVector<int>>, int> cacheGameStates;

    //Game state evaluation vars
    int score = 0;
    int mapScore;
    int sumMerges;

    //For each direction, evaluate its score (to a set depth of moves), choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        moveMap = map;
        sumMerges = 0;
        if(mapMove(moveMap, direction, sumMerges, width, height))
        {
            mapScore = gameStateScore(moveMap, sumMerges, width, height);
            getHighestScore_cache(moveMap, mapScore, Constants::DirectionChoiceDepth, spawnStateMem, movedSpawnStateMem, width, height, cacheGameStates);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }

    return chosenDirection;
}

Direction AI::getBestDirection_monoicity_cache(const QVector<QVector<int> > &map)
{
    Direction chosenDirection = Constants::PossibleMoveDirections[0];

    //Size of map, quicker to retrieve from const
    const int width = map.size();
    const int height = map[0].size();

    //Reuseable memory for depth search
    QVector<QVector<int>> spawnStateMem = map;
    QVector<QVector<int>> movedSpawnStateMem = map;
    QVector<QVector<int>> moveMap = map;

    //Previously evaluated game states
    QMap<QVector<QVector<int>>, int> cacheGameStates;

    //Game state evaluation vars
    int score = 0;
    int mapScore;
    int sumMerges;

    //For each direction, evaluate its score (to a set depth of moves), choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        moveMap = map;
        sumMerges = 0;
        if(mapMove(moveMap, direction, sumMerges, width, height))
        {
            mapScore = gameStateScore_monoicity(moveMap, sumMerges, width, height);
            getHighestScore_monoicity_cache(moveMap, mapScore, Constants::DirectionChoiceDepth, spawnStateMem, movedSpawnStateMem, width, height, cacheGameStates);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }

    return chosenDirection;
}

Direction AI::getBestDirection_snake(const QVector<QVector<int> > &map)
{
    Direction chosenDirection = Constants::PossibleMoveDirections[0];

    //Size of map, quicker to retrieve from const
    const int width = map.size();
    const int height = map[0].size();

    //Reuseable memory for depth search
    QVector<QVector<int>> spawnStateMem = map;
    QVector<QVector<int>> movedSpawnStateMem = map;
    QVector<QVector<int>> moveMap = map;

    QVector<QVector<int>> snakeGrid(width, QVector<int>(height, 0));
    bool startTop = true;
    int startVal = 1;
    for(int x = 0; x < width; x++)
    {
        if(startTop)
        {
            for(int y = 0; y < height; y++)
            {
                snakeGrid[x][y] = startVal;
                startVal*=2;
            }
        }
        else
        {
            for(int y = height-1; y > -1; y--)
            {
                snakeGrid[x][y] = startVal;
                startVal*=2;
            }
        }
        startTop = !startTop;
    }

    //Game state evaluation vars
    int score = 0;
    int mapScore;
    int sumMerges;

    //For each direction, evaluate its score (to a set depth of moves), choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        moveMap = map;
        sumMerges = 0;
        if(mapMove(moveMap, direction, sumMerges, width, height))
        {
            mapScore = gameStateScore_snake(moveMap, snakeGrid, width, height);
            getHighestScore_snake(moveMap, mapScore, Constants::DirectionChoiceDepth, spawnStateMem, movedSpawnStateMem, width, height, snakeGrid);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }

    return chosenDirection;
}

Direction AI::getBestSmoothnessDirection(const QVector<QVector<int>>& map)
{
    Direction chosenDirection = Constants::PossibleMoveDirections[0];

    //Size of map, quicker to retrieve from const
    const int width = map.size();
    const int height = map[0].size();

    //Game state evaluation vars
    int score = -99999999;
    int mapScore;
    int sumMerges;

    //For each direction, evaluate its score, choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        QVector<QVector<int>> moveMap = map;
        sumMerges = 0;
        if(mapMove(moveMap, direction, sumMerges, width, height))
        {
            mapScore = smoothGameStateScore(moveMap);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }

    return chosenDirection;
}

Direction AI::getBestSmoothness2Direction(const QVector<QVector<int>>& map)
{
    Direction chosenDirection = Constants::PossibleMoveDirections[0];

    //Size of map, quicker to retrieve from const
    const int width = map.size();
    const int height = map[0].size();

    //Game state evaluation vars
    int score = -99999999;
    int mapScore;
    int sumMerges;

    //For each direction, evaluate its score, choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        QVector<QVector<int>> moveMap = map;
        sumMerges = 0;
        if(mapMove(moveMap, direction, sumMerges, width, height))
        {
            mapScore = smooth2GameStateScore(moveMap);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }

    return chosenDirection;
}

int AI::runTests()
{
    const int width = 4;
    const int height = 4;
    int sumMerges;
    while(true)
    {
        qDebug() << "AI::runTests - Starting test";
        QVector<QVector<int>> map(width, QVector<int>(height, 0));
        map[0][0] = 2;

        while(true)
        {
            Direction dir = getBestDirection(map);
            sumMerges = 0;
            if(mapMove(map, dir, sumMerges, width, height))
            {
                //Spawn new tile

                //Find empty spaces
                QVector<QPoint> emptySpaces;
                for(int x = 0; x < width; x++)
                {
                    for(int y = 0; y < height; y++)
                    {
                        if(map[x][y] == 0)
                        {
                            emptySpaces.push_back(QPoint(x,y));
                        }
                    }
                }

                //If cant find any empty spaces
                if(emptySpaces.empty())
                {
                    qDebug() << "AI::runTests - Cant find empty space to spawn block, but game not over";
                }

                const int randomPosition = QRandomGenerator::global()->generateDouble() * emptySpaces.size() - 1;
                const QPoint spawnPos = emptySpaces[randomPosition];

                const int randomStartOption = QRandomGenerator::global()->generateDouble() * 100;
                const int startValue = randomStartOption < Constants::PercentageSpawn2block ? 2 : 4;

                map[spawnPos.x()][spawnPos.y()] = startValue;
            }
            else
            {
                qDebug() << "AI::runTests - GAME OVER!";
                int highestNumber = 0;
                for(int x = 0; x < width; x++)
                {
                    for(int y = 0; y < height; y++)
                    {
                        if(highestNumber < map[x][y])
                        {
                            highestNumber = map[x][y];
                        }
                    }
                }

                if(highestNumber >= 2048)
                {
                    qDebug() << "------ 2048 REACHED! ------------";
                    qDebug() << "------ 2048 REACHED! ------------";
                    qDebug() << "------ 2048 REACHED! ------------";
                    qDebug() << "------ 2048 REACHED! ------------";
                    qDebug() << "------ 2048 REACHED! ------------";
                    qDebug() << "------ 2048 REACHED! ------------";
                }
                else
                {
                    qDebug() << "AI::runTests - Score: " << highestNumber;
                }
                debugMap(map);
                break;
            }
        }
    }


}
