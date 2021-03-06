#include "game.h"
#include "utils.h"

#include <QJsonArray>
#include <stdexcept>
#include <cmath>
#include <exception>
#include <iostream>

Game::~Game() {
    for (auto b : *m_balls) delete b;
    delete m_balls;
    delete m_table;
}

Game *Game::clone() const
{
    std::vector<Ball*>* tempBalls = new std::vector<Ball*>();
    for (auto it = m_balls->begin(); it != m_balls->end(); ++it)
    {
        Ball* ballA = *it;
        Ball *ballB = ballA->clone();
        tempBalls->push_back(ballB);
    }
    Table *table = m_table->clone();
    Game* newGame = new Game(tempBalls, table);
    newGame->m_blackSunk = m_blackSunk;
    newGame->m_yellowBallsSunk = m_yellowBallsSunk;
    newGame->m_redBallsSunk = m_redBallsSunk;

    return newGame;
}

Memento *Game::saveToMemento()
{
    std::vector<Ball*>* tempBalls = new std::vector<Ball*>();
    for (auto it = m_balls->begin(); it != m_balls->end(); ++it)
    {
        Ball* ballA = *it;
        Ball *ballB = ballA->clone();
        tempBalls->push_back(ballB);

    }
    Table *table = m_table->clone();
    Memento *temp = new Memento(tempBalls, table, this->m_mouseEventFunctions, this->m_screenshake, this->m_shakeRadius, this->m_shakeAngle, this->m_myTurn,
                                this->m_redBallsSunk, this->m_yellowBallsSunk, this->m_blackSunk);
    return temp;
}

void Game::restoreFromMemento(Memento memento)
{
    delete this->m_balls;
    delete this->m_table;
    this->m_balls = memento.m_balls;
    Ball* c = memento.m_balls->front();
    CueBall* cb = new CueBall(c);
    this->m_balls->front() = static_cast<Ball*>(cb);
    addMouseFunctions(cb->getEvents());

    this->m_table = memento.m_table;

    m_screenshake = memento.m_screenshake;
    m_shakeAngle = memento.m_shakeAngle;
    m_shakeRadius = memento.m_shakeRadius;
    this->m_myTurn = memento.m_myTurn;
    this->m_yellowBallsSunk = memento.m_yellowBallsSunk;
    this->m_redBallsSunk = memento.m_redBallsSunk;
    this->m_blackSunk = memento.m_blackSunk;
    //delete current balls and table pointers
}

bool Game::readyToShoot()
{

    for (auto it = m_balls->begin(); it != m_balls->end(); ++it)
    {
        Ball* ballA = *it;
        if (ballA->getVelocity().length() > 1)
        {
            return false;
        }

    }
    return true;
}

void Game::applyMove(QVector2D move)
{
    this->m_balls->front()->setVelocity(move);
}



void Game::render(QPainter &painter) const {
    // table is rendered first, as its the lowest
    m_table->render(painter, m_screenshake);
    // then render all the balls
    for (Ball* b : *m_balls) b->render(painter, m_screenshake);
}

void Game::animate(double dt) {

    // keep track of the removed balls (they're set to nullptr during loop)
    // clean up afterwards
    std::vector<Ball*> toBeRemoved;
    // add these balls to the list after we finish
    std::vector<Ball*> toBeAdded;

    // (test) collide the ball with each other ball exactly once
    // to achieve this, balls only check collisions with balls "after them"
    for (auto it = m_balls->begin(); it != m_balls->end(); ++it) {
        Ball* ballA = *it;
        if (ballA == nullptr) continue;

        // correct ball velocity if colliding with table
        QVector2D tableBallDeltaV = resolveCollision(m_table, ballA);
        // test and resolve breakages with balls bouncing off table
        if (ballA->applyBreak(tableBallDeltaV, toBeAdded)) {
            // mark this ball to be deleted
            toBeRemoved.push_back(ballA);
            incrementShake();
            // nullify this ball
            *it = nullptr;

            continue;
        }

        // check whether ball should be swallowed
        if (m_table->sinks(ballA))
        {
            if(ballA == m_balls->front())
            {
                ballA->setVelocity(QVector2D(0,0));
                ballA->setPosition(QVector2D(150,250));
                break;
            }
            else if(ballA->getColour() == QColor("black"))
            {
                m_blackSunk = true;
            }
            else if(ballA->getColour() == QColor("red"))
            {
                m_redBallsSunk++;
            }
            else if(ballA->getColour() == QColor("yellow"))
            {

                m_yellowBallsSunk++;
            }
            // defer swallowing until later (messes iterators otherwise)
            toBeRemoved.push_back(ballA);
            // nullify this ball
            *it = nullptr;
            continue;
        }

        // check collision with all later balls
        for (auto nestedIt = it + 1; nestedIt != m_balls->end(); ++nestedIt) {
            Ball* ballB = *nestedIt;
            if (ballB == nullptr) continue;
            if (isColliding(ballA, ballB)) {
                // retrieve the changes in velocities for each ball and resolve collision
                QVector2D ballADeltaV,ballBDeltaV;
                std::tie(ballADeltaV, ballBDeltaV) = resolveCollision(ballA, ballB);

                // add screenshake, remove ball, and add children to table vector if breaking
                if (ballA->applyBreak(ballADeltaV, toBeAdded)) {
                    toBeRemoved.push_back(ballA);
                    incrementShake();
                    // nullify this ball
                    *it = nullptr;
                    break;
                }
                // add screenshake, remove ball, and add children to table vector if breaking
                if (ballB->applyBreak(ballBDeltaV, toBeAdded)) {
                    toBeRemoved.push_back(ballB);
                    incrementShake();
                    // nullify this ball
                    *nestedIt = nullptr;
                    continue;
                }
            }
        }
        // we marked this ball as deleted, so skip
        if (*it == nullptr) continue;

        // move ball due to speed
        ballA->translate(ballA->getVelocity() * dt);
        // apply friction
        ballA->changeVelocity(-ballA->getVelocity() * m_table->getFriction() * dt);
    }

    // clean up them trash-balls
    for (Ball* b : toBeRemoved) {
        delete b;
        // delete all balls marked with nullptr
        m_balls->erase(std::find(m_balls->begin(), m_balls->end(), nullptr));
    }
    for (Ball* b: toBeAdded) m_balls->push_back(b);
    updateShake(dt);
}

void Game::updateShake(double dt) {
    m_shakeRadius *= (1-dt)*0.9;
    m_shakeAngle += (150 + rand()%60);
    m_screenshake = QVector2D(sin(m_shakeAngle)*m_shakeRadius, cos(m_shakeAngle)*m_shakeRadius);
}

QVector2D Game::resolveCollision(const Table* table, Ball* ball) {
    QVector2D bPos = ball->getPosition();

    QVector2D startingVel = ball->getVelocity();

    // resulting multiplicity of direction. If a component is set to -1, it
    // will flip the velocity's corresponding component
    QVector2D vChange(1,1);

    // ball is beyond left side of table's bounds
    if (bPos.x() - ball->getRadius() <= 0) {
        // flip velocity if wrong dir
        if (ball->getVelocity().x() <= 0) vChange.setX(-1);
    // ball is beyond right side of table's bounds
    } else if (bPos.x() + ball->getRadius() >= 0 + table->getWidth()) {
        // flip velocity if wrong dir
        if (ball->getVelocity().x() >= 0) vChange.setX(-1);
    }
    // ball is above top of the table's bounds
    if (bPos.y() - ball->getRadius() <= 0) {
        // flip iff we're travelling in the wrong dir
        if (ball->getVelocity().y() <= 0) vChange.setY(-1);
    // ball is beyond bottom of table's bounds
    } else if (bPos.y() + ball->getRadius() >= 0 + table->getHeight()) {
        // if we're moving down (we want to let the ball bounce up if its heading back)
        if (ball->getVelocity().y() >= 0) vChange.setY(-1);
    }

    ball->multiplyVelocity(vChange);

    // return the change in velocity
    return ball->getVelocity() - startingVel;
}

std::pair<QVector2D, QVector2D> Game::resolveCollision(Ball* ballA, Ball* ballB) {
    // SOURCE : ASSIGNMENT SPEC

    // if not colliding (i.e. distance is larger than radius)
    QVector2D collisionVector = ballB->getPosition() - ballA->getPosition();
    if (collisionVector.length() > ballA->getRadius() + ballB->getRadius()) {
       throw std::logic_error("attempting to resolve collision of balls that do not touch");
    }
    collisionVector.normalize();

    QVector2D ballAStartingVelocity = ballA->getVelocity();
    QVector2D ballBStartingVelocity = ballB->getVelocity();

    float mr = ballB->getMass() / ballA->getMass();
    double pa = QVector2D::dotProduct(collisionVector, ballA->getVelocity());
    double pb = QVector2D::dotProduct(collisionVector, ballB->getVelocity());

    if (pa <= 0 && pb >= 0) return std::make_pair(QVector2D(0,0), QVector2D(0,0));

    double a = -(mr + 1);
    double b = 2*(mr * pb + pa);
    double c = -((mr - 1)*pb*pb + 2*pa*pb);
    double disc = sqrt(b*b - 4*a*c);
    double root = (-b + disc)/(2*a);
    if (root - pb < 0.01) {
        root = (-b - disc)/(2*a);
    }

    ballA->changeVelocity(mr * (pb - root) * collisionVector);
    ballB->changeVelocity((root-pb) * collisionVector);

    // return the change in velocities for the two balls
    return std::make_pair(ballA->getVelocity() - ballAStartingVelocity, ballB->getVelocity() - ballBStartingVelocity);
}
