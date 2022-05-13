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
    for(Block* pBlock : m_blocks)
    {
        const int indexX = (pBlock->geometry().x() - Constants::BoardGeometry.x())/Constants::BlockSize;
        const int indexY = (pBlock->geometry().y() - Constants::BoardGeometry.y())/Constants::BlockSize;

        map[indexX][indexY] = pBlock->value();
    }

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
    map[3][3] = 0;

    //make const... todo
    Vector2 bestDirection = m_ai.getBestDirection(map);
    bestDirection = Vector2(0,0);

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
