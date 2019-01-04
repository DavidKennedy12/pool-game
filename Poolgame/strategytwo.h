#pragma once
#include "strategy.h"
class StrategyTwo: public Strategy
{
public:
     QVector2D getMove(Game* game) override;
     void render(QPainter& painter, Game *game) override;
};

