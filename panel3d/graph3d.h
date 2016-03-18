#ifndef GRAPH3D_H
#define GRAPH3D_H

#include "object3d.h"
#include <QtGlobal>
#include <QList>

class Graph3D : public Object3D
{
    Q_OBJECT

public:
    typedef enum {tNone=0, tPoints=1, tLines=2, tPointsLines=3} Type;
    typedef enum
    {
        dtXYZ=0,
        dtXT,  dtYT,  dtZT,
        dtXY,  dtYZ,  dtZX,  dtYX,  dtZY,  dtXZ,
        dtXYT, dtYZT, dtZXT, dtYXT, dtZYT, dtXZT
    } DrawType;

private:
    QList<QVector3D> Points;
    QVector3D min;
    QVector3D max;
    float deltaTime;
    int pShiftCount;

    bool pPointsVisible;
    bool pLinesVisible;
    float pPointSize;
    float pLineWidth;
    DrawType pDrawType;

protected:
    virtual void draw();
    void drawPoints();
    QVector3D getMinBounds();
    QVector3D getMaxBounds();

public:
    explicit Graph3D(QObject *parent = 0);
    void addPoint(QVector3D point);
    void addX(float value) {addPoint(QVector3D(value, 0, 0));}
    void addY(float value) {addPoint(QVector3D(0, value, 0));}
    void addZ(float value) {addPoint(QVector3D(0, 0, value));}
    void addXY(float x, float y) {addPoint(QVector3D(x, y, 0));}
    void addYZ(float y, float z) {addPoint(QVector3D(0, y, z));}
    void addZX(float z, float x) {addPoint(QVector3D(x, 0, z));}
    void addXYZ(float x, float y, float z) {addPoint(QVector3D(x, y, z));}

    void pushPoint(QVector3D point);
    void pushX(float value) {pushPoint(QVector3D(value, 0, 0));}
    void pushY(float value) {pushPoint(QVector3D(0, value, 0));}
    void pushZ(float value) {pushPoint(QVector3D(0, 0, value));}
    void pushXY(float x, float y) {pushPoint(QVector3D(x, y, 0));}
    void pushYZ(float y, float z) {pushPoint(QVector3D(0, y, z));}
    void pushZX(float z, float x) {pushPoint(QVector3D(x, 0, z));}
    void pushXYZ(float x, float y, float z) {pushPoint(QVector3D(x, y, z));}

    void clear();

    Type type() const {return pPointsVisible? (pLinesVisible? tPointsLines: tPoints): (pLinesVisible? tLines: tNone);}
    void setType(Type type) {pPointsVisible = (type==tPoints) || (type==tPointsLines);
                             pLinesVisible = (type==tLines) || (type==tPointsLines);
                             emit changed();}
    DrawType drawType() const {return pDrawType;}
    void setDrawType(DrawType drawType) {pDrawType = drawType; emit changed();}
    float pointSize() const {return pPointSize;}
    void setPointSize(float size) {pPointSize = size; emit changed();}
    float lineWidth() const {return pLineWidth;}
    void setLineWidth(float width) {pLineWidth = width; emit changed();}

    int shiftCount() const {return pShiftCount;}
    void setShiftCount(int value) {pShiftCount = value;}
    float timeZoom() const {return 1.0 / deltaTime;}
    void setTimeZoom(float timeZoom) {deltaTime = timeZoom? 1.0 / timeZoom: 1;}

signals:

public slots:

};

#endif // GRAPH3D_H
