#include "dlg_home.h"
#include "ui_dlg_home.h"

#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>

/* Future plans:

- Block object recycle system (stop creating block memory for each spawned)
- AI to play the game
*/

namespace Constants
{
///Speed & movement
const int GameUpdateFrequency = 1;
const int BlockMovementSpeed = 3;

///Visual block settings
const int BlockPopTimeMs = 100; //Block pops bigger when merging or spawning
const int BlockSize = 100;
const int BlockDrawMargin = 5;
const int BlockRectRadius = 10;
const QRect BlockDrawRect(BlockDrawMargin, BlockDrawMargin, Constants::BlockSize-BlockDrawMargin*2, Constants::BlockSize-BlockDrawMargin*2);
const QRect BlockDrawRectPopping(0, 0, Constants::BlockSize, Constants::BlockSize);

///Visual block settings (text and colors)
const QFont BlockTextFont = QFont("Helvetica [Cronyx]", 10, QFont::Normal);
const QFontMetrics BlockTextFontMetrics(BlockTextFont);
const QColor BlockTextColor = Qt::black;
const QMap<int, QColor> BlockColors = {
    {2, QColor(238, 228, 218)},
    {4, QColor(237, 224, 200)},
    {8, QColor(242, 177, 121)},
    {16, QColor(245, 149, 99)},
    {32, QColor(246, 124, 95)},
    {64, QColor(246, 94, 59)},
    {128, QColor(237, 207, 114)},
    {256, QColor(237, 204, 97)},
    {512, QColor(237, 200, 80)},
    {1024, QColor(237, 197, 63)},
    {2048, QColor(237, 194, 46)},
};

///Board settings
const int MaxBlocksPerRow = 4;
const int MaxBlocksPerCol = 4;
const QRect BoardGeometry = QRect(0, BlockSize, BlockSize * MaxBlocksPerRow + 1, BlockSize * MaxBlocksPerCol + 1);

}

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
        if(event->key() == Qt::Key_Up)
        {
            m_blocksMutex.lock();

            m_blockPositionsBeforeInput.clear();

            for(Block* pBlock : m_blocks)
            {
                m_blockPositionsBeforeInput.push_back(pBlock->geometry().topLeft());
                if(pBlock->geometry().top() > Constants::BoardGeometry.top() - 1)
                {
                    pBlock->setVelocity(Vector2(0, -Constants::BlockMovementSpeed));
                }
            }
            m_bAcceptInput = false;
            m_blocksMutex.unlock();
        }
        else if(event->key() == Qt::Key_Down)
        {
            m_blocksMutex.lock();

            m_blockPositionsBeforeInput.clear();

            for(Block* pBlock : m_blocks)
            {
                m_blockPositionsBeforeInput.push_back(pBlock->geometry().topLeft());
                if(pBlock->geometry().bottom() < Constants::BoardGeometry.bottom() + 1)
                {
                    pBlock->setVelocity(Vector2(0, Constants::BlockMovementSpeed));
                }
            }
            m_bAcceptInput = false;
            m_blocksMutex.unlock();
        }
        else if(event->key() == Qt::Key_Right)
        {
            m_blocksMutex.lock();

            m_blockPositionsBeforeInput.clear();

            for(Block* pBlock : m_blocks)
            {
                m_blockPositionsBeforeInput.push_back(pBlock->geometry().topLeft());
                if(pBlock->geometry().right() < Constants::BoardGeometry.right() + 1)
                {
                    pBlock->setVelocity(Vector2(Constants::BlockMovementSpeed, 0));
                }
            }
            m_bAcceptInput = false;
            m_blocksMutex.unlock();
        }
        else if(event->key() == Qt::Key_Left)
        {
            m_blocksMutex.lock();

            m_blockPositionsBeforeInput.clear();

            for(Block* pBlock : m_blocks)
            {
                m_blockPositionsBeforeInput.push_back(pBlock->geometry().topLeft());
                if(pBlock->geometry().left() > Constants::BoardGeometry.left() - 1)
                {
                    pBlock->setVelocity(Vector2(-Constants::BlockMovementSpeed, 0));
                }
            }
            m_bAcceptInput = false;
            m_blocksMutex.unlock();
        }
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

        if(m_blockPositionsBeforeInput != newPositions && !trySpawnNewBlock())
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Block
///
Block::Block(QWidget* parent, const int& value, const QPoint& position) : QWidget(parent)
{
    //Block pops bigger when merging or spawning (for certain time)
    m_pPoppingTimer = new QTimer(this);
    connect(m_pPoppingTimer, SIGNAL(timeout()), this, SLOT(onEndPopping()));

    //Set inital values
    // - will be moved to seperate function when object recycling is done
    setValue(value);
    setPosition(position);
    show();
}

Block::~Block()
{
    m_pPoppingTimer->stop();
    delete m_pPoppingTimer;
}

void Block::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //Paint block rectangle (fill entire geometry if m_bIsPopping)
    // Block pops bigger when merging or spawning
    QPainterPath path;
    path.addRoundedRect(m_bIsPopping ? Constants::BlockDrawRectPopping : Constants::BlockDrawRect, Constants::BlockRectRadius, Constants::BlockRectRadius);
    painter.fillPath(path, m_col);

    //Prep value text drawing
    painter.setPen(Constants::BlockTextColor);
    painter.setFont(Constants::BlockTextFont);
    const float textWidth = Constants::BlockTextFontMetrics.horizontalAdvance(QString::number(m_value));

    //Draw value text
    painter.drawText(QPoint(Constants::BlockSize/2 - textWidth/2, Constants::BlockSize/2 + Constants::BlockTextFontMetrics.height()/4), QString::number(m_value));
}

void Block::onEndPopping()
{
    m_bIsPopping = false;
    m_pPoppingTimer->stop();
    update();
}

int Block::value() const
{
    return m_value;
}

void Block::setValue(const int &value)
{
    m_value = value;
    m_col = Constants::BlockColors[m_value];

    //Block pops bigger when merging or spawning
    //Set timer for ammount of time block is drawn as "popped"
    m_bIsPopping = true;
    m_pPoppingTimer->start(Constants::BlockPopTimeMs);
    update();
}

void Block::setPosition(const QPoint& position)
{
    setGeometry(position.x(), position.y(), Constants::BlockSize, Constants::BlockSize);
}

//Returns true if the position changed
bool Block::updatePosition()
{
    if(m_velocity.x() != 0 || m_velocity.y() != 0)
    {
        setGeometry(geometry().translated(m_velocity.x(), m_velocity.y()));
        return true;
    }
    return false;
}

void Block::setVelocity(Vector2 vel)
{
    m_velocity = vel;
}

Vector2 Block::velocity()
{
    return m_velocity;
}

int roundToMultiple(const int& n, const int& multiple)
{
    return ((n + multiple/2)/multiple) * multiple;
}

//Enforces hitboxes & checks for & performs block merge
bool Block::checkBoundaries(QRect bounds, QVector<Block*>& blocks)
{
    //If the block has a velocity:
    // - Check the block is in bounds (only considoring bounds in velocity direction)
    //   - If not in bounds push it back into bounds and stop its velocity
    // - Otherwise check if block collides (intersects) with any other blocks
    //   - If collides with same m_value block: prepare a merge (by settings m_pDeletingBlock) which will delete this block
    //   - If collides with othr m_value block; stop velocity and set flush against other block.

    if(m_velocity.x() > 0)
    {
        if(geometry().right() > bounds.right())
        {
            m_velocity = Vector2(0,0);
            setPosition(QPoint(bounds.right() - Constants::BlockSize, geometry().y()));
        }
        else
        {
            for(Block* pBlock : blocks)
            {
                const QRect pBlockRect = pBlock->geometry();
                if(geometry().intersects(pBlockRect) && this != pBlock)
                {
                    if(m_value == pBlock->value())
                    {
                        //Set merge block
                        if(m_pDeletingBlock == nullptr)
                        {
                            m_pDeletingBlock = pBlock;
                        }
                        //If found another block other than one to merge to
                        else if(m_pDeletingBlock != pBlock)
                        {
                            //Set velocity to 0 which triggers merge to m_pDeletingBlock
                            m_velocity = Vector2(0,0);
                        }
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(roundToMultiple(pBlockRect.left() - Constants::BlockSize, Constants::BlockSize), geometry().y()));
                    }
                    break;
                }
            }
        }
    }
    else if(m_velocity.x() < 0)
    {
        if(geometry().left() < bounds.left())
        {
            m_velocity = Vector2(0,0);
            setPosition(QPoint(bounds.left(), geometry().y()));
        }
        else
        {
            for(Block* pBlock : blocks)
            {
                const QRect pBlockRect = pBlock->geometry();
                if(geometry().intersects(pBlockRect) && this != pBlock)
                {
                    if(m_value == pBlock->value())
                    {
                        //Set merge block
                        if(m_pDeletingBlock == nullptr)
                        {
                            m_pDeletingBlock = pBlock;
                        }
                        //If found another block other than one to merge to
                        else if(m_pDeletingBlock != pBlock)
                        {
                            //Set velocity to 0 which triggers merge to m_pDeletingBlock
                            m_velocity = Vector2(0,0);
                        }
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(roundToMultiple(pBlockRect.right(), Constants::BlockSize), geometry().y()));
                    }
                    break;
                }
            }
        }
    }

    if(m_velocity.y() > 0)
    {
        if(geometry().bottom() > bounds.bottom())
        {
            m_velocity = Vector2(0,0);
            setPosition(QPoint(geometry().x(), bounds.bottom() - Constants::BlockSize));
        }
        else
        {
            for(Block* pBlock : blocks)
            {
                const QRect pBlockRect = pBlock->geometry();
                if(geometry().intersects(pBlockRect) && this != pBlock)
                {
                    if(m_value == pBlock->value())
                    {
                        //Set merge block
                        if(m_pDeletingBlock == nullptr)
                        {
                            m_pDeletingBlock = pBlock;
                        }
                        //If found another block other than one to merge to
                        else if(m_pDeletingBlock != pBlock)
                        {
                            //Set velocity to 0 which triggers merge to m_pDeletingBlock
                            m_velocity = Vector2(0,0);
                        }
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(geometry().x(), roundToMultiple(pBlockRect.y() - Constants::BlockSize, Constants::BlockSize)));
                    }
                    break;
                }
            }
        }
    }
    else if(m_velocity.y() < 0)
    {
        if(geometry().top() < bounds.top())
        {
            m_velocity = Vector2(0,0);
            setPosition(QPoint(geometry().x(), bounds.top()));
        }
        else
        {
            for(Block* pBlock : blocks)
            {
                const QRect pBlockRect = pBlock->geometry();
                if(geometry().intersects(pBlockRect) && this != pBlock)
                {
                    if(m_value == pBlock->value())
                    {
                        //Set merge block
                        if(m_pDeletingBlock == nullptr)
                        {
                            m_pDeletingBlock = pBlock;
                        }
                        //If found another block other than one to merge to
                        else if(m_pDeletingBlock != pBlock)
                        {
                            //Set velocity to 0 which triggers merge to m_pDeletingBlock
                            m_velocity = Vector2(0,0);
                        }
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(geometry().x(), roundToMultiple(pBlockRect.y() + Constants::BlockSize, Constants::BlockSize)));
                    }
                    break;
                }
            }
        }
    }

    //If block no longer moving & a merge was found. Peform merge
    // (delete this block and double merge blocks value)
    if(m_pDeletingBlock && m_velocity == Vector2(0,0))
    {
        m_pDeletingBlock->setValue(m_value + m_value);
        blocks.removeOne(this);
        delete this;

        for(Block* pBlock : blocks)
        {
            if(pBlock->checkBoundaries(bounds, blocks))
            {
                return true;
            }
        }
        return true;
    }

    return false;
    //Could loop checking if intersets now repositioned...
}

void DLG_Home::on_btn_restart_clicked()
{
    reset();
}
