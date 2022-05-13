#ifndef SETTINGS_H
#define SETTINGS_H

#include <QRect>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QMap>

typedef QPointF Vector2;

namespace Constants
{

///Speed & movement
const int GameUpdateFrequency = 1;
const int BlockMovementSpeed = 3;
const int AiThinkFrequency = 1000;

///Visual block settings
const int BlockPopTimeMs = 100; //Block pops bigger when merging or spawning
const int BlockSize = 100;
const int BlockDrawMargin = 5;
const int BlockRectRadius = 10;
const QRect BlockDrawRect(BlockDrawMargin, BlockDrawMargin, Constants::BlockSize-BlockDrawMargin*2, Constants::BlockSize-BlockDrawMargin*2);
const QRect BlockDrawRectPopping(0, 0, Constants::BlockSize, Constants::BlockSize);

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
const int MaxBlocksPerRow = 4;
const int MaxBlocksPerCol = 4;
const int MaxBlocks = MaxBlocksPerCol * MaxBlocksPerRow;
const QRect BoardGeometry = QRect(0, BlockSize, BlockSize * MaxBlocksPerRow + 1, BlockSize * MaxBlocksPerCol + 1);

///Score weights for game state value calculations
const int ScoreWeightHighTopLeftN0 = 100; //Reward % for having highest number in top left slot
const int ScoreWeightHighTopLeftN1 = 50;
const int ScoreWeightHighTopLeftN2 = 20;
const int ScoreWeightHighNumbersCloseN0 = 70; //Reward % for having high value blocks next to eachother
const int ScoreWeightHighNumbersCloseN1 = 40;
const int ScoreWeightHighestNumber = 1; //Reward % for having the highest number
const int ScoreWeightNumberBlocks = -15;

///Depth for best direction choice calcuations
const int DirectionChoiceDepth = 2;

///Movement options
const QList<Vector2> PossibleMoveDirections = {Vector2(0, 1), Vector2(0, -1), Vector2(1, 0), Vector2(-1, 0)};

}

#endif // SETTINGS_H
