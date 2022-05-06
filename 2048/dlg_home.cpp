#include "dlg_home.h"
#include "ui_dlg_home.h"

#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>

namespace Constants
{
const int UpdateFrequency = 1;

const int BlockSize = 30;
const int MaxBlocksPerRow = 4;
const int MaxBlocksPerCol = 4;
const QRect BoardGeometry = QRect(0, 0, BlockSize * MaxBlocksPerRow, BlockSize * MaxBlocksPerCol);

const QRect DrawBlockRect(0, 0, Constants::BlockSize, Constants::BlockSize);

const QMap<int, QColor> BlockColors = {
    {2, Qt::red},
    {4, Qt::blue}
};

const QFont NumberTextFont = QFont("Helvetica [Cronyx]", 10, QFont::Normal);
const QFontMetrics NumberTextFontMetrics(NumberTextFont);
const QColor NumberTextCol = Qt::white;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///DLG_Home
///
DLG_Home::DLG_Home(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::DLG_Home),
      m_bAcceptInput(true)
{
    ui->setupUi(this);

    m_blocks.push_back(new Block(this, 2, QPoint(0,0)));
    m_blocks.push_back(new Block(this, 4, QPoint(Constants::BlockSize,0)));
    m_blocks.push_back(new Block(this, 2, QPoint(Constants::BlockSize*2,0)));
    m_blocks.push_back(new Block(this, 4, QPoint(0,Constants::BlockSize)));
    m_blocks.push_back(new Block(this, 2, QPoint(0,Constants::BlockSize*2)));

    m_pUpdateTimer = new QTimer(this);
    connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    m_pUpdateTimer->start(Constants::UpdateFrequency);
}

DLG_Home::~DLG_Home()
{
    delete ui;

    for(Block* pBlock : m_blocks)
    {
        delete pBlock;
        pBlock = nullptr;
    }
    m_blocks.clear();
}

void DLG_Home::keyPressEvent(QKeyEvent *event)
{
    if(m_bAcceptInput)
    {
        if(event->key() == Qt::Key_Up)
        {
            for(Block* pBlock : m_blocks)
            {
                if(pBlock->geometry().top() > Constants::BoardGeometry.top() - 1)
                {
                    pBlock->setVelocity(Vector2(0, -1));
                }
            }
            m_bAcceptInput = false;
        }
        else if(event->key() == Qt::Key_Down)
        {
            for(Block* pBlock : m_blocks)
            {
                if(pBlock->geometry().bottom() < Constants::BoardGeometry.bottom() + 1)
                {
                    pBlock->setVelocity(Vector2(0, 1));
                }
            }
            m_bAcceptInput = false;
        }
        else if(event->key() == Qt::Key_Right)
        {
            for(Block* pBlock : m_blocks)
            {
                if(pBlock->geometry().right() < Constants::BoardGeometry.right() + 1)
                {
                    pBlock->setVelocity(Vector2(1, 0));
                }
            }
            m_bAcceptInput = false;
        }
        else if(event->key() == Qt::Key_Left)
        {
            for(Block* pBlock : m_blocks)
            {
                if(pBlock->geometry().left() > Constants::BoardGeometry.left() - 1)
                {
                    pBlock->setVelocity(Vector2(-1, 0));
                }
            }
            m_bAcceptInput = false;
        }
    }
}

void DLG_Home::onUpdate()
{

    //Update positions, check if any moved
    bool anyMoved = false;
    for(Block* pBlock : m_blocks)
    {
        const bool moved = pBlock->updatePosition();
        anyMoved = moved | anyMoved;
    }

    if(!m_bAcceptInput && !anyMoved)
    {
        if(!trySpawnNewBlock())
        {
            //todo - Game over...
        }
        else
        {
            update();
            m_bAcceptInput = true;
        }
    }

    if(anyMoved)
    {
        //If some blocks moved, check they're in correct bounds
        for(Block* pBlock : m_blocks)
        {
            pBlock->checkBoundaries(Constants::BoardGeometry, m_blocks);
        }
        update();
    }
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

    const int randomStartOption = QRandomGenerator::global()->generateDouble() * 1;
    const int startValue = randomStartOption == 0 ? 2 : 4;

    m_blocks.push_back(new Block(this, startValue, spawnPos));

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Block
///
Block::Block(QWidget* parent, const int& value, const QPoint& position) : QWidget(parent)
{
    setValue(value);
    setPosition(position);
    show();
}

void Block::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(Constants::DrawBlockRect, m_col);
    painter.setPen(Constants::NumberTextCol);
    painter.setFont(Constants::NumberTextFont);

    const float textWidth = Constants::NumberTextFontMetrics.horizontalAdvance(QString::number(m_value));

    painter.drawText(QPoint(Constants::BlockSize/2 - textWidth/2, Constants::BlockSize/2 + Constants::NumberTextFontMetrics.height()/4), QString::number(m_value));
}

int Block::value() const
{
    return m_value;
}

void Block::setValue(const int &value)
{
    m_value = value;
    m_col = Constants::BlockColors[m_value];
}

void Block::setPosition(const QPoint& position)
{
    setGeometry(position.x(), position.y(), Constants::BlockSize, Constants::BlockSize);
}

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

void Block::checkBoundaries(QRect bounds, QVector<Block*>& blocks)
{
    //If the block has a velocity:
    // - Check the block is in bounds (only considoring bounds in velocity direction)
    //   - If not in bounds push it back into bounds and stop its velocity
    // - Otherwise check if block collides (intersects) with any other blocks
    //   - If collides with same m_value block; plan to remove due to merge
    //   - If collides with othr m_value block; stop velocity and set flush against other block.

    if(m_velocity.x() > 0)
    {
        if(geometry().right() > bounds.right())
        {
            m_velocity = Vector2(0,0);
            setPosition(QPoint(bounds.right() - Constants::BlockSize + 1, geometry().y()));
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
                        m_pDeletingBlock = pBlock;
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(pBlockRect.left() - Constants::BlockSize, geometry().y()));
                        break;
                    }
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
                        m_pDeletingBlock = pBlock;
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(pBlockRect.right() + 1, geometry().y()));
                        break;
                    }
                }
            }
        }
    }

    if(m_velocity.y() > 0)
    {
        if(geometry().bottom() > bounds.bottom())
        {
            m_velocity = Vector2(0,0);
            setPosition(QPoint(geometry().x(), bounds.bottom() - Constants::BlockSize + 1));
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
                        m_pDeletingBlock = pBlock;
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(geometry().x(), pBlockRect.top() - Constants::BlockSize));
                        break;
                    }
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
                        m_pDeletingBlock = pBlock;
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(geometry().x(), pBlockRect.bottom() + 1));
                        break;
                    }
                }
            }
        }
    }

    if(m_pDeletingBlock && m_velocity == Vector2(0,0))
    {
        m_pDeletingBlock->setValue(m_value + m_value);
        blocks.removeOne(this);
        delete this;
    }

    //Could loop checking if intersets now repositioned...
}
