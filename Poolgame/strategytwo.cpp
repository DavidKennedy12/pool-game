#include "strategytwo.h"

QVector2D StrategyTwo::getMove(Game *game)
{
    Ball* ballA;
    QVector2D temp;
    QVector2D start = game->getBalls()->front()->getPosition();
    QVector2D line(1000,1000);
    for(auto it = game->getBalls()->begin(); it !=game->getBalls()->end(); ++it)
    {
       ballA = *it;
        if (ballA->getColour() == QColor("yellow")) //only evaluate yellow balls
        {
           temp = ballA->getPosition() - start;
           if(temp.length() < line.length())
           {
               line = temp;
           }
        }
    }
    line.normalize();
    line = line * 300;
    return line;
}

void StrategyTwo::render(QPainter &painter, Game *game )
{
    QFont font=painter.font() ;
     font.setPointSize ( 20 );
     font.setWeight(QFont::DemiBold);
     painter.setFont(font);
    QPen pen(Qt::red);
    painter.setPen(pen);

    QString display = "Player: red";
    painter.drawText(150,530, display);

    display ="Red balls sunk: " + QString::number(game->m_redBallsSunk);
    painter.drawText(150,560, display);

    pen = QPen(Qt::yellow);
    painter.setPen(pen);

    display = "Computer: yellow";
    painter.drawText(420,530, display);

    display = "Yellow balls sunk: " + QString::number(game->m_yellowBallsSunk);
    painter.drawText(420,560, display);

    if(game->m_myTurn)
    {
        display = "Your turn";
    }
    else
    {
        display = "Computers turn";
    }

    pen = QPen(Qt::white);
    painter.setPen(pen);
    painter.drawText(700,540, display);
}


