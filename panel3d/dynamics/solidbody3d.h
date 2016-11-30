#ifndef SOLIDBODY3D_H
#define SOLIDBODY3D_H

#include "dynamicmodel.h"
#include "mesh3d.h"

class SolidBody3D : public DynamicModel, public Mesh3D
{
private:
    QMatrix3x3 J0;

protected:
    virtual void draw() override;

public:
    explicit SolidBody3D(Object3D *parent=nullptr);

    void setMassInertia(float mass, QMatrix3x3 J);
    void setMassInertia(float mass, float Jx, float Jy, float Jz, float Jxy, float Jxz, float Jyz);

    void setOrient(qreal x, qreal y, qreal z);

//    using Mesh3D::Object3D;
//    using Mesh3D::Object3D::QObject;
//    using Mesh3D::parent;
//    using Mesh3D::setColor;
//    using Mesh3D::showAxes;
//    using Mesh3D::orient;
//    using Mesh3D::setVisible;
//    using Mesh3D::isVisible;
//    using Mesh3D::setCenter;
//    using Mesh3D::transformMatrix;
//    using Mesh3D::fullTransformMatrix;

//    using Mesh3D::loadModel;

    using DynamicModel::setPosition;
    using DynamicModel::rotation;
    using DynamicModel::rotate;
    using DynamicModel::setRotation;
    using DynamicModel::integrateStep;
};

#endif // SOLIDBODY3D_H
