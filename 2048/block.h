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

    ///Text value
    int value() const;
    void setValue(const int& value);

protected:
    void paintEvent(QPaintEvent* paintEvent) override;

private:
    void setPosition(const QPoint& position);

    ///Text value
    int m_value;

    ///Color - related to value
    QColor m_col;

    ///Animations
    QPropertyAnimation* m_pMoveAnimation = nullptr;
    bool m_bIsPopping = false;
};

#endif // BLOCK_H
