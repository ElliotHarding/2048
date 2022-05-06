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

    ///Text value
    int value() const;
    void setValue(const int& value);

    bool updatePosition();//Returns true if the position changed

    ///Movement
    void setVelocity(Vector2 vel);
    Vector2 velocity();

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

class DLG_Home : public QMainWindow
{
    Q_OBJECT

public:
    DLG_Home(QWidget *parent = nullptr);
    ~DLG_Home();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    ///Game loop function
    void onUpdate();

    ///UI slots
    void on_btn_restart_clicked();

private:
    Ui::DLG_Home *ui;

    ///Resets game
    void reset();

    ///Block generation
    /// - returns true if succeeded in generating block (board could be full)
    bool trySpawnNewBlock();

    ///User input
    bool m_bAcceptInput = true;

    ///Block info
    QVector<Block*> m_blocks;
    QMutex m_blocksMutex;
    QVector<QPoint> m_blockPositionsBeforeInput;

    ///Score and highscore
    int m_currentScore;
    int m_highScore = 0;
    void updateScores();

    ///Game loop timer
    QTimer* m_pUpdateTimer;
};
#endif // DLG_HOME_H
