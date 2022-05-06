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

const QMap<int, QColor> BlockColors = {
    {2, Qt::red},
    {4, Qt::blue}
};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///DLG_Home
///
DLG_Home::DLG_Home(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DLG_Home)
{
    ui->setupUi(this);

    m_blocks.push_back(new Block(2, QPoint(0,0)));
    m_blocks.push_back(new Block(4, QPoint(Constants::BlockSize,0)));
    m_blocks.push_back(new Block(2, QPoint(0,Constants::BlockSize)));

    m_pUpdateTimer = new QTimer(this);
    connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
    m_pUpdateTimer->start(10);
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

void DLG_Home::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    for(Block* pBlock : m_blocks)
    {
        pBlock->paint(painter);
    }
}

void DLG_Home::keyPressEvent(QKeyEvent *event)
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

void DLG_Home::onUpdate()
{
    for(Block* pBlock : m_blocks)
    {
        pBlock->updatePosition();
    }

    for(Block* pBlock : m_blocks)
    {
        pBlock->checkBoundaries(QRect(0,0,120,120), m_blocks);
    }

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Block
///
Block::Block(const int& value, const QPoint& position)
{
    setValue(value);
    m_rect = QRectF(position.x(), position.y(), Constants::BlockSize, Constants::BlockSize);
}

void Block::paint(QPainter& painter)
{
    painter.fillRect(m_rect, m_col);
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

QRectF Block::getRect() const
{
    return m_rect;
}

void Block::updatePosition()
{
    m_rect.translate(m_velocity.x(), m_velocity.y());
}

void Block::setVelocity(Vector2 vel)
{
    m_velocity = vel;
}

void Block::checkBoundaries(QRect bounds, QVector<Block*>& blocks)
{
    if(m_velocity.x() > 0)
    {
        if(m_rect.right() > bounds.right())
        {
            m_velocity = Vector2(0,0);
            m_rect.setX(bounds.right() - Constants::BlockSize);
            m_rect.setRight(bounds.right());
        }
        else
        {
            for(Block* pBlock : blocks)
            {
                const QRectF pBlockRect = pBlock->getRect();
                if(m_rect.intersects(pBlockRect) && this != pBlock)
                {
                    m_velocity = Vector2(0,0);
                    m_rect.setX(pBlockRect.left() - Constants::BlockSize);
                    m_rect.setRight(pBlockRect.left());
                }
            }
        }
    }
    else if(m_velocity.x() < 0)
    {
        if(m_rect.left() < bounds.left())
        {
            m_velocity = Vector2(0,0);
            m_rect.setX(bounds.left());
            m_rect.setRight(bounds.left() + Constants::BlockSize);
        }
        else
        {
            for(Block* pBlock : blocks)
            {
                const QRectF pBlockRect = pBlock->getRect();
                if(m_rect.intersects(pBlockRect) && this != pBlock)
                {
                    m_velocity = Vector2(0,0);
                    m_rect.setX(pBlockRect.right());
                    m_rect.setRight(pBlockRect.right() + Constants::BlockSize);
                }
            }
        }
    }

    if(m_velocity.y() > 0)
    {
        if(m_rect.bottom() > bounds.bottom())
        {
            m_velocity = Vector2(0,0);
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
                    m_velocity = Vector2(0,0);
                    m_rect.setY(pBlockRect.top() - Constants::BlockSize);
                    m_rect.setBottom(pBlockRect.top());
                }
            }
        }
    }
    else if(m_velocity.y() < 0)
    {
        if(m_rect.top() < bounds.top())
        {
            m_velocity = Vector2(0,0);
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
                    m_velocity = Vector2(0,0);
                    m_rect.setY(pBlockRect.bottom());
                    m_rect.setBottom(pBlockRect.bottom() + Constants::BlockSize);
                }
            }
        }
    }

    //Could loop checking if intersets now repositioned...
}
