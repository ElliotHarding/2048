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

bool withinRangeInclusive(const int& value, const int& min, const int& max)
{
    return value >= min && value <= max;
}

/*      More compressed map move - less efficient
bool mapMove(QVector<QVector<int>>& map, const Vector2& direction)
{
    const int xStart =  direction.x() > 0 ? map.size()-1 : 0;
    const int xInc =    direction.x() > 0 ? -1 : 1;
    const int yStart =  direction.y() > 0 ? map[0].size()-1 : 0;
    const int yInc =    direction.y() > 0 ? -1 : 1;
    bool anyMoved = false;
    for(int moveCount = 0; moveCount < map.size(); moveCount++)
    {
        bool moved = false;
        for(int x = xStart; xInc == -1 ? (x > -1) : (x < map.size()); x+=xInc)
        {
            for(int y = yStart; yInc == -1 ? (y > -1) : (y < map[x].size()); y+=yInc)
            {
                if(map[x][y] != 0 && inRange(x, 0, map.size(), direction.x()) && inRange(y, 0, map[x].size(), direction.y()))
                {
                    if(map[x+direction.x()][y+direction.y()] == 0)
                    {
                        map[x+direction.x()][y+direction.y()] = map[x][y];
                        map[x][y] = 0;
                        moved = true;
                    }
                    else if(map[x+direction.x()][y+direction.y()] == map[x][y])
                    {
                        map[x+direction.x()][y+direction.y()] *= 2;
                        map[x][y] = 0;
                        moved = true;
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

    return anyMoved;
}*/

bool mapMove(QVector<QVector<int>>& map, const Vector2& direction, int& numMerges)
{
    bool anyMoved = false;
    if(direction.x() > 0)
    {
        for(int moveCount = 0; moveCount < map.size(); moveCount++)
        {
            bool moved = false;
            for(int x = map.size()-1; x > -1; x--)
            {
                for(int y = 0; y < map[0].size(); y++)
                {
                    if(map[x][y] != 0 && x < map.size()-1)
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
    else if(direction.x() < 0)
    {
        for(int moveCount = 0; moveCount < map.size(); moveCount++)
        {
            bool moved = false;
            for(int x = 0; x < map.size(); x++)
            {
                for(int y = 0; y < map[0].size(); y++)
                {
                    if(map[x][y] != 0 && x > 0)
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
    else if(direction.y() > 0)
    {
        for(int moveCount = 0; moveCount < map[0].size(); moveCount++)
        {
            bool moved = false;
            for(int x = 0; x < map.size(); x++)
            {
                for(int y = map[0].size()-1; y > -1; y--)
                {
                    if(map[x][y] != 0 && y < map[0].size()-1)
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
    else if(direction.y() < 0)
    {
        for(int moveCount = 0; moveCount < map[0].size(); moveCount++)
        {
            bool moved = false;
            for(int x = 0; x < map.size(); x++)
            {
                for(int y = 0; y < map[0].size(); y++)
                {
                    if(map[x][y] != 0 && y > 0)
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

int gameStateScore(const QVector<QVector<int>>& map, QVector<NumberAndLocation>& blockValues, const int& numMerges)
{
    //Number of merges adds to score
    int score = numMerges * Constants::ScoreWeightNumMerges;

    int numZeroBlocks = 0;
    int iBlockValues = 0;
    for(int x = 0; x < map.size(); x++)
    {
        for(int y = 0; y < map[0].size(); y++)
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

    const int highestNumber = blockValues[blockValues.size()-1].number;
    const int highestNumberX = blockValues[blockValues.size()-1].posX;
    const int highestNumberY = blockValues[blockValues.size()-1].posY;
    const int secondHighestNumberX = blockValues.size() > 1 ? blockValues[blockValues.size()-2].posX : 0;
    const int secondHighestNumberY = blockValues.size() > 1 ? blockValues[blockValues.size()-2].posY : 0;
    const int thirdHighestNumberX = blockValues.size() > 2 ? blockValues[blockValues.size()-3].posX : 0;
    const int thirdHighestNumberY = blockValues.size() > 2 ? blockValues[blockValues.size()-3].posY : 0;

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
    if(map[map.size()-1][map[0].size()-1] != 0)
    {
        score -= Constants::ScoreWeightHighTopLeftN0;
    }
    else if(map[map.size()-1][map[0].size()-2] != 0 || map[map.size()-2][map[0].size()-1] != 0)
    {
        score -= Constants::ScoreWeightHighTopLeftN1;
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
    score += Constants::ScoreWeightNumberBlocks * numZeroBlocks;

    return score > 0 ? score : 0;
}

void getHighestScore(const QVector<QVector<int>>& map, int& highScore, int depth,
                     QVector<QVector<int>>& spawnState, QVector<QVector<int>>& movedSpawnState, QVector<NumberAndLocation>& blockValues)
{
    if(depth == 0)
    {
        return;
    }

    int numMerges;
    for(int x = 0; x < map.size(); x++)
    {
        for(int y = 0; y < map[x].size(); y++)
        {
            if(map[x][y] == 0)
            {
                spawnState = map;
                spawnState[x][y] = 2;

                for(const Vector2& direction : Constants::PossibleMoveDirections)
                {
                    movedSpawnState = spawnState;
                    numMerges = 0;
                    if(mapMove(movedSpawnState, direction, numMerges))
                    {
                        highScore += gameStateScore(movedSpawnState, blockValues, numMerges);
                        getHighestScore(movedSpawnState, highScore, depth - 1, spawnState, movedSpawnState, blockValues);
                    }
                }
            }
        }
    }
}

Vector2 AI::getBestDirection(const QVector<QVector<int>>& map)
{
    Vector2 chosenDirection = Constants::PossibleMoveDirections[0];

    QVector<QVector<int>> spawnStateMem = map;
    QVector<QVector<int>> movedSpawnStateMem = map;
    QVector<QVector<int>> moveMap = map;

    NumberAndLocation numAndLocation;
    QVector<NumberAndLocation> blockValuesMem = QVector<NumberAndLocation>(Constants::MaxBlocks, numAndLocation);

    int score = 0;
    int mapScore;
    int numMerges;
    for(const Vector2& direction : Constants::PossibleMoveDirections)
    {
        moveMap = map;
        numMerges = 0;
        if(mapMove(moveMap, direction, numMerges))
        {
            mapScore = gameStateScore(moveMap, blockValuesMem, numMerges);
            getHighestScore(moveMap, mapScore, Constants::DirectionChoiceDepth, spawnStateMem, movedSpawnStateMem, blockValuesMem);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }

    return chosenDirection;
}
