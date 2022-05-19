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

bool mapMove(QVector<QVector<int>>& map, const Direction& direction, int& numMerges, const int& xWidth, const int& yWidth)
{
    bool anyMoved = false;
    if(direction == RIGHT)
    {
        for(int moveCount = 0; moveCount < xWidth-1; moveCount++)
        {
            bool moved = false;
            for(int x = xWidth-2; x > -1; x--)
            {
                for(int y = 0; y < yWidth; y++)
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
        for(int moveCount = 0; moveCount < xWidth-1; moveCount++)
        {
            bool moved = false;
            for(int x = 1; x < xWidth; x++)
            {
                for(int y = 0; y < yWidth; y++)
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
        for(int moveCount = 0; moveCount < yWidth-1; moveCount++)
        {
            bool moved = false;
            for(int x = 0; x < xWidth; x++)
            {
                for(int y = yWidth-2; y > -1; y--)
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
        for(int moveCount = 0; moveCount < yWidth-1; moveCount++)
        {
            bool moved = false;
            for(int x = 0; x < xWidth; x++)
            {
                for(int y = 1; y < yWidth; y++)
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

struct NumberAndLocation
{
    int number;
    int posX;
    int posY;
};

bool compareNumberAndLocation(const NumberAndLocation &a, const NumberAndLocation &b)
{
    return a.number < b.number;
}

int gameStateScore(const QVector<QVector<int>>& map, QVector<NumberAndLocation>& blockValues, const int& numMerges, const int& xWidth, const int& yWidth)
{
    //Number of merges adds to score
    int score = numMerges * Constants::ScoreWeightNumMerges;

    int numZeroBlocks = 0;
    int iBlockValues = 0;
    for(int x = 0; x < xWidth; x++)
    {
        for(int y = 0; y < yWidth; y++)
        {
            if(map[x][y] == 0)
            {
                numZeroBlocks++;
            }
            blockValues[iBlockValues].number = map[x][y];
            blockValues[iBlockValues].posX = x;
            blockValues[iBlockValues].posY = y;
            iBlockValues++;
        }
    }
    std::sort(blockValues.begin(), blockValues.end(), compareNumberAndLocation);

    const int highestNumber = blockValues[iBlockValues-1].number;
    const int highestNumberX = blockValues[iBlockValues-1].posX;
    const int highestNumberY = blockValues[iBlockValues-1].posY;
    const int secondHighestNumberX = blockValues.size() > 1 ? blockValues[iBlockValues-2].posX : 0;
    const int secondHighestNumberY = blockValues.size() > 1 ? blockValues[iBlockValues-2].posY : 0;
    const int thirdHighestNumberX = blockValues.size() > 2 ? blockValues[iBlockValues-3].posX : 0;
    const int thirdHighestNumberY = blockValues.size() > 2 ? blockValues[iBlockValues-3].posY : 0;

    //Highest number in top left
    if(map[0][0] == highestNumber)
    {
        score += Constants::ScoreWeightHighTopLeftN0;
    }
    else if(map[0][1] == highestNumber || map[1][0] == highestNumber)
    {
        score += Constants::ScoreWeightHighTopLeftN1;
    }
    else if(map[1][1] == highestNumber || map[2][0] == highestNumber || map[0][2] == highestNumber)
    {
        score += Constants::ScoreWeightHighTopLeftN2;
    }

    //No number in bottom right
    if(map[xWidth-1][yWidth-1] == 0)
    {
        score += Constants::ScoreWeightNoneBottomRightN0;
    }
    else if(map[xWidth-1][yWidth-2] == 0 || map[xWidth-2][yWidth-1] == 0)
    {
        score += Constants::ScoreWeightNoneBottomRightN1;
    }

    //Highest number created
    score += highestNumber * Constants::ScoreWeightHighestNumber;

    //High numbers close to highest number
    if(secondHighestNumberX > highestNumberX - 2 && secondHighestNumberX < highestNumberX + 2 &&
       secondHighestNumberY > highestNumberY - 2 && secondHighestNumberY < highestNumberY + 2)
    {
        score += Constants::ScoreWeightHighNumbersCloseN0;
    }
    else if(thirdHighestNumberX > highestNumberX - 2 && thirdHighestNumberX < highestNumberX + 2 &&
            thirdHighestNumberY > highestNumberY - 2 && thirdHighestNumberY < highestNumberY + 2)
    {
        score += Constants::ScoreWeightHighNumbersCloseN1;
    }

    //Add to score for number of 0 blocks
    score += Constants::ScoreWeightNumberEmptySpots * numZeroBlocks;

    return score > 0 ? score : 0;
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

void getHighestScore(const QVector<QVector<int>>& map, int& highScore, int depth,
                     QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState, QVector<NumberAndLocation>& blockValues,
                     const int& xWidth, const int& yWidth)
{
    if(depth == 0)
    {
        return;
    }

    int numMerges;
    for(int x = 0; x < xWidth; x++)
    {
        for(int y = 0; y < yWidth; y++)
        {
            if(map[x][y] == 0)
            {
                spawnState = map;
                spawnState[x][y] = 2;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    numMerges = 0;
                    if(mapMove(movedSpawnState, direction, numMerges, xWidth, yWidth))
                    {
                        highScore += gameStateScore(movedSpawnState, blockValues, numMerges, xWidth, yWidth);
                        getHighestScore(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, blockValues, xWidth, yWidth);
                    }
                }

                spawnState[x][y] = 4;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    numMerges = 0;
                    if(mapMove(movedSpawnState, direction, numMerges, xWidth, yWidth))
                    {
                        highScore += gameStateScore(movedSpawnState, blockValues, numMerges, xWidth, yWidth);
                        getHighestScore(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, blockValues, xWidth, yWidth);
                    }
                }
            }
        }
    }
}

Direction AI::getBestDirection(const QVector<QVector<int>>& map)
{
    Direction chosenDirection = Constants::PossibleMoveDirections[0];

    QVector<QVector<int>> spawnStateMem = map;
    QVector<QVector<int>> movedSpawnStateMem = map;
    QVector<QVector<int>> moveMap = map;

    const int xWidth = map.size();
    const int yWidth = map[0].size();

    QVector<NumberAndLocation> blockValuesMem = QVector<NumberAndLocation>(Constants::MaxBlocks, NumberAndLocation());

    int score = 0;
    int mapScore;
    int numMerges;
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        moveMap = map;
        numMerges = 0;
        if(mapMove(moveMap, direction, numMerges, xWidth, yWidth))
        {
            mapScore = gameStateScore(moveMap, blockValuesMem, numMerges, xWidth, yWidth);
            getHighestScore(moveMap, mapScore, Constants::DirectionChoiceDepth, spawnStateMem, movedSpawnStateMem, blockValuesMem, xWidth, yWidth);
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

    const int xWidth = map.size();
    const int yWidth = map[0].size();

    int score = -99999999;
    int mapScore;
    int numMerges;
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        QVector<QVector<int>> moveMap = map;
        numMerges = 0;
        if(mapMove(moveMap, direction, numMerges, xWidth, yWidth))
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

    const int xWidth = map.size();
    const int yWidth = map[0].size();

    int score = -99999999;
    int mapScore;
    int numMerges;
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        QVector<QVector<int>> moveMap = map;
        numMerges = 0;
        if(mapMove(moveMap, direction, numMerges, xWidth, yWidth))
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
