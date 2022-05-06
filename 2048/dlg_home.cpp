#include "dlg_home.h"
#include "ui_dlg_home.h"

/*#include <QRandomGenerator>
    int randomNumber = QRandomGenerator::global()->generateDouble() * 1;
    m_value = randomNumber == 0 ? 2 : 4;
*/

#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>

namespace Constants
{
const int BlockSize = 30;
const QRect BoardGeometry = QRect(0, 0, BlockSize * 3, BlockSize * 3);

const QRect DrawBlockRect(0, 0, Constants::BlockSize, Constants::BlockSize);

const QMap<int, QColor> BlockColors = {
    {2, Qt::red},
    {4, Qt::blue}
};
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
    m_blocks.push_back(new Block(this, 2, QPoint(0,Constants::BlockSize)));

    m_pUpdateTimer = new QTimer(this);
    connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    m_pUpdateTimer->start(1);
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
                if(pBlock->getRect().top() > Constants::BoardGeometry.top() - 1)
                {
                    pBlock->setVelocity(Vector2(0, -1));
                }
            }
        }
        else if(event->key() == Qt::Key_Down)
        {
            for(Block* pBlock : m_blocks)
            {
                if(pBlock->getRect().bottom() < Constants::BoardGeometry.bottom() + 1)
                {
                    pBlock->setVelocity(Vector2(0, 1));
                }
            }
        }
        else if(event->key() == Qt::Key_Right)
        {
            for(Block* pBlock : m_blocks)
            {
                if(pBlock->getRect().right() < Constants::BoardGeometry.right() + 1)
                {
                    pBlock->setVelocity(Vector2(1, 0));
                }
            }
        }
        else if(event->key() == Qt::Key_Left)
        {
            for(Block* pBlock : m_blocks)
            {
                if(pBlock->getRect().left() > Constants::BoardGeometry.left() - 1)
                {
                    pBlock->setVelocity(Vector2(-1, 0));
                }
            }
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

    //If no blocks moved set accept input for new movement
    m_bAcceptInput = !anyMoved;

    if(anyMoved)
    {
        //If some blocks moved, check they're in correct bounds
        for(Block* pBlock : m_blocks)
        {
            if(pBlock->checkBoundaries(QRect(0,0,120,120), m_blocks))
            {
                m_blocks.removeOne(pBlock);
                delete pBlock;
            }
        }
        update();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Block
///
Block::Block(QWidget* parent, const int& value, const QPoint& position) : QWidget(parent)
{
    setValue(value);
    setPosition(position);
}

void Block::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(Constants::DrawBlockRect, m_col);
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

bool Block::checkBoundaries(QRect bounds, QVector<Block*>& blocks)
{
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
                        pBlock->setValue(m_value + m_value);
                        return true;
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(pBlockRect.left() - Constants::BlockSize, geometry().y()));
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
                        pBlock->setValue(m_value + m_value);
                        return true;
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(bounds.left(), geometry().y()));
                        m_rect.setX(pBlockRect.right());
                        m_rect.setRight(pBlockRect.right() + Constants::BlockSize);
                    }
                }
            }
        }
    }

    if(m_velocity.y() > 0)
    {
        if(m_rect.bottom() > bounds.bottom())
        {
            m_velocity = Vector2(0,0);
            setPosition(QPoint(geometry().x(), geometry().y()));
            m_rect.setY(bounds.bottom() - Constants::BlockSize);
            m_rect.setBottom(bounds.bottom());
        }
        else
        {
            for(Block* pBlock : blocks)
            {
                const QRectF pBlockRect = pBlock->getRect();
                if(m_rect.intersects(pBlockRect) && this != pBlock)
                {
                    if(m_value == pBlock->value())
                    {
                        pBlock->setValue(m_value + m_value);
                        return true;
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(geometry().x(), geometry().y()));
                        m_rect.setY(pBlockRect.top() - Constants::BlockSize);
                        m_rect.setBottom(pBlockRect.top());
                    }
                }
            }
        }
    }
    else if(m_velocity.y() < 0)
    {
        if(m_rect.top() < bounds.top())
        {
            m_velocity = Vector2(0,0);
            setPosition(QPoint(geometry().x(), geometry().y()));
            m_rect.setY(bounds.top());
            m_rect.setBottom(bounds.top() + Constants::BlockSize);
        }
        else
        {
            for(Block* pBlock : blocks)
            {
                const QRectF pBlockRect = pBlock->getRect();
                if(m_rect.intersects(pBlockRect) && this != pBlock)
                {
                    if(m_value == pBlock->value())
                    {
                        pBlock->setValue(m_value + m_value);
                        return true;
                    }
                    else
                    {
                        m_velocity = Vector2(0,0);
                        setPosition(QPoint(geometry().x(), geometry().y()));
                        m_rect.setY(pBlockRect.bottom());
                        m_rect.setBottom(pBlockRect.bottom() + Constants::BlockSize);
                    }
                }
            }
        }
    }

    return false;
    //Could loop checking if intersets now repositioned...
}
