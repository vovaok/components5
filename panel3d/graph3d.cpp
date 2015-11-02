#include "graph3d.h"

Graph3D::Graph3D(QObject *parent) :
    Object3D(parent)
{
    pPointsVisible = true;
    pLinesVisible = false;
    pPointSize = 3;
    pLineWidth = 2;
    pDrawType = dtXYZ;
    pShiftCount = 100;
    deltaTime = 1;
}
//--------------------------------------------------------------------------

void Graph3D::draw()
{
    glPointSize(pPointSize);
    glLineWidth(pLineWidth);
    if (pPointsVisible)
    {
        glBegin(GL_POINTS);
        drawPoints();
        glEnd();
    }
    if (pLinesVisible)
    {
        glBegin(GL_LINE_STRIP);
        drawPoints();
        glEnd();
    }
}

void Graph3D::drawPoints()
{
    float time = 0;
    foreach (QVector3D p, Points)
    {
        QVector3D v;
        switch (pDrawType)
        {
            case dtXYZ: v = p; break;
            case dtXT: v = QVector3D(time, p.x(), 0); break;
            case dtYT: v = QVector3D(time, p.y(), 0); break;
            case dtZT: v = QVector3D(time, p.z(), 0); break;
            case dtXY: v = QVector3D(p.x(), p.y(), 0); break;
            case dtYZ: v = QVector3D(p.y(), p.z(), 0); break;
            case dtZX: v = QVector3D(p.z(), p.x(), 0); break;
            case dtYX: v = QVector3D(p.y(), p.x(), 0); break;
            case dtZY: v = QVector3D(p.z(), p.y(), 0); break;
            case dtXZ: v = QVector3D(p.x(), p.z(), 0); break;
            case dtXYT: v = QVector3D(time, p.x(), p.y()); break;
            case dtYZT: v = QVector3D(time, p.y(), p.z()); break;
            case dtZXT: v = QVector3D(time, p.z(), p.x()); break;
            case dtYXT: v = QVector3D(time, p.y(), p.x()); break;
            case dtZYT: v = QVector3D(time, p.z(), p.y()); break;
            case dtXZT: v = QVector3D(time, p.x(), p.z()); break;
            default: v = p;
        }
        GLfloat pt[3] = {v.x(), v.y(), v.z()};
        glVertex3fv(pt);
        time += deltaTime;
    }
}
//--------------------------------------------------------------------------

void Graph3D::clear()
{
    min = QVector3D(0, 0, 0);
    max = QVector3D(0, 0, 0);
    Points.clear();
    emit changed();
}
//--------------------------------------------------------------------------

void Graph3D::addPoint(QVector3D point)
{
    min.setX(qMin(point.x(), min.x()));
    min.setY(qMin(point.y(), min.y()));
    min.setZ(qMin(point.z(), min.z()));
    max.setX(qMax(point.x(), max.x()));
    max.setY(qMax(point.y(), max.y()));
    max.setZ(qMax(point.z(), max.z()));
    Points << point;
    emit changed();
}

void Graph3D::pushPoint(QVector3D point)
{
    if (Points.count() >= pShiftCount)
        Points.removeFirst();
    addPoint(point);
}
//--------------------------------------------------------------------------

QVector3D Graph3D::getMaxBounds()
{
    return max;
}

QVector3D Graph3D::getMinBounds()
{
    return min;
}
//--------------------------------------------------------------------------
