#ifndef BLOCK_H
#define BLOCK_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include "settings.h"

class Block : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int popValue READ popValue WRITE setPopValue)

public:
    Block(QWidget *parent, const int& value, const QPoint& position);
    ~Block();

    ///Animations
    void startMoveAnimation(int x, int y);
    void setToMerge(int x, int y, Block* pMergingBlock);
    int popValue();
    void setPopValue(int popValue);

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
    QTimer* m_pMergeTimer = nullptr; //Triggers block value doubling after merge after some time (to give UI movement times long enough)

    ///Animations
    QPropertyAnimation* m_pMoveAnimation = nullptr;
    QSequentialAnimationGroup* m_pPopAnimationGroup = nullptr;
    QPropertyAnimation* m_pPopValueIncAnimation = nullptr;
    QPropertyAnimation* m_pPopValueDecAnimation = nullptr;
    int m_popValue = 0;
};

#endif // BLOCK_H
