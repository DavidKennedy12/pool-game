#pragma once
#include "strategy.h"
class StrategyOne: public Strategy
{
public:
     QVector2D getMove(Game* game) override;
     void render(QPainter& painter, Game *game) override;
};

