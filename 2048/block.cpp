#include "block.h"

#include <QTimer>
#include <QPainter>

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

void Block::onCollideWithHitbox(const QPoint &correctPosition)
{
    setVelocity(Vector2(0,0));
    setPosition(correctPosition);
}

void Block::onFoundMergeBlock(Block* pBlock)
{
    //Check if already found block to merge with
    if(m_pFoundMergeBlock == nullptr)
    {
        //Set block to merge with
        m_pFoundMergeBlock = pBlock;
    }

    //Found another possible block to merge with
    //This means can merge with original block - since started going past it
    else if(m_pFoundMergeBlock != pBlock)
    {
        //Set velocity to 0 which triggers merge
        m_velocity = Vector2(0,0);
    }
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

void Block::setVelocity(const Vector2& vel)
{
    m_velocity = vel;
}

Vector2 Block::velocity() const
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
            onCollideWithHitbox(QPoint(bounds.right() - Constants::BlockSize, geometry().y()));
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
                        onFoundMergeBlock(pBlock);
                    }
                    else
                    {
                        onCollideWithHitbox(QPoint(roundToMultiple(pBlockRect.left() - Constants::BlockSize, Constants::BlockSize), geometry().y()));
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
            onCollideWithHitbox(QPoint(bounds.left(), geometry().y()));
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
                        onFoundMergeBlock(pBlock);
                    }
                    else
                    {
                        onCollideWithHitbox(QPoint(roundToMultiple(pBlockRect.right(), Constants::BlockSize), geometry().y()));
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
            onCollideWithHitbox(QPoint(geometry().x(), bounds.bottom() - Constants::BlockSize));
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
                        onFoundMergeBlock(pBlock);
                    }
                    else
                    {
                        onCollideWithHitbox(QPoint(geometry().x(), roundToMultiple(pBlockRect.y() - Constants::BlockSize, Constants::BlockSize)));
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
            onCollideWithHitbox(QPoint(geometry().x(), bounds.top()));
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
                        onFoundMergeBlock(pBlock);
                    }
                    else
                    {
                        onCollideWithHitbox(QPoint(geometry().x(), roundToMultiple(pBlockRect.y() + Constants::BlockSize, Constants::BlockSize)));
                    }
                    break;
                }
            }
        }
    }

    //If block no longer moving & a merge was found. Peform merge
    // (delete this block and double merge blocks value)
    if(m_pFoundMergeBlock && m_velocity == Vector2(0,0))
    {
        m_pFoundMergeBlock->setValue(m_value + m_value);
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
