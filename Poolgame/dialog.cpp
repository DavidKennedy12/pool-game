#include "dialog.h"
#include "ui_dialog.h"
#include <QPainter>
#include <QTimer>
#include <iostream>
#include <QMouseEvent>
#include "utils.h"

Dialog::Dialog(Game *game, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_game(game)
{
    ui->setupUi(this);

    // for animating (i.e. movement, collision) every animFrameMS
    aTimer = new QTimer(this);
    connect(aTimer, SIGNAL(timeout()), this, SLOT(nextAnim()));
    aTimer->start(animFrameMS);

    // for drawing every drawFrameMS milliseconds
    dTimer = new QTimer(this);
    connect(dTimer, SIGNAL(timeout()), this, SLOT(tryRender()));
    dTimer->start(drawFrameMS);

    // set the window size to be at least the table size
    this->resize(game->getMinimumWidth(), game->getMinimumHeight() + 80);
    this->setStyleSheet("Background-color: #00004d;");


    Memento* test = this->m_game->saveToMemento();
    mementos.push_back(test);

}

Dialog::~Dialog()
{
    delete aTimer;
    delete dTimer;
    delete m_game;
    delete ui;
}

void Dialog::tryRender() {
    this->update();
}

void Dialog::nextAnim() {
    if(m_game->m_myTurn == false && m_game->readyToShoot()) //cpu move
    {
        m_opponent->setGame(m_game->clone());
        m_game->applyMove(m_opponent->getMove());
        m_game->m_myTurn = true;
    }
    m_game->animate(1.0/(double)animFrameMS);
}

void Dialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    m_game->render(painter);
    m_opponent->render(painter, m_game);

}

void Dialog::mousePressEvent(QMouseEvent* event) {
    if (!m_game->m_myTurn || !m_game->readyToShoot())
    {
        return;
    }
    evalAllEventsOfTypeSpecified(MouseEventable::EVENTS::MouseClickFn, event);
}

void Dialog::mouseReleaseEvent(QMouseEvent* event) {
    if (!m_game->m_myTurn)
    {
        return;
    }
    bool before = m_game->readyToShoot();
    Memento* test = this->m_game->saveToMemento();
    evalAllEventsOfTypeSpecified(MouseEventable::EVENTS::MouseRelFn, event);
    bool after = m_game->readyToShoot();

    if(before == true && after == false)
    {
        mementos.push_back(test);
        m_game->m_myTurn = false;
    }
    else
    {
        delete test;
    }
}
void Dialog::mouseMoveEvent(QMouseEvent* event) {
    evalAllEventsOfTypeSpecified(MouseEventable::EVENTS::MouseMoveFn, event);
}

void Dialog::setOpponent(Opponent *opponent)
{
    m_opponent = opponent;
}

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_R && mementos.size() > 0) //is space bar hit
    {
        this->m_game->restoreFromMemento(*mementos.at(mementos.size() - 1));
        mementos.pop_back();
        if(mementos.size() == 0)
        {
               mementos.push_back(m_game->saveToMemento());
        }
    }
}

void Dialog::evalAllEventsOfTypeSpecified(MouseEventable::EVENTS t, QMouseEvent *event) {
    // handle all the clicky events, and remove them if they've xPIRED
    MouseEventable::EventQueue& Qu = m_game->getEventFns();
    for (ssize_t i = Qu.size()-1; i >= 0; i--) {
        if (auto spt = (Qu.at(i)).lock()) {
            if (spt->second == t) {
                spt->first(event);
            }
        } else {
            // remove this element from our vector
            Qu.erase(Qu.begin() + i);
        }
    }
}
