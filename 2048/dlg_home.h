#ifndef DLG_HOME_H
#define DLG_HOME_H

#include <QMainWindow>
#include <QMutex>

QT_BEGIN_NAMESPACE
namespace Ui { class DLG_Home; }
QT_END_NAMESPACE

typedef QPointF Vector2;

class Block : public QWidget
{
    Q_OBJECT

public:
    Block(QWidget *parent, const int& value, const QPoint& position);

        QRect TESTRECT;

    int value() const;
    void setValue(const int& value);

    bool updatePosition();

    void setVelocity(Vector2 vel);
    Vector2 velocity();

    //Enforces hitboxes & checks for & performs block merge
    bool checkBoundaries(QRect bounds, QVector<Block*>& blocks);

protected:
    void paintEvent(QPaintEvent* paintEvent) override;

private:
    void setPosition(const QPoint& position);

    Block* m_pDeletingBlock = nullptr;
    int m_value;
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
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onUpdate();

private:
    Ui::DLG_Home *ui;

    bool trySpawnNewBlock();

    bool m_bAcceptInput = true;

    bool m_bSomethingMovedSinceLastInput = false;

    QVector<Block*> m_blocks;
    QMutex m_blocksMutex;

    QTimer* m_pUpdateTimer;
};
#endif // DLG_HOME_H
