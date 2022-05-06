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

    reset();

    //Start game loop - runs forever
    m_pUpdateTimer = new QTimer(this);
    connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    m_pUpdateTimer->setTimerType(Qt::PreciseTimer);
    m_pUpdateTimer->start(Constants::GameUpdateFrequency);

    m_pAiTimer = new QTimer(this);
    connect(m_pAiTimer, SIGNAL(timeout()), this, SLOT(onAiThink()));
    m_pAiTimer->setTimerType(Qt::PreciseTimer);
    m_pAiTimer->start(Constants::AiThinkFrequency);
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

void DLG_Home::move(Qt::Key dirKey)
{
    //Get velocity applied by arrow keys
    Vector2 appliedVelocity;
    if(dirKey == Qt::Key_Up)
    {
        appliedVelocity = Vector2(0, -Constants::BlockMovementSpeed);
    }
    else if(dirKey == Qt::Key_Down)
    {
        appliedVelocity = Vector2(0, Constants::BlockMovementSpeed);
    }
    else if(dirKey == Qt::Key_Right)
    {
        appliedVelocity = Vector2(Constants::BlockMovementSpeed, 0);
    }
    else if(dirKey == Qt::Key_Left)
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

        //If board changed after input then can try spawn a new block
        if(m_blocksPositionsBeforeInput != newPositions)
        {
            if(!trySpawnNewBlock())
            {
                //todo - Game over...
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
    int map[Constants::MaxBlocksPerCol][Constants::MaxBlocksPerRow] = {0};
    for(Block* pBlock : m_blocks)
    {
        const int indexX = (pBlock->geometry().x() - Constants::BoardGeometry.x())/Constants::BlockSize;
        const int indexY = (pBlock->geometry().y() - Constants::BoardGeometry.y())/Constants::BlockSize;

        map[indexX][indexY] = pBlock->value();
    }

    /*
    qDebug() << "---------------";
    for(int y = 0; y < Constants::MaxBlocksPerCol; y++)
    {
        QString colStr = "";
        for(int x = 0; x < Constants::MaxBlocksPerRow; x++)
        {
            colStr += QString::number(map[x][y]) + " ";
        }
        qDebug() << colStr;
    }
    qDebug() << "---------------";
    */

    Qt::Key moveDir = Qt::Key_0;

    //Todo make decision
    for(int y = 1; y < Constants::MaxBlocksPerCol-1; y++)
    {
        for(int x = 1; x < Constants::MaxBlocksPerRow-1; x++)
        {
            if(map[x][y] != 0)
            {
                if(map[x][y] == map[x][y+1])
                {
                    moveDir = Qt::Key_Up;
                    break;
                }
                else if(map[x][y] == map[x][y-1])
                {
                    moveDir = Qt::Key_Up;
                    break;
                }
                else if(map[x][y] == map[x+1][y])
                {
                    moveDir = Qt::Key_Right;
                    break;
                }
                else if(map[x][y] == map[x-1][y])
                {
                    moveDir = Qt::Key_Right;
                    break;
                }
            }
        }
        if(moveDir != Qt::Key_0)
            break;
    }

    if(moveDir == Qt::Key_0)
    {
        const int random = QRandomGenerator::global()->generateDouble() * 4;
        moveDir = random == 0 ? Qt::Key_Up :
                                random == 1 ? Qt::Key_Down :
                                              random == 2 ? Qt::Key_Left :
                                                            Qt::Key_Right;
    }
    else
    {
        static int counter = 0;
        qDebug() << "Intentional move " << counter++;
    }

    m_blocksMutex.unlock();
    move(moveDir);
    return;
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
