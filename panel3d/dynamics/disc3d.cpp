#include "disc3d.h"

Disc3D::Disc3D(Object3D *parent) :
    Primitive3D(parent),
    mDensity(1000),
    mRadius(0.5),
    mHeight(0.05)
{
//    mAxe = new Primitive3D(this);
//    mAxe->setCylinder(5, 50);
//    mAxe->setColor(Qt::blue);
    calcMassInertia();
}


void Disc3D::calcMassInertia()
{
//    QVector3D a = Primitive3D::center() * 0.01;
    qreal R = mRadius;
    qreal H = mHeight;
    qreal V = M_PI * R * R * H;
    qreal m = V * mDensity;

    QMatrix4x4 S;
    S.rotate(orient().x(), 1, 0, 0);
    S.rotate(orient().y(), 0, 1, 0);
    S.rotate(orient().z(), 0, 0, 1);

    QMatrix4x4 J;
    J(0, 0) = m * (3*R*R + H*H) / 12;
    J(1, 1) = J(0, 0);
    J(2, 2) = m * (R*R) / 2;

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

    J = S * J * S.transposed();

    setMass(m);
    setInertialTensor(J);
}

void Disc3D::draw()
{
    Primitive3D::setPosition(position() * 100);
    Primitive3D::setRotation(rotation());
    Primitive3D::draw();

//    QVector3D w = angularVelocity() * 10;
//    glLineWidth(5.0);
//    glEnable(GL_COLOR_MATERIAL);
//    glColor3f(1.0, 0, 0);
//    glBegin(GL_LINES);
//    glVertex3f(0, 0, 0);
//    glVertex3f(0, 0, 50);
//    glEnd();
//    glColor3f(0, 0, 1.0);
//    glBegin(GL_LINES);
//    glVertex3f(0, 0, 0);
//    glVertex3f(w.x(), w.y(), w.z());
//    glEnd();
//    glDisable(GL_COLOR_MATERIAL);
}

//void Disc3D::integrateStep(qreal h)
//{
//    DynamicModel::integrateStep(h);

////    Primitive3D::setPosition(mPosition * 100);
////    Primitive3D::setRotation(mRotation);
////    QQuaternion q();
////    mAxe->setRotation(q);
//}

void Disc3D::setDensity(qreal density_kg_m3)
{
    mDensity = density_kg_m3;
    calcMassInertia();
}

void Disc3D::setSize(qreal radius_m, qreal height_m)
{
    mRadius = radius_m;
    mHeight = height_m;
    setCylinder(mRadius*100, mHeight*100);
    setCenter(0, 0, -mHeight*100/2);
    calcMassInertia();
}

void Disc3D::setOrient(qreal x, qreal y, qreal z)
{
    Primitive3D::setOrient(x, y, z);
    calcMassInertia();
}

//void Disc3D::setCenter(qreal Cx_m, qreal Cy_m, qreal Cz_m)
//{
//    Primitive3D::setCenter(Cx_m*100, Cy_m*100, Cz_m*100);
//    calcMassInertia();
//}
