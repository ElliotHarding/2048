#ifndef DLG_HOME_H
#define DLG_HOME_H

#include <QMainWindow>
#include <QMutex>
#include <QFrame>

#include "ai.h"
#include "block.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DLG_Home; }
QT_END_NAMESPACE

class AiThread;

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
    void onUpdate();//Called once blocks move animations are finished

    ///Ai
    void onAiThink();
    void onAiMove(int direction);

    ///UI slots
    void on_btn_restart_clicked();
    void on_cb_useAi_toggled(bool checked);


private:
    ///UI
    Ui::DLG_Home *ui;
    QList<QFrame*> m_uiLines;

    ///Resets game
    void reset();
    void resetUiLinesAndGeometry();//Creates grid for UI

    ///Make a move human + AI
    void move(Direction direction);

    ///Block generation
    /// - returns true if succeeded in generating block (board could be full)
    bool trySpawnNewBlock();

    ///User input
    bool m_bAcceptInput = true;

    ///Block info
    QVector<QVector<Block*>> m_blocksGrid;
    QMutex m_blocksMutex;

    ///Score and highscore
    int m_currentScore;
    int m_highScore = 0;
    void updateScores();

    ///Game loop timer
    QTimer* m_pAiTimer; //Calls AI movement decision slot every Constants::AiThinkFrequency ms
    QTimer* m_pFinishAnimationTimer; //Calls onUpdate() once move animations have finished
    bool m_bGameOver = false;

    ///AI
    AiThread* m_pAiThread;
};

#include <QThread>
class AiThread : public QThread
{
        Q_OBJECT
public:
    AiThread();

    void setMap(const QVector<QVector<int>>& map);
    void setStop();
    bool isSetStop();

    void run();

signals:
    void foundBestDirection(int direction);

private:
    QMutex m_mutex;
    QVector<QVector<int>> m_map;
    bool m_bStop;
    bool m_bWorkOnMap;
    AI m_ai;//Probably could make a static function for AI
};

class TestThread : public QThread
{
    Q_OBJECT
public:
    TestThread();
    void run();
};

#endif // DLG_HOME_H
