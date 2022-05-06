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
    ~Block();

    int value() const;
    void setValue(const int& value);

    bool updatePosition();//Returns true if the position changed

    void setVelocity(Vector2 vel);
    Vector2 velocity();

    //Enforces hitboxes & checks for & performs block merge
    bool checkBoundaries(QRect bounds, QVector<Block*>& blocks);

protected:
    void paintEvent(QPaintEvent* paintEvent) override;

private slots:
    void onEndPopping();

private:
    void setPosition(const QPoint& position);

    Block* m_pDeletingBlock = nullptr;
    int m_value;
    QColor m_col;
    Vector2 m_velocity;

    ///Block popping stuff
    /// - Block pops bigger when merging or spawning
    bool m_bIsPopping = false;
    QTimer* m_pPoppingTimer;
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
    void on_btn_restart_clicked();

private:
    Ui::DLG_Home *ui;

    void reset();

    bool trySpawnNewBlock();

    bool m_bAcceptInput = true;

    QVector<Block*> m_blocks;
    QMutex m_blocksMutex;
    QVector<QPoint> m_blockPositionsBeforeInput;

    int m_currentScore = 0;
    int m_highScore = 0;
    void updateScores();

    QTimer* m_pUpdateTimer;
};
#endif // DLG_HOME_H
