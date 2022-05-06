#include "dlg_home.h"
#include "ui_dlg_home.h"

#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///DLG_Home
///
DLG_Home::DLG_Home(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::DLG_Home)
{
    ui->setupUi(this);

    reset();

    //Start game loop - runs forever
    m_pUpdateTimer = new QTimer(this);
    connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    m_pUpdateTimer->setTimerType(Qt::PreciseTimer);
    m_pUpdateTimer->start(Constants::GameUpdateFrequency);
}

DLG_Home::~DLG_Home()
{
    delete ui;

    m_blocksMutex.lock();

    m_pUpdateTimer->stop();
    delete m_pUpdateTimer;

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

    m_blocksMutex.unlock();

    m_currentScore = 0;
    updateScores();
}

void DLG_Home::keyPressEvent(QKeyEvent *event)
{
    if(m_bAcceptInput)
    {
        //Get velocity applied by arrow keys
        Vector2 appliedVelocity;
        if(event->key() == Qt::Key_Up)
        {
            appliedVelocity = Vector2(0, -Constants::BlockMovementSpeed);
        }
        else if(event->key() == Qt::Key_Down)
        {
            appliedVelocity = Vector2(0, Constants::BlockMovementSpeed);
        }
        else if(event->key() == Qt::Key_Right)
        {
            appliedVelocity = Vector2(Constants::BlockMovementSpeed, 0);
        }
        else if(event->key() == Qt::Key_Left)
        {
            appliedVelocity = Vector2(-Constants::BlockMovementSpeed, 0);
        }
        else
        {
            return;
        }

        m_blocksMutex.lock();

        //Log block positions before they're changed by applied velocity
        m_blocksPositionsBeforeInput.clear();

        //Apply velocity to all blocks
        for(Block* pBlock : m_blocks)
        {
            m_blocksPositionsBeforeInput.push_back(pBlock->geometry().topLeft());

            if(Constants::BoardGeometry.contains(pBlock->geometry().topLeft() + QPoint(appliedVelocity.x()/Constants::BlockMovementSpeed, appliedVelocity.y()/Constants::BlockMovementSpeed)))
            {
                pBlock->setVelocity(appliedVelocity);
            }
        }

        //Block input (adding extra velocities) until things have moved where they need to go
        m_bAcceptInput = false;

        m_blocksMutex.unlock();
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

    else if(!m_bAcceptInput && !anyMoved)
    {
        QVector<QPoint> newPositions;
        for(Block* pBlock : m_blocks)
        {
            newPositions.push_back(pBlock->geometry().topLeft());
        }

        if(m_blocksPositionsBeforeInput != newPositions && !trySpawnNewBlock())
        {
            //todo - Game over...
        }
        else
        {
            m_currentScore += 2;
            updateScores();

            m_bAcceptInput = true;
        }
    }

    m_blocksMutex.unlock();
}

bool DLG_Home::trySpawnNewBlock()
{
    //Find empty spaces
    QVector<QPoint> emptySpaces;
    for(int x = 0; x < Constants::BlockSize * Constants::MaxBlocksPerRow; x+=Constants::BlockSize)
    {
        for(int y = 0; y < Constants::BlockSize * Constants::MaxBlocksPerCol; y+=Constants::BlockSize)
        {
            bool bLocationTaken = false;
            for(Block* pBlock : m_blocks)
            {
                if(pBlock->geometry().contains(Constants::BoardGeometry.topLeft() + QPoint(x+2,y+2)))
                {
                    bLocationTaken = true;
                    break;
                }
            }

            if(!bLocationTaken)
            {
                emptySpaces.push_back(Constants::BoardGeometry.topLeft() + QPoint(x,y));
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

    const int randomStartOption = QRandomGenerator::global()->generateDouble() * 1;
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
