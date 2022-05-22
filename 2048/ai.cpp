#include "ai.h"
#include <QDebug>

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
            row+=QString::number(map[x][y]);
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
bool mapMove(QVector<QVector<int>>& map, const Direction& direction, int& numMerges, const int& width, const int& height)
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
                            numMerges++;
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
                            numMerges++;
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
                            numMerges++;
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
                            numMerges++;
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
#include <math.h>
int monotonicity(const QVector<QVector<int>>& map, const int& width, const int& height)
{
    int totals[4] = {0,0,0,0};

    const double log2 = log(2);

    //Cols
    for(int x = 0; x < width; x++)
    {
        int current = 0;
        int next = 1;
        while(next < height)
        {
            while(next < height && map[x][next] == 0)
            {
                next++;
            }
            if(next > height-1)
            {
                next--;
            }

            int currentValue = map[x][current] != 0 ? log(map[x][current]) / log2 : 0;
            int nextValue = map[x][next] != 0 ? log(map[x][next]) / log2 : 0;

            if(currentValue > nextValue)
            {
                totals[0] += nextValue - currentValue;
            }
            else
            {
                totals[1] += currentValue - nextValue;
            }

            current = next;
            next++;
        }
    }

    //Rows
    for(int y = 0; y < height; y++)
    {
        int current = 0;
        int next = 1;
        while(next < width)
        {
            while(next < width && map[next][y] == 0)
            {
                next++;
            }
            if(next > width-1)
            {
                next--;
            }

            int currentValue = map[current][y] != 0 ? log(map[current][y]) / log2 : 0;
            int nextValue = map[next][y] != 0 ? log(map[next][y]) / log2 : 0;

            if(currentValue > nextValue)
            {
                totals[2] += nextValue - currentValue;
            }
            else
            {
                totals[3] += currentValue - nextValue;
            }

            current = next;
            next++;
        }
    }

    return std::max(totals[0], totals[1]) + std::max(totals[2], totals[3]);
}

int gameStateScore(const QVector<QVector<int>>& map, const int& numMerges, const int& width, const int& height)
{
    //Number of merges adds to score
    int score = numMerges * Constants::ScoreWeightNumMerges;

    int numZeroBlocks = 0;
    int highestNumber = 0;
    int smoothness = 0;
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            const int mapVal = map[x][y];
            if(x < width-1)
                smoothness -= abs(log(mapVal) - log(map[x+1][y]));
            if(y < height-1)
                smoothness -= abs(log(mapVal) - log(map[x][y+1]));
            if(x > 0)
                smoothness -= abs(log(mapVal) - log(map[x-1][y]));
            if(y > 0)
                smoothness -= abs(log(mapVal)- log(map[x][y-1]));

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
        numZeroBlocks = maxBlocks - 1;
    }

    score += (smoothness * Constants::ScoreWeightSmoothness);///(width*height-numZeroBlocks);

    //Highest number created
    score += highestNumber * Constants::ScoreWeightHighestNumber;

    //Add to score for number of 0 blocks
    score += Constants::ScoreWeightNumberEmptySpots * log(numZeroBlocks);

    score += monotonicity(map, width, height) * Constants::ScoreWeightMonoicity;

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
    int numMerges;

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
                    numMerges = 0;
                    if(mapMove(movedSpawnState, direction, numMerges, width, height))
                    {
                        highScore += gameStateScore(movedSpawnState, numMerges, width, height) * Constants::RatioSpawn2Block;
                        getHighestScore(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height);
                    }
                }

                //Add 4 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 4;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    numMerges = 0;
                    if(mapMove(movedSpawnState, direction, numMerges, width, height))
                    {
                        highScore += gameStateScore(movedSpawnState, numMerges, width, height) * Constants::RatioSpawn4Block;
                        getHighestScore(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height);
                    }
                }
            }
        }
    }
}

void getHighestScoreCacheValues(const QVector<QVector<int>>& map, int& highScore, int depth,
                     QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState,
                     const int& width, const int& height, QMap<QVector<QVector<int>>, int>& cacheValues)
{
    if(depth == 0)
    {
        return;
    }

    //Check cache of previously evaluated game states
    QMap<QVector<QVector<int>>, int>::ConstIterator it = cacheValues.find(map);
    if(it != cacheValues.end())
    {
        highScore += it.value();
        return;
    }

    //Game state evaluation vars
    int numMerges;

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
                    numMerges = 0;
                    if(mapMove(movedSpawnState, direction, numMerges, width, height))
                    {
                        const int score = gameStateScore(movedSpawnState, numMerges, width, height) * Constants::RatioSpawn2Block;
                        cacheValues.insert(map, score);
                        highScore += score;
                        getHighestScoreCacheValues(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height, cacheValues);
                    }
                }

                //Add 4 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                spawnState[x][y] = 4;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    numMerges = 0;
                    if(mapMove(movedSpawnState, direction, numMerges, width, height))
                    {
                        const int score = gameStateScore(movedSpawnState, numMerges, width, height) * Constants::RatioSpawn4Block;
                        cacheValues.insert(map, score);
                        highScore += score;
                        getHighestScoreCacheValues(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, width, height, cacheValues);
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
    int numMerges;

    //For each direction, evaluate its score (to a set depth of moves), choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        moveMap = map;
        numMerges = 0;
        if(mapMove(moveMap, direction, numMerges, width, height))
        {
            mapScore = gameStateScore(moveMap, numMerges, width, height);
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

Direction AI::getBestDirectionCacheValues(const QVector<QVector<int> > &map)
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
    int numMerges;

    //For each direction, evaluate its score (to a set depth of moves), choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        moveMap = map;
        numMerges = 0;
        if(mapMove(moveMap, direction, numMerges, width, height))
        {
            mapScore = gameStateScore(moveMap, numMerges, width, height);
            getHighestScoreCacheValues(moveMap, mapScore, Constants::DirectionChoiceDepth, spawnStateMem, movedSpawnStateMem, width, height, cacheGameStates);
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
    int numMerges;

    //For each direction, evaluate its score, choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        QVector<QVector<int>> moveMap = map;
        numMerges = 0;
        if(mapMove(moveMap, direction, numMerges, width, height))
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
    int numMerges;

    //For each direction, evaluate its score, choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        QVector<QVector<int>> moveMap = map;
        numMerges = 0;
        if(mapMove(moveMap, direction, numMerges, width, height))
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
