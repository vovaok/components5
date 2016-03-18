#include "palka3d.h"

Palka3D::Palka3D(Object3D *parent) :
    Primitive3D(parent),
    mDensity(1000),
    mSize(1.0, 0.05, 0.05)
{
    setSize(mSize.x(), mSize.y(), mSize.z());
}

void Palka3D::calcMassInertia()
{
    QVector3D a;// = Primitive3D::center() * 0.01;
    QMatrix4x4 S;
    S.rotate(orient().x(), 1, 0, 0);
    S.rotate(orient().y(), 0, 1, 0);
    S.rotate(orient().z(), 0, 0, 1);



    //QVector3D Cm = mSize / 2;
    qreal Lx = mSize.x();
    qreal Ly = mSize.y();
    qreal Lz = mSize.z();
    qreal V = Lx * Ly * Lz;
    qreal m = V * mDensity;

    QMatrix4x4 J;
    J(0, 0) = m * (Ly*Ly + Lz*Lz) / 12;
    J(1, 1) = m * (Lx*Lx + Lz*Lz) / 12;
    J(2, 2) = m * (Lx*Lx + Ly*Ly) / 12;

//    qreal a2 = a.lengthSquared();
//    J(0, 0) += m * (a2 - a.x()*a.x());
//    J(0, 1) += m * (-a.x()*a.y());
//    J(0, 2) += m * (-a.x()*a.z());
//    J(1, 1) += m * (a2 - a.y()*a.y());
//    J(1, 2) += m * (-a.y()*a.z());
//    J(2, 2) += m * (a2 - a.z()*a.z());
//    J(1, 0) = J(0, 1);
//    J(2, 0) = J(0, 2);
//    J(2, 1) = J(1, 2);

    //J = S.transposed() * J * S;

    a = S*a;

    J = S * J * S.transposed();

    setMass(m);
    setInertialTensor(J);
//    setCenterOfMass(a);
}

void Palka3D::draw()
{
    Primitive3D::setPosition(position() * 100);
    Primitive3D::setRotation(rotation());
    Primitive3D::draw();

//    QVector3D w = angularVelocity() * 10;
//    glEnable(GL_COLOR_MATERIAL);
//    glColor3f(1.0, 0, 0);

//    glPushMatrix();

////    glPointSize(15);
////    glBegin(GL_POINTS);
////    QVector3D cc = rcpos*100;
////    glVertex3f(cc.x(), cc.y(), cc.z());
////    glEnd();

//    glRotatef(-orient().z(), 0, 0, 1);
//    glRotatef(-orient().y(), 0, 1, 0);
//    glRotatef(-orient().x(), 1, 0, 0);
//    glLineWidth(5.0);
//    glBegin(GL_LINES);
//    glVertex3f(0, 0, 0);
//    glVertex3f(50, 0, 0);
//    glEnd();
//    glColor3f(0, 0, 1.0);
//    glBegin(GL_LINES);
//    glVertex3f(0, 0, 0);
//    glVertex3f(w.x(), w.y(), w.z());
//    glEnd();

//    glDisable(GL_COLOR_MATERIAL);
//    glPopMatrix();
}

//void Palka3D::integrateStep(qreal h)
//{
//    DynamicModel::integrateStep(h);
////    Primitive3D::setPosition(mPosition * 100);
//////    Primitive3D::setRotation(mRotation);
//////    Primitive3D::setTransform(mTransform.constData());
////    Primitive3D::setRotation(mTransform);
//}
//---------------------------------------------------------

void Palka3D::setDensity(qreal density_kg_m3)
{
    mDensity = density_kg_m3;
    calcMassInertia();
}

void Palka3D::setSize(qreal Lx_m, qreal Ly_m, qreal Lz_m)
{
    mSize.setX(Lx_m);
    mSize.setY(Ly_m);
    mSize.setZ(Lz_m);
    setBox(100*Lx_m, 100*Ly_m, 100*Lz_m);
    calcMassInertia();
}

//void Palka3D::setCenter(qreal Cx_m, qreal Cy_m, qreal Cz_m)
//{
//    Primitive3D::setCenter(Cx_m*100, Cy_m*100, Cz_m*100);
//    calcMassInertia();
//}

void Palka3D::setOrient(qreal x, qreal y, qreal z)
{
    Primitive3D::setOrient(x, y, z);
    calcMassInertia();
}
//---------------------------------------------------------
