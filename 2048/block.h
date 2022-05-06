#ifndef BLOCK_H
#define BLOCK_H

#include <QWidget>
#include "settings.h"

class Block : public QWidget
{
    Q_OBJECT

public:
    Block(QWidget *parent, const int& value, const QPoint& position);
    ~Block();

    ///Text value
    int value() const;
    void setValue(const int& value);

    ///Movement
    void setVelocity(Vector2 vel);
    Vector2 velocity();
    bool updatePosition();//Returns true if the position changed

    //Enforces hitboxes & checks for & performs block merge
    bool checkBoundaries(QRect bounds, QVector<Block*>& blocks);

protected:
    void paintEvent(QPaintEvent* paintEvent) override;

private:
    void setPosition(const QPoint& position);

    ///Merging brother block
    /// - Set when found a block to merge with
    /// - Waits for this blocks velocity to stop before merging
    Block* m_pDeletingBlock = nullptr;

    ///Text value
    int m_value;

    ///Color - related to value
    QColor m_col;

    ///Movement
    Vector2 m_velocity;

    ///Block popping stuff
    /// - Block pops bigger when merging or spawning
    bool m_bIsPopping = false;
    QTimer* m_pPoppingTimer;
private slots:
    void onEndPopping();
};

#endif // BLOCK_H
