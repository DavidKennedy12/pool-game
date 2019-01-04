#pragma once

#include "ball.h"
#include "game.h"
#include "memento.h"
#include "strategy.h"

class Opponent
{
public:
    Opponent(Game *game, Strategy *strategy);

    ~Opponent();

    /**
     * @brief getMovet - return the next shot that the computer will make
     * @return the chosen shot
     */
    QVector2D getMove();

    /**
     * @brief setGame - set the game for opponent to analyse
     * @param game - the game to be analysed
     */
    void setGame(Game *game);

    /**
     * @brief render - draw the table to screen using the specified painter
     * @param painter - painter to use
     * @param game - to retrieve information about sunken balls to display
     */
    void render(QPainter &painter, Game* game);

private:
    Game* m_game;
    Strategy *m_strategy;
};
