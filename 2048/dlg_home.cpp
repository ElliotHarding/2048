#include "dlg_home.h"
#include "ui_dlg_home.h"

#include <QRandomGenerator>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>
#include <QThread>
#include <QPropertyAnimation>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///DLG_Home
///
DLG_Home::DLG_Home(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::DLG_Home)
{
    ui->setupUi(this);

    m_pAiThread = new AiThread();
    connect(m_pAiThread, SIGNAL(foundBestDirection(int)), this, SLOT(onAiMove(int)));
    m_pAiThread->start();

    //Calls AI movement decision slot every Constants::AiThinkFrequency ms
    m_pAiTimer = new QTimer(this);
    m_pAiTimer->setTimerType(Qt::PreciseTimer);
    connect(m_pAiTimer, SIGNAL(timeout()), this, SLOT(onAiThink()));    

    //Calls onUpdate() once move animations have finished
    m_pFinishAnimationTimer = new QTimer(this);
    m_pFinishAnimationTimer->setTimerType(Qt::PreciseTimer);
    m_pFinishAnimationTimer->setSingleShot(true);
    connect(m_pFinishAnimationTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));

    reset();
}

DLG_Home::~DLG_Home()
{
    m_blocksMutex.lock();

    //Stop and delete animation timer
    m_pFinishAnimationTimer->stop();
    delete m_pFinishAnimationTimer;

    //Stop and delete AI timer
    m_pAiTimer->stop();
    delete m_pAiTimer;

    //Stop and delete AI thread
    //Terrible code
    m_pAiThread->setStop();
    while(m_pAiThread->isWorking())
    {       
        m_blocksMutex.unlock();
        QThread::sleep(10);
        m_blocksMutex.lock();
    }
    m_pAiThread->terminate();
    delete m_pAiThread;

    for(QVector<Block*> blockCol : m_blocksGrid)
    {
        for(Block* block : blockCol)
        {
            delete block;
        }
        blockCol.clear();
    }
    m_blocksGrid.clear();

    m_blocksMutex.unlock();
    delete ui;
}

//Reset game
void DLG_Home::reset()
{
    resetUiLinesAndGeometry();

    m_blocksMutex.lock();

    //Remove previous blocks
    // todo : object recycle system to stop creating new memory each time
    for(QVector<Block*> blockCol : m_blocksGrid)
    {
        for(Block* block : blockCol)
        {
            delete block;
        }
        blockCol.clear();
    }
    m_blocksGrid.clear();

    //Create new grid
    m_blocksGrid = QVector<QVector<Block*>>(ui->sb_cols->value(), QVector<Block*>(ui->sb_rows->value(), nullptr));

    //Initial game board contains one block
    m_blocksGrid[0][0] = new Block(this, 2, Constants::BoardStart);

    m_bAcceptInput = true;
    m_bGameOver = false;

    //Start game loop - runs forever
    if(ui->cb_useAi->isChecked())
    {
        m_pAiTimer->start(Constants::AiThinkFrequency);
    }

    m_blocksMutex.unlock();

    m_currentScore = 0;
    updateScores();
}

//Creates grid for UI
void DLG_Home::resetUiLinesAndGeometry()
{
    //Uses existing line objects to create ui grid
    //If theres not enough line objects, creates more
    //If theres too many line objects, removes them

    const int numCols = ui->sb_cols->value();
    const int numRows = ui->sb_rows->value();
    const int totalWidth = numCols * Constants::BlockSize;
    const int totalHeight = numRows * Constants::BlockSize;
    int lineIndex = 0;
    for(int x = 0; x < numCols+1; x++)
    {
        if(lineIndex > m_uiLines.size()-1)
        {
            QFrame* newCol = new QFrame(this);
            newCol->setFrameShape(QFrame::VLine);
            newCol->setGeometry(Constants::BoardStart.x() + x * Constants::BlockSize, Constants::BoardStart.y(), 1, totalHeight);
            newCol->raise();
            newCol->show();
            m_uiLines.push_back(newCol);
        }
        else
        {
            m_uiLines[lineIndex]->setFrameShape(QFrame::VLine);
            m_uiLines[lineIndex]->setGeometry(Constants::BoardStart.x() + x * Constants::BlockSize, Constants::BoardStart.y(), 1, totalHeight);
        }
        lineIndex++;
    }
    for(int y = 0; y < numRows+1; y++)
    {
        if(lineIndex > m_uiLines.size()-1)
        {
            QFrame* newRow = new QFrame(this);
            newRow->setFrameShape(QFrame::HLine);
            newRow->setGeometry(Constants::BoardStart.x(), Constants::BoardStart.y() + y * Constants::BlockSize, totalWidth, 1);
            newRow->raise();
            newRow->show();
            m_uiLines.push_back(newRow);
        }
        else
        {
            m_uiLines[lineIndex]->setFrameShape(QFrame::HLine);
            m_uiLines[lineIndex]->setGeometry(Constants::BoardStart.x(), Constants::BoardStart.y() + y * Constants::BlockSize, totalWidth, 1);
        }
        lineIndex++;
    }

    //Remove any extra lines
    if(lineIndex < m_uiLines.size()-1)
    {
        for(int i = lineIndex; i < m_uiLines.size();)
        {
            delete m_uiLines[i];
            m_uiLines.removeAt(i);
        }
    }

    ui->lbl_background->setGeometry(Constants::BoardStart.x(), Constants::BoardStart.y(), totalWidth, totalHeight);
    setGeometry(geometry().x(), geometry().y(), Constants::BoardStart.x() + totalWidth, Constants::BoardStart.y() + totalHeight);
}

void DLG_Home::keyPressEvent(QKeyEvent *event)
{
    if(m_bAcceptInput)
    {
        if(event->key() == Qt::Key_Up)
        {
            move(UP);
        }
        else if(event->key() == Qt::Key_Down)
        {
            move(DOWN);
        }
        else if(event->key() == Qt::Key_Right)
        {
            move(RIGHT);
        }
        else if(event->key() == Qt::Key_Left)
        {
            move(LEFT);
        }
    }
}

bool inRange(const int& value, const int& min, const int& max)
{
    return value >= min && value <= max;
}

typedef QPointF Vector2;
Vector2 directionToVector(Direction direction)
{
    switch (direction)
    {
        case UP:
            return Vector2(0, -1);
        case DOWN:
            return Vector2(0, 1);
        case LEFT:
            return Vector2(-1, 0);
        case RIGHT:
            return Vector2(1, 0);
    }
    return Vector2(0,0);
}

void DLG_Home::move(Direction dir)
{
    m_blocksMutex.lock();

    //Block input until things have moved where they need to go
    m_bAcceptInput = false;

    bool anyMoved = false;

    //Perform move in dir direction
    const int xStart =  dir == RIGHT ? ui->sb_rows->value()-2 : dir == LEFT ? 1 : 0;
    const int xInc =    dir == RIGHT ? -1 : 1;
    const int yStart =  dir == DOWN ? ui->sb_cols->value()-2 : dir == UP ? 1 : 0;
    const int yInc =    dir == DOWN ? -1 : 1;
    Vector2 direction = directionToVector(dir);
    for(int moveCount = 0; moveCount < m_blocksGrid.size()-1; moveCount++)
    {
        bool moved = false;
        for(int x = xStart; inRange(x, 0, ui->sb_cols->value()-1); x+=xInc)
        {
            for(int y = yStart; inRange(y, 0, ui->sb_rows->value()-1); y+=yInc)
            {
                if(m_blocksGrid[x][y] != 0)
                {
                    if(m_blocksGrid[x+direction.x()][y+direction.y()] == 0)
                    {
                        m_blocksGrid[x+direction.x()][y+direction.y()] = m_blocksGrid[x][y];
                        m_blocksGrid[x+direction.x()][y+direction.y()]->startMoveAnimation(Constants::BoardStart.x() + (Constants::BlockSize * (x+direction.x())), Constants::BoardStart.y() + (Constants::BlockSize * (y+direction.y())));
                        m_blocksGrid[x][y] = nullptr;
                        moved = true;
                    }
                    else if(m_blocksGrid[x+direction.x()][y+direction.y()]->value() == m_blocksGrid[x][y]->value())
                    {
                        const int xPos = Constants::BoardStart.x() + (Constants::BlockSize * (x+direction.x()));
                        const int yPos = Constants::BoardStart.y() + (Constants::BlockSize * (y+direction.y()));
                        m_blocksGrid[x][y]->setToMerge(xPos, yPos, m_blocksGrid[x+direction.x()][y+direction.y()]);
                        m_blocksGrid[x][y]->startMoveAnimation(xPos, yPos);
                        m_blocksGrid[x][y] = nullptr;
                        moved = true;
                    }
                }
            }
        }
        if(moved)
        {
            anyMoved = true;
        }
        else
        {
            break;
        }
    }

    if(anyMoved)
    {
        //Spawn timer to handle stuff once move animations are finished (calls onUpdate())
        m_pFinishAnimationTimer->start(Constants::MoveAnimationMs);
    }
    else
    {
        m_bAcceptInput = true;
    }

    m_blocksMutex.unlock();
}

//Called once blocks move animations are finished
void DLG_Home::onUpdate()
{
    m_blocksMutex.lock();

    if(!trySpawnNewBlock())
    {
        m_bGameOver = true;
        m_pAiTimer->stop();
    }
    else
    {
        m_currentScore += 2;
        updateScores();
    }

    m_bAcceptInput = true;

    m_blocksMutex.unlock();
}

void DLG_Home::onAiThink()
{
    m_blocksMutex.lock();
    if(!m_bAcceptInput)
    {
        m_blocksMutex.unlock();
        return;
    }

    //Block input until things have moved where they need to go
    m_bAcceptInput = false;

    //Turn m_blocksGrid into something AI can understand
    std::vector<std::vector<int>> map(ui->sb_cols->value(), std::vector<int>(ui->sb_rows->value(), 0));
    for(int x = 0; x < ui->sb_cols->value(); x++)
    {
        for(int y = 0; y < ui->sb_rows->value(); y++)
        {
            if(m_blocksGrid[x][y] != nullptr)
            {
                map[x][y] = m_blocksGrid[x][y]->value();
            }
        }
    }

#ifdef AI_DEBUG
    //Test map
    map[0][0] = 2;
    map[1][0] = 0;
    map[2][0] = 0;
    map[3][0] = 0;
    map[0][1] = 0;
    map[1][1] = 2;
    map[2][1] = 0;
    map[3][1] = 2048;
    map[0][2] = 0;
    map[1][2] = 4;
    map[2][2] = 0;
    map[3][2] = 0;
    map[0][3] = 0;
    map[1][3] = 2048;
    map[2][3] = 0;
    map[3][3] = 0;
#endif

    m_pAiThread->setMap(map);

    m_blocksMutex.unlock();
}

bool DLG_Home::trySpawnNewBlock()
{
    //Find empty spaces
    QVector<QPoint> emptySpaces;
    for(int x = 0; x < ui->sb_cols->value(); x++)
    {
        for(int y = 0; y < ui->sb_rows->value(); y++)
        {
            if(m_blocksGrid[x][y] == nullptr)
            {
                emptySpaces.push_back(QPoint(x,y));
            }
        }
    }

    //If cant find any empty spaces
    if(emptySpaces.empty())
    {
        return false;
    }

    const int randomPosition = QRandomGenerator::global()->generateDouble() * emptySpaces.size() - 1;
    const QPoint spawnPos = emptySpaces[randomPosition];

    const int randomStartOption = QRandomGenerator::global()->generateDouble() * 100;
    const int startValue = randomStartOption < Constants::PercentageSpawn2block ? 2 : 4;

    m_blocksGrid[spawnPos.x()][spawnPos.y()] = new Block(this, startValue, Constants::BoardStart + QPoint(spawnPos.x() * Constants::BlockSize, spawnPos.y() * Constants::BlockSize));

    return true;
}

void DLG_Home::updateScores()
{
    ui->lblScoreValue->setText(QString::number(m_currentScore));
    if(m_currentScore > m_highScore)
    {
        m_highScore = m_currentScore;
        ui->lblHighScoreValue->setText(QString::number(m_highScore));
    }
}

void DLG_Home::on_btn_restart_clicked()
{
#ifdef RUN_TESTS
    TestThread* pTestThread1 = new TestThread();
    TestThread* pTestThread2 = new TestThread();
    TestThread* pTestThread3 = new TestThread();
    TestThread* pTestThread4 = new TestThread();
    pTestThread1->start(QThread::Priority::HighPriority);
    pTestThread2->start(QThread::Priority::HighPriority);
    pTestThread3->start(QThread::Priority::HighPriority);
    pTestThread4->start(QThread::Priority::HighPriority);
#endif

    reset();
}

void DLG_Home::on_cb_useAi_toggled(bool checked)
{
    if(checked)
    {
        m_pAiTimer->start(Constants::AiThinkFrequency);
    }
    else
    {
        m_pAiTimer->stop();
    }
}

void DLG_Home::onAiMove(int direction)
{
    move((Direction)direction);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///AiThread
///
AiThread::AiThread() :
    QThread(),
    m_bStop(false),
    m_bWorking(false)
{
}

void AiThread::setMap(const std::vector<std::vector<int>>& map)
{
    if(!m_bWorking)
    {
        m_map = map;
        m_bWorking = true;
    }
}

void AiThread::setStop()
{
    m_bStop = true;
}

bool AiThread::isWorking()
{
    return m_bWorking;
}

void AiThread::run()
{
    while(true)
    {
        if(m_bStop)
        {
            return;
        }

        if(m_bWorking)
        {
#ifdef AI_DEBUG
    clock_t start = clock();
#endif
            //Could make a static function for AI...
            const Direction bestDirection = m_ai.getBestDirection(m_map);
            emit foundBestDirection(bestDirection);

#ifdef AI_DEBUG
    clock_t end = clock();
    qDebug() << "DLG_Home::onAiThink: Think time: " << end - start;
#endif

            m_bWorking = false;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///TestThread
///
TestThread::TestThread() : QThread()
{
}

void TestThread::run()
{
    AI ai;
    ai.runTests();
}
