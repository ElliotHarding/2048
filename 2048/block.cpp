#include "block.h"

#include <QTimer>
#include <QPainter>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Block
///
Block::Block(QWidget* parent, const int& value, const QPoint& position) : QWidget(parent)
{
    //Set inital values
    // - will be moved to seperate function when object recycling is done
    setValue(value);
    setPosition(position);
    show();

    m_pMergeTimer = new QTimer(this);
    m_pMergeTimer->setSingleShot(true);
    connect(m_pMergeTimer, SIGNAL(timeout()), this, SLOT(onMerge()));
}

Block::~Block()
{
    if(m_pMoveAnimation != nullptr)
    {
        m_pMoveAnimation->stop();
        delete m_pMoveAnimation;
    }

    m_pMergeTimer->stop();
    delete m_pMergeTimer;

    m_pMergingBlock = nullptr;
}

void Block::startMoveAnimation(int x, int y)
{
    if(m_pMoveAnimation)
    {
        m_pMoveAnimation->stop();
        delete m_pMoveAnimation;
    }

    m_pMoveAnimation = new QPropertyAnimation(this, "geometry");
    m_pMoveAnimation->setDuration(Constants::MoveAnimationMs);
    m_pMoveAnimation->setStartValue(geometry());
    m_pMoveAnimation->setEndValue(QRect(Constants::BoardGeometry.left() + x * Constants::BlockSize,
                                Constants::BoardGeometry.top() + y * Constants::BlockSize,
                                Constants::BlockSize,
                                Constants::BlockSize));
    m_pMoveAnimation->start();
}

void Block::setToMerge(int x, int y, Block *pMergingBlock)
{
    startMoveAnimation(x, y);
    m_pMergingBlock = pMergingBlock;
    m_pMergeTimer->start(Constants::MoveAnimationMs);
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

void Block::onMerge()
{
    m_pMergingBlock->setValue(m_value*2);
    delete this;
}

int Block::value() const
{
    return m_value;
}

void Block::setValue(const int &value)
{
    m_value = value;
    m_col = Constants::BlockColors[m_value];

    //todo pop

    update();
}

void Block::setPosition(const QPoint& position)
{
    setGeometry(position.x(), position.y(), Constants::BlockSize, Constants::BlockSize);
}
