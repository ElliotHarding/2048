#ifndef DLG_HOME_H
#define DLG_HOME_H

#include <QMainWindow>
#include <QMutex>
#include <QFrame>
#include <QThread>

#include "ai.h"
#include "block.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DLG_Home; }
QT_END_NAMESPACE

class AiThread;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///DLG_Home
///
class DLG_Home : public QMainWindow
{
    Q_OBJECT

public:
    DLG_Home(QWidget *parent = nullptr);
    ~DLG_Home();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:

    ///Ai
    void onAiThink();
    void onAiMove(int direction);

    ///UI slots
    void on_btn_restart_clicked();
    void onBlockAnimationsFinished();

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

    ///Score and highscore
    int m_currentScore;
    int m_highScore = 0;
    void updateScores();

    ///Game loop timer
    QTimer* m_pFinishAnimationTimer; //Calls onBlockAnimationsFinished() once move animations have finished
    bool m_bGameOver = false;

    ///AI
    AiThread* m_pAiThread;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///AiThread
///
class AiThread : public QThread
{
        Q_OBJECT
public:
    AiThread();

    void requestMove(const std::vector<std::vector<int>>& map);

    ///Flags
    void setStop();
    bool isWorking();

    ///Actual thread function
    void run();

signals:
    ///Communication with UI thread
    void foundBestDirection(int direction);

private:
    ///Atomic flags
    std::atomic<bool> m_bStop;
    std::atomic<bool> m_bWorking;

    ///Memory for working
    AI m_ai;
    std::vector<std::vector<int>> m_map;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///TestThread
///
class TestThread : public QThread
{
    Q_OBJECT
public:
    TestThread();
    void run();
};

#endif // DLG_HOME_H
