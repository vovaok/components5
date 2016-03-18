#include "joint3d.h"

Joint3D::Joint3D(DynamicModel *model1, DynamicModel *model2) :
    mModel1(model1),
    mModel2(model2),
    jz1(0, 0, 1), jz2(0, 0, 1),
    jx1(1, 0, 0), jx2(1, 0, 0),
    mQ(0), mQold(0), mDq(0), mQinit(0),
    mSign(1),
    kF_x(5e9), kF_v(1000), //50000
    kM_angle(1e6), kMv_normal(7), kMv_collinear(0.3) //1000
{
}

void Joint3D::setRotationAxes(QVector3D model1_axis, QVector3D model2_axis)
{
    jz1 = model1_axis.normalized();
    jz2 = model2_axis.normalized();

    //        jx1 = jz1.z()>0? QVector3D(1, 0, 0): jz1.z()<0? QVector3D(-1, 0, 0): jz1.x()>0? QVector3D(0, 0, -1): jz1.x()<0? QVector3D(0, 0, 1): QVector3D(1, 0, 0);
    //        jx2 = jz2.z()>0? QVector3D(1, 0, 0): jz2.z()<0? QVector3D(-1, 0, 0): jz2.x()>0? QVector3D(0, 0, -1): jz2.x()<0? QVector3D(0, 0, 1): QVector3D(1, 0, 0);

    QVector3D jy1(0, 1, 0);
    if (QVector3D::crossProduct(jy1, jz1).isNull())
        jy1.setX(23);
    jx1 = QVector3D::crossProduct(jy1, jz1).normalized();

    QVector3D jy2(0, 1, 0);
    if (QVector3D::crossProduct(jy2, jz2).isNull())
        jy2.setX(23);
    jx2 = QVector3D::crossProduct(jy2, jz2).normalized();

    solve();
}

void Joint3D::applyConstraints()
{
    QVector3D x1, v1, w1, x2, v2, w2;
    QMatrix3x3 T1, T2;

    if (mModel1)
    {
        x1 = mModel1->position();
        v1 = mModel1->velocity();
        T1 = mModel1->rotation();
        w1 = mModel1->angularVelocity();
    }
    QMatrix3x3 W1 = koso(w1);

    if (mModel2)
    {
        x2 = mModel2->position();
        v2 = mModel2->velocity();
        T2 = mModel2->rotation();
        w2 = mModel2->angularVelocity();
    }
    QMatrix3x3 W2 = koso(w2);

    QVector3D p1 = x1 + T1*r1;
    QVector3D p2 = x2 + T2*r2;

    QVector3D vp1 = v1 + T1*W1*r1;
    QVector3D vp2 = v2 + T2*W2*r2;

#warning vse v kube
    F = kF_x*(p2 - p1)*(p2 - p1)*(p2 - p1) + kF_v*(vp2 - vp1);

    ////    mExternalForce = -(Fg - m*(a*0.25 + T*(E + W*W)*(rC-0.25*r)));// + 100*(0*r-x-T*r) + 200*(-v-T*W*r); // 10, 2000; // 10000, 1000
    ////    mExternalForce = -(Fg - m*((a-T*E*r - T*W*W*r)*0.5 + T*(E + W*W)*rC));
    //    mExternalForce = 5000*(r-x-T*r) + 200*(-v-T*W*r);
    ////    mExternalForce.setX(0);

    //QVector3D jz1(0, 1, 0);
    QVector3D ja1 = QVector3D(1, 1, 1) - jz1;
    QVector3D jc1 = jz1 * kMv_collinear + ja1 * kMv_normal;
    //QVector3D jz2(0, 1, 0);
    QVector3D ja2 = QVector3D(1, 1, 1) - jz2;
    QVector3D jc2 = jz2 * kMv_collinear + ja2 * kMv_normal;

//    QMatrix3x3 T1_1 = invert(T1);
//    QMatrix3x3 T2_1 = invert(T2);
    QVector3D j1 = T1 * jz1;
    QVector3D j2 = T2 * jz2;

    QVector3D ix1 = T1 * jx1;
    QVector3D ix2 = T2 * jx2;
    float q_temp = atan2(QVector3D::dotProduct(j1, QVector3D::crossProduct(ix1, ix2)), QVector3D::dotProduct(ix1, ix2));
    float dq = q_temp - mQold;
    if (dq > M_PI)
        dq -= 2*M_PI;
    else if (dq < -M_PI)
        dq += 2*M_PI;
    mQold = q_temp;
    mDq = dq;
    mQ += mDq;

    QVector3D cp = QVector3D::crossProduct(j1, j2);
    M = kM_angle*cp*cp*cp + (T2*(w2*jc2) - (T1*(w1*jc1)));

    if (mModel1)
    {
        mModel1->addExternalForce(F, r1);
        mModel1->addExternalMomentum(M);
    }
    if (mModel2)
    {
        mModel2->addExternalForce(-F, r2);
        mModel2->addExternalMomentum(-M);
    }
}

void Joint3D::solve()
{
    QVector3D x1, x2;
    QMatrix3x3 T1, T2;

    if (mModel1)
    {
        x1 = mModel1->position();
        T1 = mModel1->rotation();
    }

    if (mModel2)
    {
        x2 = mModel2->position();
        T2 = mModel2->rotation();
        QVector3D j1 = T1 * jz1;
        QVector3D j2 = T2 * jz2;
        QVector3D rot = QVector3D::crossProduct(j2, j1);
        float angle = atan2(rot.length(), QVector3D::dotProduct(j2, j1)) * 180 / M_PI;
        if (fabs(angle) > 0.001)
        {
            rot.normalize();
            rot = T2.transposed() * rot;
            if (rot.length() < 0.00001)
                rot = T1*jx1;//QVector3D(0, 0, 0);
            mModel2->rotate(angle, rot.x(), rot.y(), rot.z());
            T2 = mModel2->rotation();
        }
        QVector3D ix1 = T2.transposed() * T1 * jx1;
        QVector3D ix2 = jx2;
        QMatrix3x3 mrot = rotationMatrix(mSign*mQinit*M_PI/180, jz2);
        ix2 = mrot*ix2;
        QVector3D cross = QVector3D::crossProduct(ix1, ix2);
        float q_temp = -atan2(cross.length(), QVector3D::dotProduct(ix1, ix2)) * 180/M_PI;
        if (q_temp)
        {
            rot = cross.normalized();
            if (rot.length() < 0.00001)
                rot = jz2;
            mModel2->rotate(q_temp, rot.x(), rot.y(), rot.z());
            T2 = mModel2->rotation();
        }
        mModel2->setPosition(x1 + T1*r1 - T2*r2);
    }
}
