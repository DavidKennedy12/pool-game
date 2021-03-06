#pragma once

#include <QColor>
#include <QPainter>

#include "pocket.h"

class Ball;

class Table {
protected:
    int m_width;
    int m_height;
    QBrush m_brush;
    double m_friction;
public:
    virtual ~Table() {}
    Table(int width, int height, QColor colour, double friction) :
        m_width(width), m_height(height),
        m_brush(colour), m_friction(friction) {}


    Table(const Table &other)
        : m_width(other.m_width), m_height(other.m_height), m_brush(other.m_brush), m_friction(other.m_friction)
    {}

    //clone method
    virtual Table * clone () const = 0;

    /**
     * @brief render - draw the table to screen using the specified painter
     * @param painter - painter to use
     */
    virtual void render(QPainter& painter, const QVector2D& offset) = 0;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    double getFriction() const { return m_friction; }

    virtual bool sinks(Ball*) { return false; }
};

class StageOneTable : public Table
{
public:
    StageOneTable(int width, int height, QColor colour, double friction) :
        Table(width, height, colour, friction) {}

    StageOneTable(const StageOneTable &other)
        : Table(other)
    {}

    StageOneTable * clone () const override
    {
         return new StageOneTable(*this);
    }
    /**
     * @brief render - draw the stageonetable to screen using the specified painter
     * @param painter - painter to use
     */
    void render(QPainter &painter, const QVector2D& offset) override;
};

class StageTwoTable : public Table {
protected:
    std::vector<Pocket*> m_pockets;

public:
    StageTwoTable(int width, int height, QColor colour, double friction) :
        Table(width, height, colour, friction) {}

    StageTwoTable(const StageTwoTable &other)
        : Table(other)
    {
        //add pockets

        for (auto p = other.m_pockets.begin(); p != other.m_pockets.end(); ++p)
        {
            Pocket* pocketA = *p;
            m_pockets.push_back(pocketA->clone());
        }
    }

    StageTwoTable * clone () const override
    {
         return new StageTwoTable(*this);
    }

    ~StageTwoTable();

    /**
     * @brief render - draw the stageonetable to screen using the specified painter
     * @param painter - painter to use
     */
    void render(QPainter &painter, const QVector2D& offset) override;

    // sinky winky ball
    virtual bool sinks(Ball* b) override;

    /* self explanatory */
    void addPocket(Pocket* p) { m_pockets.push_back(p); }
};
