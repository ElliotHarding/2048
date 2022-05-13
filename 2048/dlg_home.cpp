#include "dlg_home.h"
#include "ui_dlg_home.h"

#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>
#include <QThread>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///DLG_Home
///
DLG_Home::DLG_Home(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::DLG_Home)
{
    ui->setupUi(this);

    m_pUpdateTimer = new QTimer(this);
    connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    m_pUpdateTimer->setTimerType(Qt::PreciseTimer);

    m_pAiTimer = new QTimer(this);
    connect(m_pAiTimer, SIGNAL(timeout()), this, SLOT(onAiThink()));
    m_pAiTimer->setTimerType(Qt::PreciseTimer);

    reset();
}

DLG_Home::~DLG_Home()
{
    delete ui;

    m_blocksMutex.lock();

    m_pUpdateTimer->stop();
    delete m_pUpdateTimer;

    m_pAiTimer->stop();
    delete m_pAiTimer;

    for(Block* pBlock : m_blocks)
    {
        delete pBlock;
    }
    m_blocks.clear();
    m_blocksMutex.unlock();
}

//Reset game
void DLG_Home::reset()
{
    m_blocksMutex.lock();

    //Remove previous blocks
    // todo : object recycle system to stop creating new memory each time
    for(Block* pBlock : m_blocks)
    {
        delete pBlock;
    }
    m_blocks.clear();

    //Initial game board contains one block
    m_blocks.push_back(new Block(this, 2, Constants::BoardGeometry.topLeft()));

    m_bAcceptInput = true;
    m_bGameOver = false;

    //Start game loop - runs forever
    m_pUpdateTimer->start(Constants::GameUpdateFrequency);
    m_pAiTimer->start(Constants::AiThinkFrequency);

    m_blocksMutex.unlock();

    m_currentScore = 0;
    updateScores();
}

void DLG_Home::keyPressEvent(QKeyEvent *event)
{
    if(m_bAcceptInput)
    {
        move((Qt::Key)event->key());
    }
}

void DLG_Home::applyVelocity(const Vector2& vel)
{
    m_blocksMutex.lock();

    //Log block positions before they're changed by applied velocity
    m_blocksPositionsBeforeInput.clear();

    //Apply velocity to all blocks
    for(Block* pBlock : m_blocks)
    {
        m_blocksPositionsBeforeInput.push_back(pBlock->geometry().topLeft());

        if(Constants::BoardGeometry.contains(pBlock->geometry().topLeft() + QPoint(vel.x()/Constants::BlockMovementSpeed, vel.y()/Constants::BlockMovementSpeed)))
        {
            pBlock->setVelocity(vel);
        }
    }

    //Block input (adding extra velocities) until things have moved where they need to go
    m_bAcceptInput = false;

    m_blocksMutex.unlock();
}

void DLG_Home::move(Qt::Key dirKey)
{
    //Get velocity applied by arrow keys
    if(dirKey == Qt::Key_Up)
    {
        applyVelocity(Vector2(0, -Constants::BlockMovementSpeed));
    }
    else if(dirKey == Qt::Key_Down)
    {
        applyVelocity(Vector2(0, Constants::BlockMovementSpeed));
    }
    else if(dirKey == Qt::Key_Right)
    {
        applyVelocity(Vector2(Constants::BlockMovementSpeed, 0));
    }
    else if(dirKey == Qt::Key_Left)
    {
        applyVelocity(Vector2(-Constants::BlockMovementSpeed, 0));
    }    
}

void DLG_Home::onUpdate()
{
    m_blocksMutex.lock();

    //Update positions, check if any moved
    bool anyMoved = false;
    for(Block* pBlock : m_blocks)
    {
        const bool moved = pBlock->updatePosition();
        anyMoved = moved | anyMoved;        
    }

    if(anyMoved)
    {
        //If some blocks moved, check they're in correct bounds
        for(Block* pBlock : m_blocks)
        {
            if(pBlock->checkBoundaries(Constants::BoardGeometry, m_blocks))
            {
                break;
            }
        }
        update();
    }

    else if(!m_bAcceptInput)
    {
        QVector<QPoint> newPositions;
        for(Block* pBlock : m_blocks)
        {
            newPositions.push_back(pBlock->geometry().topLeft());
        }

        if(m_blocks.size() == Constants::MaxBlocks)
        {
            m_bGameOver = true;
            m_pAiTimer->stop();
            m_pUpdateTimer->stop();
        }

        //If board changed after input then can try spawn a new block
        if(m_blocksPositionsBeforeInput != newPositions)
        {
            if(!trySpawnNewBlock())
            {
                qDebug() << "DLG_Home::onUpdate : Failed to spawn new block, but game not over";
            }
            else
            {
                m_currentScore += 2;
                updateScores();
            }
        }

        m_bAcceptInput = true;
    }

    m_blocksMutex.unlock();
}

bool withinRangeInclusive(const int& value, const int& min, const int& max)
{
    return value >= min && value <= max;
}

void mapMove(QVector<QVector<int>>& map, const Vector2& direction)
{
    if(direction.x() > 0)
    {
        for(int moveCount = 0; moveCount < Constants::MaxBlocksPerRow; moveCount++)
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
        for(int moveCount = 0; moveCount < Constants::MaxBlocksPerRow; moveCount++)
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
        for(int moveCount = 0; moveCount < Constants::MaxBlocksPerCol; moveCount++)
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
        for(int moveCount = 0; moveCount < Constants::MaxBlocksPerCol; moveCount++)
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

    /*
    qDebug() << "---------------";
    for(int y = 0; y < map[0].size(); y++)
    {
        QString colStr = "";
        for(int x = 0; x < map.size(); x++)
        {
            colStr += QString::number(map[x][y]) + " ";
        }
        qDebug() << colStr;
    }
    qDebug() << "---------------";*/
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
    for(int y = 0; y < Constants::MaxBlocksPerCol; y++)
    {
        for(int x = 0; x < Constants::MaxBlocksPerRow; x++)
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
    score += ScoreWeights::ScoreWeightNumberBlocks * (blockValues.size() / (Constants::MaxBlocksPerCol * Constants::MaxBlocksPerRow));

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

/*
Vector2 getBestDirection(const QVector<QVector<int>>& map)
{
    Vector2 chosenDirection = MovementOptions::PossibleMoveDirections[0];
    int score = 0;
    for(const Vector2& direction : MovementOptions::PossibleMoveDirections)
    {
        QVector<QVector<int>> moveMap = map;
        mapMove(moveMap, direction);
        int mapScore = map != moveMap ? gameStateScore(moveMap) : 0;
        if(mapScore > score)
        {
            score = mapScore;
            chosenDirection = direction;
        }
    }
    return chosenDirection;
}
*/

void getHighestScore(QVector<QVector<int>> map, int& highScore, int depth)
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

Vector2 getBestDirection(const QVector<QVector<int>>& map)
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

void DLG_Home::onAiThink()
{
    m_blocksMutex.lock();
    if(!m_bAcceptInput || m_bGameOver)
    {
        m_blocksMutex.unlock();
        return;
    }

    //Todo generate map
    QVector<QVector<int>> map(Constants::MaxBlocksPerCol, QVector<int>(Constants::MaxBlocksPerRow, 0));
    for(Block* pBlock : m_blocks)
    {
        const int indexX = (pBlock->geometry().x() - Constants::BoardGeometry.x())/Constants::BlockSize;
        const int indexY = (pBlock->geometry().y() - Constants::BoardGeometry.y())/Constants::BlockSize;

        map[indexX][indexY] = pBlock->value();
    }

    const Vector2 bestDirection = getBestDirection(map);

    m_blocksMutex.unlock();
    applyVelocity(bestDirection);
}

bool DLG_Home::trySpawnNewBlock()
{
    //Find empty spaces
    QVector<QPoint> emptySpaces;
    for(int x = Constants::BoardGeometry.left(); x < Constants::BoardGeometry.right(); x+=Constants::BlockSize)
    {
        for(int y = Constants::BoardGeometry.top(); y < Constants::BoardGeometry.bottom(); y+=Constants::BlockSize)
        {
            bool bLocationTaken = false;
            for(Block* pBlock : m_blocks)
            {
                if(pBlock->geometry().contains(QPoint(x+2,y+2)))
                {
                    bLocationTaken = true;
                    break;
                }
            }

            if(!bLocationTaken)
            {
                emptySpaces.push_back(QPoint(x,y));
            }
        }
    }

    //If cant find any empty spaces
    if(emptySpaces.empty())
    {
        return false;
    }

    const int randomPosition = QRandomGenerator::global()->generateDouble() * emptySpaces.size() - 1;
    const QPoint spawnPos = emptySpaces[randomPosition];

    const int randomStartOption = QRandomGenerator::global()->generateDouble() * 2;
    const int startValue = randomStartOption == 0 ? 2 : 4;

    m_blocks.push_back(new Block(this, startValue, spawnPos));

    return true;
}

void DLG_Home::updateScores()
{
    ui->lblScoreValue->setText(QString::number(m_currentScore));
    if(m_currentScore > m_highScore)
    {
        m_highScore = m_currentScore;
        ui->lblHighScoreValue->setText(QString::number(m_highScore));
    }
}

void DLG_Home::on_btn_restart_clicked()
{
    reset();
}
