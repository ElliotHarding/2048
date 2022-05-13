#include "dlg_home.h"
#include "ui_dlg_home.h"

#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>
#include <QThread>
#include <QPropertyAnimation>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///DLG_Home
///
DLG_Home::DLG_Home(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::DLG_Home)
{
    ui->setupUi(this);

    m_pAiTimer = new QTimer(this);
    m_pAiTimer->setTimerType(Qt::PreciseTimer);
    connect(m_pAiTimer, SIGNAL(timeout()), this, SLOT(onAiThink()));    

    m_pFinishAnimationTimer = new QTimer(this);
    m_pFinishAnimationTimer->setTimerType(Qt::PreciseTimer);
    m_pFinishAnimationTimer->setSingleShot(true);
    connect(m_pFinishAnimationTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));

    reset();
}

DLG_Home::~DLG_Home()
{
    delete ui;

    m_blocksMutex.lock();

    m_pAiTimer->stop();
    delete m_pAiTimer;

    for(QVector<Block*> blockCol : m_blocksGrid)
    {
        for(Block* block : blockCol)
        {
            delete block;
        }
        blockCol.clear();
    }
    m_blocksGrid.clear();

    m_blocksMutex.unlock();
}

//Reset game
void DLG_Home::reset()
{
    m_blocksMutex.lock();

    //Remove previous blocks
    // todo : object recycle system to stop creating new memory each time
    for(QVector<Block*> blockCol : m_blocksGrid)
    {
        for(Block* block : blockCol)
        {
            delete block;
        }
        blockCol.clear();
    }
    m_blocksGrid.clear();

    //Create new grid
    m_blocksGrid = QVector<QVector<Block*>>(Constants::MaxBlocksPerCol, QVector<Block*>(Constants::MaxBlocksPerRow, nullptr));

    //Initial game board contains one block
    m_blocksGrid[0][0] = new Block(this, 2, Constants::BoardGeometry.topLeft());

    m_bAcceptInput = true;
    m_bGameOver = false;

    //Start game loop - runs forever
    m_pAiTimer->start(Constants::AiThinkFrequency);

    m_blocksMutex.unlock();

    m_currentScore = 0;
    updateScores();
}

void DLG_Home::keyPressEvent(QKeyEvent *event)
{
    if(m_bAcceptInput)
    {
        if(event->key() == Qt::Key_Up)
        {
            move(Vector2(0, -1));
        }
        else if(event->key() == Qt::Key_Down)
        {
            move(Vector2(0, 1));
        }
        else if(event->key() == Qt::Key_Right)
        {
            move(Vector2(1, 0));
        }
        else if(event->key() == Qt::Key_Left)
        {
            move(Vector2(-1, 0));
        }
    }
}

bool inRange(const int& value, const int& min, const int& max)
{
    return value >= min && value <= max;
}

bool inRange(const int& value, const int& min, const int& max, const int& inc)
{
    return inRange(value, min, max) && inRange(value + inc, min, max);
}

void DLG_Home::move(Vector2 direction)
{
    m_blocksMutex.lock();

    //Block input until things have moved where they need to go
    m_bAcceptInput = false;

    const int xStart =  direction.x() > 0 ? Constants::MaxBlocksPerRow-1 : 0;
    const int xInc =    direction.x() > 0 ? -1 : 1;
    const int yStart =  direction.y() > 0 ? Constants::MaxBlocksPerCol-1 : 0;
    const int yInc =    direction.y() > 0 ? -1 : 1;
    for(int moveCount = 0; moveCount < m_blocksGrid.size(); moveCount++)
    {
        bool moved = false;
        for(int x = xStart; inRange(x, 0, Constants::MaxBlocksPerRow-1); x+=xInc)
        {
            for(int y = yStart; inRange(y, 0, Constants::MaxBlocksPerCol-1); y+=yInc)
            {
                if(m_blocksGrid[x][y] != 0 && inRange(x, 0, Constants::MaxBlocksPerRow-1, direction.x()) && inRange(y, 0, Constants::MaxBlocksPerCol-1, direction.y()))
                {
                    if(m_blocksGrid[x+direction.x()][y+direction.y()] == 0)
                    {
                        m_blocksGrid[x+direction.x()][y+direction.y()] = m_blocksGrid[x][y];
                        m_blocksGrid[x][y] = nullptr;
                        moved = true;
                    }
                    else if(m_blocksGrid[x+direction.x()][y+direction.y()]->value() == m_blocksGrid[x][y]->value())
                    {
                        m_blocksGrid[x][y]->setToMerge(x+direction.x(), y+direction.y(), m_blocksGrid[x+direction.x()][y+direction.y()]);
                        m_blocksGrid[x][y] = nullptr;
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

    for(int x = 0; x < Constants::MaxBlocksPerRow; x++)
    {
        for(int y = 0; y < Constants::MaxBlocksPerCol; y++)
        {
            if(m_blocksGrid[x][y] != nullptr)
            {
                m_blocksGrid[x][y]->startMoveAnimation(x, y);
            }
        }
    }

    m_pFinishAnimationTimer->start(Constants::MoveAnimationMs);

    m_blocksMutex.unlock();
}

void DLG_Home::onUpdate()
{
    m_blocksMutex.lock();

    if(!trySpawnNewBlock())
    {
        m_bGameOver = true;
        m_pAiTimer->stop();
    }
    else
    {
        m_currentScore += 2;
        updateScores();
    }

    m_bAcceptInput = true;

    m_blocksMutex.unlock();
}

void DLG_Home::onAiThink()
{
    m_blocksMutex.lock();
    if(!m_bAcceptInput)
    {
        m_blocksMutex.unlock();
        return;
    }

    //Todo generate map
    QVector<QVector<int>> map(Constants::MaxBlocksPerCol, QVector<int>(Constants::MaxBlocksPerRow, 0));
    for(int x = 0; x < Constants::MaxBlocksPerRow; x++)
    {
        for(int y = 0; y < Constants::MaxBlocksPerCol; y++)
        {
            if(m_blocksGrid[x][y] != nullptr)
            {
                map[x][y] = m_blocksGrid[x][y]->value();
            }
        }
    }

    /* Test map
    map[0][0] = 2;
    map[1][0] = 0;
    map[2][0] = 0;
    map[3][0] = 0;
    map[0][1] = 0;
    map[1][1] = 2;
    map[2][1] = 0;
    map[3][1] = 0;
    map[0][2] = 0;
    map[1][2] = 4;
    map[2][2] = 0;
    map[3][2] = 0;
    map[0][3] = 0;
    map[1][3] = 2;
    map[2][3] = 0;
    map[3][3] = 0;*/

    //make const... todo
    const Vector2 bestDirection = m_ai.getBestDirection(map);

    m_blocksMutex.unlock();
    move(bestDirection);
}

bool DLG_Home::trySpawnNewBlock()
{
    //Find empty spaces
    QVector<QPoint> emptySpaces;
    for(int x = 0; x < Constants::MaxBlocksPerRow; x++)
    {
        for(int y = 0; y < Constants::MaxBlocksPerCol; y++)
        {
            if(m_blocksGrid[x][y] == nullptr)
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

    const int randomStartOption = QRandomGenerator::global()->generateDouble() * 100;
    const int startValue = randomStartOption < Constants::PercentageSpawn2block ? 2 : 4;

    m_blocksGrid[spawnPos.x()][spawnPos.y()] = new Block(this, startValue, Constants::BoardGeometry.topLeft() + QPoint(spawnPos.x() * Constants::BlockSize, spawnPos.y() * Constants::BlockSize));

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
