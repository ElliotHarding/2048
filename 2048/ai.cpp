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
                        score = gameStateScore(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn2Block / (depth * 0.1);
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
                        score = gameStateScore(movedSpawnState, sumMerges, width, height) * Constants::RatioSpawn2Block / (depth * 0.1);
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
