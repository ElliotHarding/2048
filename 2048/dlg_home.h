#ifndef DLG_HOME_H
#define DLG_HOME_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class DLG_Home; }
QT_END_NAMESPACE

typedef QPointF Vector2;

class Block
{
public:
    Block(const int& value, const QPoint& position);

    void paint(QPainter& painter);

    int value() const;
    void setValue(const int& value);

    QRectF getRect() const;

    bool updatePosition();

    void setVelocity(Vector2 vel);
    bool checkBoundaries(QRect bounds, QVector<Block*>& blocks);

private:
    int m_value;
    QRectF m_rect;
    QColor m_col;
    Vector2 m_velocity;
};

class DLG_Home : public QMainWindow
{
    Q_OBJECT

public:
    DLG_Home(QWidget *parent = nullptr);
    ~DLG_Home();

protected:
    void paintEvent(QPaintEvent* paintEvent) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onUpdate();

private:
    Ui::DLG_Home *ui;

    bool m_bAcceptInput = true;

    QVector<Block*> m_blocks;

    QTimer* m_pUpdateTimer;
};
#endif // DLG_HOME_H
