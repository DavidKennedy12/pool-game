#include "opponent.h"

Opponent::Opponent(Game *game, Strategy *strategy)
    :  m_game(game), m_strategy(strategy)
{}

Opponent::~Opponent()
{}

QVector2D Opponent::getMove()
{
    return m_strategy->getMove(m_game);
}

void Opponent::setGame(Game *game)
{
    m_game = game;
}

void Opponent::render(QPainter &painter, Game* game)
{
    m_strategy->render(painter, game);
}
