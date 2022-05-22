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
const int MoveAnimationMs = 150;

///Spawn block settings
const int PercentageSpawn2block = 80;
const int PercentageSpawn4block = 20;
const double RatioSpawn2Block = PercentageSpawn2block/100;
const double RatioSpawn4Block = PercentageSpawn4block/100;

///Visual block settings
const int BlockPopTimeMs = 100; //Block pops bigger when merging or spawning
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

///Score weights for game state valuation
const int ScoreWeightNumMerges = 350;
const int ScoreWeightSmoothness = 10;
const int ScoreWeightHighestNumber = 100;
const int ScoreWeightNumberEmptySpots = 100;

///Score weights for monoicity game state valuation
const int ScoreWeight_Monoicity = 40;
const int ScoreWeight_Monoicity_Smoothness = 10;
const int ScoreWeight_Monoicity_HighestNumber = 150;
const int ScoreWeight_Monoicity_NumberEmptySpots = 270;
const int BaseScore_Monoicity = 2000;

///Depth for best direction choice calcuations
const int DirectionChoiceDepth = 3;

///Movement options
const QList<Direction> PossibleMoveDirections = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};

}

//#define AI_DEBUG

#endif // SETTINGS_H
