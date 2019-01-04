#pragma once

#include "ball.h"
#include "table.h"
#include "utils.h"
#include "game.h"

class Strategy
{
public:

    /**
     * @brief getMove - calucate and return the move to play
     * @param game - the game to analyse
     * @return - the shot to make
     */
    virtual QVector2D getMove(Game* game) = 0;


    /**
     * @brief render - draw the details of the shot, such as which turn and how many of each colour ball are sunk
     * @param painter - the brush to use to draw
     * @param game - the game to retrieve details from
     */
    virtual void render(QPainter& painter, Game *game) = 0;
};


