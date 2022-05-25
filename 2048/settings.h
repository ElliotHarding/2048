#ifndef SETTINGS_H
#define SETTINGS_H

#include <QRect>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QMap>

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

namespace Constants
{

///Speed & movement
const int AiThinkFrequency = 100;
const int MoveAnimationMs = 150; //Must be bigger than BlockPopTimeMs (below)

///Spawn block settings
const int PercentageSpawn2block = 80;
const int PercentageSpawn4block = 20;
const double RatioSpawn2Block = PercentageSpawn2block/100;
const double RatioSpawn4Block = PercentageSpawn4block/100;

///Visual block settings
const int BlockPopTimeMs = 100; //Block pops bigger when merging or spawning //Must be smaller than MoveAnimationMs
const int BlockSize = 100;
const int BlockDrawMargin = 5;
const int BlockRectRadius = 10;

///Visual block settings (text and colors)
const QFont BlockTextFont = QFont("Helvetica [Cronyx]", 10, QFont::Normal);
const QFontMetrics BlockTextFontMetrics(BlockTextFont);
const QColor BlockTextColor = Qt::black;
const QMap<int, QColor> BlockColors = {
    {2, QColor(238, 228, 218)},
    {4, QColor(237, 224, 200)},
    {8, QColor(242, 177, 121)},
    {16, QColor(245, 149, 99)},
    {32, QColor(246, 124, 95)},
    {64, QColor(246, 94, 59)},
    {128, QColor(237, 207, 114)},
    {256, QColor(237, 204, 97)},
    {512, QColor(237, 200, 80)},
    {1024, QColor(237, 197, 63)},
    {2048, QColor(237, 194, 46)},
};

///Board settings
const QPoint BoardStart = QPoint(0, 100);

///Generic AI params
const double DepthMultiplier = 0.1;

///AI Score weights for game state valuation
const int ScoreWeightSumMerges = 550;
const int ScoreWeightSmoothness = 10;
const int ScoreWeightHighestNumber = 100;
const int ScoreWeightNumberEmptySpots = 100;

///AI Score weights for monoicity game state valuation
const int ScoreWeight_Monoicity = 40;
const int ScoreWeight_Monoicity_Smoothness = 10;
const int ScoreWeight_Monoicity_HighestNumber = 150;
const int ScoreWeight_Monoicity_NumberEmptySpots = 270;
const int BaseScore_Monoicity = 2000;

///Depth for best direction choice calcuations
const int DirectionChoiceDepth = 3;

///Movement options
const std::vector<Direction> PossibleMoveDirections = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};

}

///Test timing & algorithm of AI
#define AI_DEBUG

///Run multiple games simutaneously, outputting end score
/// - Triggerd by pressing reset UI button
//#define RUN_TESTS

///Sum method
/// - Just find highest scoring move, dont sum move scores with depth
#define AI_NO_SUM_SCORES

///Sum method
/// - A messed up method, but got to 2048
#ifdef AI_NO_SUM_SCORES
//#define AI_NO_SUM_WINNER_1
#endif

///AI Algorithm method
//#define AI_CACHE

///AI Algorithm method
#define AI_NORMAL //Reached 2048

///AI Algorithm method
#ifndef AI_NORMAL
//#define AI_MONOICITY
#endif

///AI Algorithm method
//#define AI_SNAKE

///AI Algorithm method
//#define AI_SMOOTH_1

///AI Algorithm method
//#define AI_SMOOTH_2

#endif // SETTINGS_H
