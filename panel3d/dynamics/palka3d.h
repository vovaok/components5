#ifndef PALKA3D_H
#define PALKA3D_H

#include "dynamicmodel.h"
#include "primitive3d.h"

class Palka3D : public DynamicModel, private Primitive3D
{
private:
    qreal mDensity;
    QVector3D mSize;

    void calcMassInertia();

protected:
    virtual void draw() override;
//    virtual void integrateStep(qreal h) override;

public:
    Palka3D(Object3D *parent=nullptr);

    void setDensity(qreal density_kg_m3);
    void setSize(qreal Lx_m, qreal Ly_m, qreal Lz_m);
//    void setCenter(qreal Cx_m, qreal Cy_m, qreal Cz_m);
    void setOrient(qreal x, qreal y, qreal z);

    using Primitive3D::Object3D;
    using Primitive3D::Object3D::QObject;
    using Primitive3D::parent;
    using Primitive3D::setColor;
    using Primitive3D::showAxes;
    using Primitive3D::orient;
    using Primitive3D::setVisible;
    using Primitive3D::isVisible;
    using DynamicModel::setPosition;
    using DynamicModel::rotation;
    using DynamicModel::rotate;
    using DynamicModel::setRotation;
    using DynamicModel::integrateStep;
};

#endif // PALKA3D_H
