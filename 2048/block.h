#ifndef BLOCK_H
#define BLOCK_H

#include <QWidget>
#include <QPropertyAnimation>
#include "settings.h"

class Block : public QWidget
{
    Q_OBJECT

public:
    Block(QWidget *parent, const int& value, const QPoint& position);
    ~Block();

    ///Animations
    void startMoveAnimation(int x, int y);

    void setToMerge(int x, int y, Block* pMergingBlock);

    ///Text value
    int value() const;
    void setValue(const int& value);

protected:
    void paintEvent(QPaintEvent* paintEvent) override;

private slots:
    void onMerge();

private:
    void setPosition(const QPoint& position);

    ///Value properties
    int m_value;
    QColor m_col;

    ///Merging with block
    Block* m_pMergingBlock = nullptr;
    QTimer* m_pMergeTimer = nullptr;

    ///Animations
    QPropertyAnimation* m_pMoveAnimation = nullptr;
    bool m_bIsPopping = false;
};

#endif // BLOCK_H
