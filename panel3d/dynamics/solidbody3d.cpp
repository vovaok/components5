#include "solidbody3d.h"

SolidBody3D::SolidBody3D(Object3D *parent) :
    Mesh3D(parent)
{
}

void SolidBody3D::draw()
{
    Mesh3D::setPosition(position() * 100);
    Mesh3D::setRotation(rotation());
    Mesh3D::draw();
}

void SolidBody3D::setMassInertia(float mass, QMatrix3x3 J)
{
    J0 = J;
    QMatrix3x3 T;
    T = T * rotationMatrix(orient().x(), QVector3D(1, 0, 0));
    T = T * rotationMatrix(orient().x(), QVector3D(1, 0, 0));
    T = T * rotationMatrix(orient().x(), QVector3D(1, 0, 0));
    J = T * J0 * T.transposed();
    setMass(mass);
    setInertialTensor(J);
}

void SolidBody3D::setMassInertia(float mass, float Jx, float Jy, float Jz, float Jxy, float Jxz, float Jyz)
{
    J0(0,0) = Jx;    J0(0,1) = Jxy;   J0(0,2) = Jxz;
    J0(1,0) = Jxy;   J0(1,1) = Jy;    J0(1,2) = Jyz;
    J0(2,0) = Jxz;   J0(2,1) = Jyz;   J0(2,2) = Jz;
    setMassInertia(mass, J0);
}

void SolidBody3D::setOrient(qreal x, qreal y, qreal z)
{
    Mesh3D::setOrient(x, y, z);
    setMassInertia(mass(), J0);
}
