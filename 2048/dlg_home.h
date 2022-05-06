#ifndef DLG_HOME_H
#define DLG_HOME_H

#include <QMainWindow>
#include <QMutex>

#include "block.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DLG_Home; }
QT_END_NAMESPACE

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

    ///Ai loop function
    void onAiThink();

    ///UI slots
    void on_btn_restart_clicked();

private:
    Ui::DLG_Home *ui;

    ///Resets game
    void reset();

    ///Make a move human + AI
    void move(Qt::Key dirKey);

    ///Block generation
    /// - returns true if succeeded in generating block (board could be full)
    bool trySpawnNewBlock();

    ///User input
    bool m_bAcceptInput = true;

    ///Block info
    QVector<Block*> m_blocks;
    QMutex m_blocksMutex;
    QVector<QPoint> m_blocksPositionsBeforeInput;

    ///Score and highscore
    int m_currentScore;
    int m_highScore = 0;
    void updateScores();

    ///Game loop timer
    QTimer* m_pUpdateTimer;
    QTimer* m_pAiTimer;
};
#endif // DLG_HOME_H
