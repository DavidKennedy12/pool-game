#include "memento.h"

Memento::~Memento()
{}

Memento::Memento(std::vector<Ball *> *balls, Table *table, MouseEventable::EventQueue events, QVector2D screenShake,
                 double shakeRadius, double shakeAngle, bool myTurn, int redBallsSunk, int yellowBallsSunk, bool blackSunk)
    : m_balls(balls), m_table(table), m_mouseEventFunctions(events), m_screenshake(screenShake),
      m_shakeRadius(shakeRadius), m_shakeAngle(shakeAngle), m_myTurn(myTurn), m_redBallsSunk(redBallsSunk), m_yellowBallsSunk(yellowBallsSunk), m_blackSunk(blackSunk)
{}

