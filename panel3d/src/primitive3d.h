#ifndef PRIMITIVE3D_H
#define PRIMITIVE3D_H

#include "object3d.h"
#include <QVector>

class Primitive3D : public Object3D
{
    Q_OBJECT

public:
    typedef enum
    {
        plane,
        box,
        sphere,
        cylinder,
        cone,
        polygon
    } Type;

private:
    Type ptype;
    qreal data[6];
    QVector<QPointF> mPoints;
    int pstacks;
    int pslices;

protected:
    virtual void draw();
    QVector3D getMinBounds();
    QVector3D getMaxBounds();

public:
    explicit Primitive3D(QObject *parent = 0);
    Primitive3D(Type type, QObject *parent = 0);

    void setPlane(QVector3D vectorX, QVector3D vectorY);
    void setBox(qreal x, qreal y, qreal z);
    void setSphere(qreal radius);
    void setCylinder(qreal radius, qreal height);
    void setCone(qreal baseRadius, qreal topRadius, qreal height);
    void setPolygon(QVector<QPointF> points);

    void setDetalization(int stacks, int slices);

signals:

public slots:

};

#endif // PRIMITIVE3D_H
