#include "ai.h"

AI::AI()
{

}

bool withinRangeInclusive(const int& value, const int& min, const int& max)
{
    return value >= min && value <= max;
}

void mapMove(QVector<QVector<int>>& map, const Vector2& direction)
{
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
            if(!moved)
            {
                break;
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
            if(!moved)
            {
                break;
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
            if(!moved)
            {
                break;
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
            if(!moved)
            {
                break;
            }
        }
    }
}

namespace ScoreWeights
{
const int ScoreWeightHighTopLeftN0 = 100; //Reward % for having highest number in top left slot
const int ScoreWeightHighTopLeftN1 = 50;
const int ScoreWeightHighTopLeftN2 = 20;

const int ScoreWeightHighNumbersCloseN0 = 70; //Reward % for having high value blocks next to eachother
const int ScoreWeightHighNumbersCloseN1 = 40;

const int ScoreWeightHighestNumber = 1; //Reward % for having the highest number
const int ScoreWeightNumberBlocks = -15;
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

int gameStateScore(QVector<QVector<int>> map)
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
        score += ScoreWeights::ScoreWeightHighTopLeftN0;
    }
    else if(map[0][1] == highestNumber || map[1][0] == highestNumber)
    {
        score += ScoreWeights::ScoreWeightHighTopLeftN1;
    }
    else if(map[1][1] == highestNumber || map[2][0] == highestNumber || map[0][2] == highestNumber)
    {
        score += ScoreWeights::ScoreWeightHighTopLeftN2;
    }

    //Highest number created
    score += highestNumber * ScoreWeights::ScoreWeightHighestNumber;

    //High numbers close to highest number
    if(secondHighestNumberX > highestNumberX - 2 && secondHighestNumberX < highestNumberX + 2 &&
       secondHighestNumberY > highestNumberY - 2 && secondHighestNumberY < highestNumberY + 2)
    {
        score += ScoreWeights::ScoreWeightHighNumbersCloseN0;
    }
    else if(thirdHighestNumberX > highestNumberX - 2 && thirdHighestNumberX < highestNumberX + 2 &&
            thirdHighestNumberY > highestNumberY - 2 && thirdHighestNumberY < highestNumberY + 2)
    {
        score += ScoreWeights::ScoreWeightHighNumbersCloseN1;
    }

    //Lower score for too many blocks
    score += ScoreWeights::ScoreWeightNumberBlocks * (blockValues.size() / (map.size() * map[0].size()));

    return score > 0 ? score : 0;
}

namespace MovementOptions
{
const QList<Vector2> PossibleMoveDirections = {Vector2(0, 1), Vector2(0, -1), Vector2(1, 0), Vector2(-1, 0)};
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
    for(QVector<QVector<int>> spawnState : spawnStates)
    {
        for(const Vector2& direction : MovementOptions::PossibleMoveDirections)
        {
            QVector<QVector<int>> movedSpawnState = spawnState;
            mapMove(movedSpawnState, direction);

            const bool spawnStateMoved = spawnState != movedSpawnState;
            if(spawnStateMoved)
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
    Vector2 chosenDirection = MovementOptions::PossibleMoveDirections[0];
    int score = 0;
    for(const Vector2& direction : MovementOptions::PossibleMoveDirections)
    {
        QVector<QVector<int>> moveMap = map;
        mapMove(moveMap, direction);

        const bool mapMoved = map != moveMap;
        if(mapMoved)
        {
            int mapScore = gameStateScore(moveMap);
            getHighestScore(moveMap, mapScore, 2);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }
    return chosenDirection;
}
