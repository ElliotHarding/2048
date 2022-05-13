#include "ai.h"
#include <QDebug>

AI::AI()
{
}

bool withinRangeInclusive(const int& value, const int& min, const int& max)
{
    return value >= min && value <= max;
}

bool mapMove(QVector<QVector<int>>& map, const Vector2& direction)
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
    int number = 0;
    int posX = 0;
    int posY = 0;
};

bool compareNumberAndLocation(const NumberAndLocation &a, const NumberAndLocation &b)
{
    return a.number < b.number;
}

int gameStateScore(const QVector<QVector<int>>& map)
{
    int score = 0;

    QList<NumberAndLocation> blockValues;
    for(int x = 0; x < map.size(); x++)
    {
        for(int y = 0; y < map[0].size(); y++)
        {
            NumberAndLocation numAndLocation;
            numAndLocation.number = map[x][y];
            numAndLocation.posX = x;
            numAndLocation.posY = y;
            blockValues.push_back(numAndLocation);
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

    //Lower score for too many blocks
    score += Constants::ScoreWeightNumberBlocks * (blockValues.size() / (map.size() * map[0].size()));

    return score > 0 ? score : 0;
}

QList<QVector<QVector<int>>> possibleSpawnStates(const QVector<QVector<int>>& map)
{
    QList<QVector<QVector<int>>> possibleSpawnStates;

    for(int x = 0; x < map.size(); x++)
    {
        for(int y = 0; y < map[x].size(); y++)
        {
            if(map[x][y] == 0)
            {
                QVector<QVector<int>> newMap = map;
                newMap[x][y] = 2;
                possibleSpawnStates.push_back(newMap);
            }
        }
    }

    return possibleSpawnStates;
}

void getHighestScore(const QVector<QVector<int>>& map, int& highScore, int depth)
{
    if(depth == 0)
    {
        return;
    }

    QList<QVector<QVector<int>>> spawnStates = possibleSpawnStates(map);
    for(const QVector<QVector<int>>& spawnState : spawnStates)
    {
        for(const Vector2& direction : Constants::PossibleMoveDirections)
        {
            QVector<QVector<int>> movedSpawnState = std::move(spawnState);
            if(mapMove(movedSpawnState, direction))
            {
                int score = gameStateScore(movedSpawnState);
                if(score > highScore)
                {
                    highScore = score;
                }
                getHighestScore(movedSpawnState, highScore, depth - 1);
            }
        }
    }
}

Vector2 AI::getBestDirection(const QVector<QVector<int>>& map)
{
    Vector2 chosenDirection = Constants::PossibleMoveDirections[0];
    int score = 0;
    for(const Vector2& direction : Constants::PossibleMoveDirections)
    {
        QVector<QVector<int>> moveMap = std::move(map);
        if(mapMove(moveMap, direction))
        {
            int mapScore = gameStateScore(moveMap);
            getHighestScore(moveMap, mapScore, Constants::DirectionChoiceDepth);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }
    return chosenDirection;
}
