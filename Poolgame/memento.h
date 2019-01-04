#pragma once
#include <QJsonObject>
#include <functional>
#include <QMouseEvent>

#include "abstractstagefactory.h"
#include "table.h"
#include "ball.h"
#include "balldecorator.h"
#include "utils.h"



class Memento
{

public: //wide interface
    virtual ~Memento();

private: //narrow interface
    friend class Game;
    Memento(std::vector<Ball*>* balls, Table* table, MouseEventable::EventQueue events,
            QVector2D screenShake, double shakeRadius, double shakeAngle, bool myTurn, int redBallsSunk, int yellowBallsSunk, bool blackSunk);

    std::vector<Ball*>* m_balls;
    Table* m_table;
    MouseEventable::EventQueue m_mouseEventFunctions;
    QVector2D m_screenshake;
    double m_shakeRadius;
    double m_shakeAngle;
    bool m_myTurn;
    int m_redBallsSunk;
    int m_yellowBallsSunk;
    bool m_blackSunk;
};

