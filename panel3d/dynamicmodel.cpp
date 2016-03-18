#include "dynamicmodel.h"

QVector3D DynamicModel::mGravity = QVector3D(0, 0, -9.81);

DynamicModel::DynamicModel() :
//    mPitch(0), mYaw(0), mRoll(0),
    mCalcEulerAngles(false),
    mMass(1.0),
    Kupr(0),
    mBounceRadius(1.0),
    mCollisionState(false),
    mKtr(0), mKrot(0), mQpot(0)
{
}

void DynamicModel::integrate(qreal dt, PhaseVector *result)
{
    integrateStep(dt, result);
    mCollisionState = false;
    mExternalForce = QVector3D();
    mExternalMomentum = QVector3D();
}

void DynamicModel::reset()
{
    mP.x = QVector3D();
    mP.v = QVector3D();
    mP.a = QVector3D();
//    mRotation = QQuaternion();
    mP.T.setToIdentity();
    mP.w = QVector3D();
    mP.e = QVector3D();
    mForce = QVector3D();
    mMomentum = QVector3D();
    mExternalForce = QVector3D();
    mExternalMomentum = QVector3D();
}

void DynamicModel::addExternalForce(QVector3D F, QVector3D r)
{
    r = mP.T * r;
    mExternalForce += F;
    mExternalMomentum += QVector3D::crossProduct(r, mExternalForce);
}

void DynamicModel::integrateStep(qreal h, PhaseVector *result)
{
    if (mCollisionState)
        collisionHandler(h);

    float m = mMass;
    QVector3D &Fg = mForceOfGravity;
//    QVector3D rC = mCenterOfMass;
    QMatrix3x3 &J = mInertialTensor;
    QMatrix3x3 &J_1 = mInertialTensorInv;

    PhaseVector *p = result? result: &mP;

    QVector3D &F = mForce;
    QVector3D &M = mMomentum;

//    QVector3D w_ = w + e*h/2;
//    QMatrix3x3 W = koso(w_);

    //QMatrix3x3 &T = mTransform;
    QMatrix3x3 T_1 = p->T.transposed();

    F = mExternalForce;
    M = mExternalMomentum;

    p->e = J_1 * (T_1 * M - QVector3D::crossProduct(mP.w, J * mP.w));
    p->w = mP.w + p->e * h;
    if (p->w.lengthSquared() > 10000)
        p->T = p->T * rotationMatrix(p->w.length()*h, p->w.normalized());
    else
        p->T = mP.T * rotat(p->w * h);
//        mTransform.setColumn(0, mTransform.column(0).normalized());
//        mTransform.setColumn(1, mTransform.column(1).normalized());
//        mTransform.setColumn(2, mTransform.column(2).normalized());

    p->a = (F + Fg) / m;
    p->v = mP.v + p->a * h;
    p->x = mP.x + p->v * h;

    mKtr = m * p->v.lengthSquared() / 2;
    mKrot = QVector3D::dotProduct(p->w, J*p->w) / 2;
    mQpot = -QVector3D::dotProduct(Fg, (p->x));// + T*rC));
}

//void DynamicModel::integrateStep(qreal h)
//{
//    if (mCollisionState)
//        collisionHandler(h);

//    QVector3D &g = mGravity;
//    float m = mMass;
//    QVector3D Fg = m * g;
//    QVector3D rC = mCenterOfMass;
//    QMatrix3x3 J = mInertialTensor.toGenericMatrix<3,3>();
//    QMatrix3x3 J_1 = mInertialTensorInv.toGenericMatrix<3,3>();
//    QVector3D &x = mPosition;
//    QVector3D &v = mVelocity;
//    QVector3D &a = mAcceleration;
////    QQuaternion &q = mRotation;
//    QVector3D &w = mAngularVelocity;
//    QVector3D &e = mAngularAcceleration;
//    QVector3D &F = mForce;
//    QVector3D &M = mMomentum;

////    QVector3D rCold = rC;
////    rC = mRotation.rotatedVector(rC);
//////    rC = mTransform * rC;
////    rcpos = rC;

////    F = Fg + mExternalForce;
////    M = QVector3D::crossProduct(rC, Fg) - QVector3D::crossProduct(rC, m*a) + mExternalMomentum;

////    M = mRotation.conjugate().rotatedVector(M);
//////    M = mTransform.inverted() * M;

////    // Runge-Kutta order 4
//////    QVector3D k1 = J_1 * (M - QVector3D::crossProduct(w, J*w));
//////    QVector3D k2 = J_1 * (M - QVector3D::crossProduct(w + h/2*k1, J*(w+h/2*k1)));
//////    QVector3D k3 = J_1 * (M - QVector3D::crossProduct(w + h/2*k2, J*(w+h/2*k2)));
//////    QVector3D k4 = J_1 * (M - QVector3D::crossProduct(w + h*k3, J*(w+h*k3)));
//////    w = w + h/6*(k1 + 2*k2 + 2*k3 + k4);

////    QVector3D w_ = w + e*h/2;
////    e = J_1 * (M - QVector3D::crossProduct(w_, J*w_));
////    w += e * h;

////    // m * a.C = F;
////    // m * (a + e*rC + w*w*rC) = F
////    // a = F/m - e*rC - w*w*rC;

////    QVector3D FinnTheHuman = QVector3D::crossProduct(e, rC) - QVector3D::crossProduct(w_, QVector3D::crossProduct(w_, rC));

////    a = F / m + FinnTheHuman;
//////    a = F / m + QVector3D::crossProduct(e, rC) - QVector3D::crossProduct(w_, QVector3D::crossProduct(w_, rC));


//    QVector3D w_ = w + e*h/2;

////    QMatrix3x3 Rc = koso(rC);
////    QMatrix3x3 Ja_1 = invert(J + m*Rc*Rc);

//    QMatrix3x3 W = koso(w_);
//    QMatrix3x3 E = koso(e);


//    QMatrix3x3 T = mTransform.toGenericMatrix<3,3>();
//    QMatrix3x3 T_1 = mTransform.inverted().toGenericMatrix<3,3>();


////    QVector3D r(0, 0, 0.5);

//////    mExternalForce = -(Fg - m*(a*0.25 + T*(E + W*W)*(rC-0.25*r)));// + 100*(0*r-x-T*r) + 200*(-v-T*W*r); // 10, 2000; // 10000, 1000
//////    mExternalForce = -(Fg - m*((a-T*E*r - T*W*W*r)*0.5 + T*(E + W*W)*rC));
////    mExternalForce = 5000*(r-x-T*r) + 200*(-v-T*W*r);
//////    mExternalForce.setX(0);
//    F = mExternalForce;

////    r = T * r;
////    QMatrix3x3 R = koso(r);
////    mExternalMomentum += R * F;
////    QVector3D ja(1, 0, 1);
////    QVector3D OCbKA(0, 1, 0);
////    QVector3D jy = mTransform.column(1).toVector3D();
////    mExternalMomentum += 100*(-QVector3D::crossProduct(OCbKA, jy)) + 200*(-w*ja);

////    F = mExternalForce;
//    M = mExternalMomentum;

//    //QVector3D T1F = mRotation.conjugate().rotatedVector(F);

//    //QVector3D T1F = T_1 * F;

//    //e = Ja_1 * (M - W*J*w_ - Rc*T1F + m*Rc*W*W*rC); // (M(Fg) == 0) true story: no shoulder - no moment.
//    e = J_1 * (T_1*M - W*J*w_);
//    w += e * h;




//    E = koso(e);
////    W = koso(w);
//    a = (F + Fg) / m;
////    a = (F + Fg) / m - T*(E + W*W)*rC;
////    a = Ja_1 * (J * ((F+Fg)/m - W*W*rC) + Rc*(M + Rc*Fg - W*J*w));
//    v += a * h;
//    x += v * h;


//    if (!w.isNull())
//    {
//        mTransform.rotate(w.length()*h*180/M_PI, w.normalized());
//        mTransform.setColumn(0, mTransform.column(0).normalized());
//        mTransform.setColumn(1, mTransform.column(1).normalized());
//        mTransform.setColumn(2, mTransform.column(2).normalized());
////        q = q*QQuaternion::fromAxisAndAngle(w.normalized(), w.length()*180/M_PI * h);// * q;
//    }
////    q.normalize();


////    T = mTransform.toGenericMatrix<3,3>();
////    E = koso(e);
////    W = koso(w);
////    a = (F + Fg) / m - T*(E + W*W)*rC;
//////    a = Ja_1 * (J * ((F+Fg)/m - W*W*rC) + Rc*(M + Rc*Fg - W*J*w));
////    v += a * h;
////    x += v * h;


////    mTransform.setRow(3, QVector4D(x, 1));


////    qreal qx = q.x();
////    qreal qy = q.y();
////    qreal qz = q.z();
////    qreal qw = q.scalar();

////    mEx = QVector3D(1 - 2*qy*qy - 2*qz*qz, 2*qx*qy + 2*qz*qw, 2*qx*qz - 2*qy*qw);
////    mEy = QVector3D(2*qx*qy - 2*qz*qw, 1 - 2*qx*qx - 2*qz*qz, 2*qy*qz + 2*qx*qw);
////    mEz = QVector3D(2*qx*qz + 2*qy*qw, 2*qy*qz - 2*qx*qw, 1 - 2*qx*qx - 2*qy*qy);

////    if (mCalcEulerAngles)
////    {
////        mYaw = atan2f(2*qx*qy + 2*qz*qw, 1 - 2*qy*qy - 2*qz*qz);
////        mPitch = asinf(2*qy*qw + 2*qx*qz);
////        mRoll = atan2f(2*qx*qw - 2*qy*qz , 1 - 2*qx*qx - 2*qz*qz);
////    }

//    mKtr = m * v.lengthSquared() / 2;
//    mKrot = QVector3D::dotProduct(w, J*w) / 2;
////    mQpot = -m * QVector3D::dotProduct(g, (x + mRotation.rotatedVector(rC)));
//    mQpot = -m * QVector3D::dotProduct(g, (x + T*rC));
//}

void DynamicModel::collisionHandler(qreal h)
{
    qreal dot = QVector3D::dotProduct(mP.v, mCollisionPointNormal);
    if (dot > 0)
        dot = 0;
    QVector3D Vn = dot * mCollisionPointNormal; // normal velocity
    QVector3D dp = -Vn * mMass * (Kupr + 1); // impulse
    addExternalForce(dp/h, mCollisionPoint); //the point is reaction force application point
    dot = QVector3D::dotProduct(mForce, mCollisionPointNormal);
    if (dot > 0)
        dot = 0;
    QVector3D N = -dot * mCollisionPointNormal;
    addExternalForce(N, mCollisionPoint);
}

