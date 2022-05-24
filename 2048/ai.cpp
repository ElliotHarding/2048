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

int smoothGameStateScore(const QVector<QVector<int>>& map, const int& width, const int& height)
{
    int smoothness = 0;
    for(int x = 1; x < width-1; x++)
    {
        for(int y = 1; y < height-1; y++)
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

int smooth2GameStateScore(const QVector<QVector<int>>& map, const int& width, const int& height)
{
    int smoothness = 0;
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
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

void AI::getHighestScore(int& highScore, int depth)
{
    if(depth == 0)
    {
        return;
    }

    //Game state evaluation vars
    int sumMerges;
#ifdef AI_NO_SUM_SCORES
    int score;
#endif

    //Loop through map, if find an empty spot add a 2 & 4 to that spot (spawnState)
    // then evaluate spawnState (to a set depth of moves)
    for(int x = 0; x < m_width; x++)
    {
        for(int y = 0; y < m_height; y++)
        {
            if(m_mapsAtDepths[depth][x][y] == 0)
            {
                //Add 2 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                m_mapsAtDepths[depth][x][y] = 2;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    m_mapsAtDepths[depth-1] = m_mapsAtDepths[depth];
                    sumMerges = 0;
                    if(mapMove(m_mapsAtDepths[depth-1], direction, sumMerges, m_width, m_height))
                    {
#ifdef AI_NO_SUM_SCORES
    #ifdef AI_NO_SUM_WINNER_1
                        //Strange method of dividing by depth (which decrements) but got to 2048...
                        score = gameStateScore(m_mapsAtDepths[depth-1], sumMerges) * Constants::RatioSpawn2Block / (depth * Constants::DepthMultiplier);
    #else
                        score = gameStateScore(m_mapsAtDepths[depth-1], sumMerges) * Constants::RatioSpawn2Block;
    #endif
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += gameStateScore(m_mapsAtDepths[depth-1], sumMerges) * Constants::RatioSpawn2Block;
#endif
                        getHighestScore(highScore, depth - 1);
                    }
                }

                //Add 4 to empty spot to simulate spawn
                //For each direction, add its score to current score (to a set depth of moves)
                m_mapsAtDepths[depth][x][y] = 4;
                for(const Direction& direction : Constants::PossibleMoveDirections)
                {
                    m_mapsAtDepths[depth-1] = m_mapsAtDepths[depth];
                    sumMerges = 0;
                    if(mapMove(m_mapsAtDepths[depth-1], direction, sumMerges, m_width, m_height))
                    {
#ifdef AI_NO_SUM_SCORES
    #ifdef AI_NO_SUM_WINNER_1
                        //Strange method of dividing by depth (which decrements) & also using spawn2block instead of 4, but got to 2048...
                        score = gameStateScore(m_mapsAtDepths[depth-1], sumMerges) * Constants::RatioSpawn2Block / (depth * Constants::DepthMultiplier);
    #else
                        score = gameStateScore(m_mapsAtDepths[depth-1], sumMerges) * Constants::RatioSpawn4Block;
    #endif
                        if(score > highScore)
                        {
                            highScore = score;
                        }
#else
                        highScore += gameStateScore(m_mapsAtDepths[depth-1], sumMerges) * Constants::RatioSpawn4Block;
#endif
                        getHighestScore(highScore, depth - 1);
                    }
                }

                m_mapsAtDepths[depth][x][y] = 0;
            }
        }
    }
}

Direction AI::getBestDirection(const QVector<QVector<int>>& map)
{
    Direction chosenDirection = Constants::PossibleMoveDirections[0];

    //Size of map, quicker to retrieve from const
    m_width = map.size();
    m_height = map[0].size();

#ifdef AI_SNAKE
    //Create snake grid
    if(m_snakeGrid.size() != m_width || m_snakeGrid[0].size() != m_height)
    {
        m_snakeGrid = QVector<QVector<int>>(m_width, QVector<int>(m_height, 0));

        bool startTop = true;
        int startVal = 1;
        for(int x = 0; x < m_width; x++)
        {
            if(startTop)
            {
                for(int y = 0; y < m_height; y++)
                {
                    m_snakeGrid[x][y] = startVal;
                    startVal*=2;
                }
            }
            else
            {
                for(int y = m_height-1; y > -1; y--)
                {
                    m_snakeGrid[x][y] = startVal;
                    startVal*=2;
                }
            }
            startTop = !startTop;
        }
    }
#endif

    if(m_mapsAtDepths.size() == 0)
    {
        for(int i = 0; i < Constants::DirectionChoiceDepth+1; i++)
        {
            m_mapsAtDepths.push_back(map);
        }
    }

    //Game state evaluation vars
    int score = -9999999;
    int mapScore;
    int sumMerges;

    //For each direction, evaluate its score (to a set depth of moves), choose best direction
    for(const Direction& direction : Constants::PossibleMoveDirections)
    {
        m_mapsAtDepths[Constants::DirectionChoiceDepth] = map;
        sumMerges = 0;
        if(mapMove(m_mapsAtDepths[Constants::DirectionChoiceDepth], direction, sumMerges, m_width, m_height))
        {
            mapScore = gameStateScore(m_mapsAtDepths[Constants::DirectionChoiceDepth], sumMerges);
            getHighestScore(m_mapsAtDepths[Constants::DirectionChoiceDepth], mapScore, Constants::DirectionChoiceDepth);
            if(mapScore > score)
            {
                score = mapScore;
                chosenDirection = direction;
            }
        }
    }

#ifdef AI_CACHE
    m_cacheGameStates.clear();
#endif

    return chosenDirection;
}

int AI::gameStateScore(const QVector<QVector<int>>& map, const int& sumMerges)
{
#ifdef AI_CACHE
    QMap<QVector<QVector<int>>, int>::ConstIterator it = m_cacheGameStates.find(map);
    if(it != m_cacheGameStates.end())
    {
        return it.value();
    }
#endif

    int score = 0;

#ifdef AI_NORMAL

    int numZeroBlocks = 0;
    int highestNumber = 0;
    int smoothness = 0;
    for(int x = 0; x < m_width; x++)
    {
        for(int y = 0; y < m_height; y++)
        {
            const int mapVal = map[x][y];
            if(x < m_width-1)
                smoothness -= abs(mapVal - map[x+1][y]);
            if(y < m_height-1)
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

    const int maxBlocks = m_width * m_height;
    if(numZeroBlocks == maxBlocks)
    {
        numZeroBlocks--;
    }
    else if(numZeroBlocks == 0)
    {
        return 0;
    }

    //Number of merges adds to score
    score += sumMerges * Constants::ScoreWeightSumMerges;

    //Smoothness
    score += (smoothness * Constants::ScoreWeightSmoothness)/(m_width*m_height-numZeroBlocks);

    //Highest number created
    score += highestNumber * Constants::ScoreWeightHighestNumber;

    //Add to score for number of 0 blocks
    score += numZeroBlocks * Constants::ScoreWeightNumberEmptySpots;

#endif

#ifdef AI_MONOICITY

    //Returns map values after --> log(value)/log(2)
    QVector<QVector<double>> logMap = log2Map(map, m_width, m_height);

    int numZeroBlocks = 0;
    double highestNumber = 0;
    double smoothness = 0;
    for(int x = 0; x < m_width; x++)
    {
        for(int y = 0; y < m_height; y++)
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

                if(x < m_width-1 && logMap[x+1][y] != 0)
                    smoothness -= abs(mapVal - logMap[x+1][y]);
                if(y < m_height-1 && logMap[x][y+1] != 0)
                    smoothness -= abs(mapVal - logMap[x][y+1]);
            }
        }
    }

    score += Constants::BaseScore_Monoicity;

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
    const int monoton = monotonicity(logMap, m_width, m_height) * Constants::ScoreWeight_Monoicity;
#endif
    score += monotonicity(logMap, m_width, m_height) * Constants::ScoreWeight_Monoicity;

#ifdef AI_DEBUG
    if(score < 0)
    {
        qDebug() << "gameStateScore_monoicity: score: " << score;
        qDebug() << "Smooth: " << smooth;
        qDebug() << "Monton: " << monoton;
    }
#endif

#endif

#ifdef AI_SNAKE
    for(int x = 0; x < m_width; x++)
    {
        for(int y = 0; y < m_height; y++)
        {
            score += map[x][y] * m_snakeGrid[x][y];
        }
    }
#endif

#ifdef AI_SMOOTH_1
    score += smoothGameStateScore(map, m_width, m_height);
#endif

#ifdef AI_SMOOTH_2
    score += smooth2GameStateScore(map, m_width, m_height);
#endif


#if defined(AI_SMOOTH_1) || defined(AI_SMOOTH_2)
    const int finalScore = score;
#else
    const int finalScore = score > 0 ? score : 0;
#endif

#ifdef AI_CACHE
    m_cacheGameStates.insert(map, finalScore);
#endif

    return finalScore;
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
