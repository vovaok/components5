#ifndef DYNAMICMODEL_H
#define DYNAMICMODEL_H

#include <qmath.h>
#include <QVector3D>
#include <QQuaternion>
#include <QMatrix3x3>
#include <QMatrix4x4>

QMatrix3x3 invert(const QMatrix3x3 &m);
inline QMatrix3x3 rotationMatrix(double angle_rad, const QVector3D &v);
inline QVector3D operator*(const QMatrix3x3 &m, const QVector3D &v);

class DynamicModel
{
public:
    class PhaseVector
    {
    public:
        QVector3D a, v, x;
        QVector3D e, w;
        QMatrix3x3 T;

        PhaseVector operator *(float factor)
        {
            PhaseVector p;
            p.a = a * factor;
            p.v = v * factor;
            p.x = x * factor;
            p.e = e * factor;
            p.w = w * factor;
            p.T = T * factor;
            return p;
        }

        PhaseVector operator +(const PhaseVector &o)
        {
            PhaseVector p;
            p.a = a + o.a;
            p.v = v + o.v;
            p.x = x + o.x;
            p.e = e + o.e;
            p.w = w + o.w;
            p.T = T + o.T;
            return p;
        }
    };

private:
//    qreal mPitch;
//    qreal mYaw;
//    qreal mRoll;

//    QVector3D mEx, mEy, mEz;

    PhaseVector mP;

protected:
    bool mCalcEulerAngles;

    static QVector3D mGravity;

    qreal mMass;
    qreal Kupr;
    QMatrix3x3 mInertialTensor;
    QMatrix3x3 mInertialTensorInv;
//    QVector3D mCenterOfMass;
    QVector3D mForceOfGravity;

//    QVector3D mPosition;
//    QVector3D mVelocity;
//    QVector3D mAcceleration;

////    QQuaternion mRotation;
//    QMatrix3x3 mTransform;
//    QVector3D mAngularVelocity;
//    QVector3D mAngularAcceleration;

    QVector3D mForce;
    QVector3D mMomentum;
    QVector3D mExternalForce;
    QVector3D mExternalMomentum;

    QVector3D mCollisionPoint;
    QVector3D mCollisionPointNormal;
    qreal mBounceRadius;
    bool mCollisionState;

    qreal mKtr; // translational kinetic energy
    qreal mKrot; // rotational kinetic energy
    qreal mQpot; // potential energy

    friend class DynamicObject3D;

    virtual void integrateStep(qreal h, PhaseVector *result=nullptr);
    virtual void collisionHandler(qreal h);

public:
    explicit DynamicModel();
    virtual ~DynamicModel() {}

    static QVector3D gravity() {return mGravity;}
    QVector3D forceOfGravity() const {return mMass*mGravity;}

    void integrate(qreal dt, PhaseVector *result=nullptr);
    void reset();

    const QVector3D &position() const {return mP.x;}
    void setPosition(QVector3D pos) {mP.x = pos;}
//    const QQuaternion &rotation() const {return mRotation;}
//    void setRotation(QQuaternion rot) {mRotation = rot;}
    const QMatrix3x3 &rotation() const {return mP.T;}
    void setRotation(const QMatrix3x3 &m) {mP.T = m;}
    const QVector3D &velocity() const {return mP.v;}
    void setVelocity(QVector3D vel) {mP.v = vel;}
    const QVector3D &angularVelocity() const {return mP.w;}
    void setAngularVelocity(QVector3D vel) {mP.w = vel;}
    QVector3D acceleration() const {return mP.a;}
    QVector3D angularAcceleration() const {return mP.e;}

    const PhaseVector &phaseVector() const {return mP;}
    void setPhaseVector(const PhaseVector& phase) {mP = phase;}

//    qreal pitch() const {return mPitch;}
    float yaw() const
    {
        return atan2(mP.T(1, 0), mP.T(0, 0));
    }
//    qreal roll() const {return mRoll;}

    QVector3D ex() const {return QVector3D(mP.T(0, 0), mP.T(0, 1), mP.T(0, 2));}
    QVector3D ey() const {return QVector3D(mP.T(1, 0), mP.T(1, 1), mP.T(1, 2));}
    QVector3D ez() const {return QVector3D(mP.T(2, 0), mP.T(2, 1), mP.T(2, 2));}

    virtual void setControl(qreal u) {(void)u;}
    virtual void setControl(qreal u1, qreal u2) {(void)u1; (void)u2;}
    virtual void setControl(qreal u1, qreal u2, qreal u3) {(void)u1; (void)u2; (void)u3;}
    virtual void setControl(qreal u1, qreal u2, qreal u3, qreal u4) {(void)u1; (void)u2; (void)u3; (void)u4;}

    void setMass(qreal m) {mMass = m; mForceOfGravity = mMass * mGravity;}
    void setInertialTensor(const QMatrix3x3 &J) {mInertialTensor = J; mInertialTensorInv = invert(mInertialTensor);}
    void setInertialTensor(const QMatrix4x4 &J) {mInertialTensor = J.toGenericMatrix<3,3>(); mInertialTensorInv = invert(mInertialTensor);}
//    void setCenterOfMass(QVector3D C) {mCenterOfMass = C;}

     // these things disappears after every integrate()
    void setCollisionPoint(QVector3D point, QVector3D normal) {mCollisionState = true; mCollisionPoint = point; mCollisionPointNormal = normal;}
    bool collisionState() const {return mCollisionState;}

    void addExternalForce(QVector3D F, QVector3D r = QVector3D());
    void addExternalMomentum(QVector3D M) {mExternalMomentum += M;}
    void addExternalLocalMomentum(QVector3D M) {mExternalMomentum += mP.T*M;}
    QVector3D force() const {return mForce;}
    QVector3D momentum() const {return mMomentum;}

    qreal mass() const {return mMass;}
    const QMatrix3x3 &inertialTensor() const {return mInertialTensor;}
//    QVector3D centerOfMass() const {return mCenterOfMass;}

    qreal kineticEnergy() const {return mKtr + mKrot;}
    qreal potentialEnergy() const {return mQpot;}

    QVector3D rcpos;

    void rotate(qreal angle, qreal vx, qreal vy, qreal vz)
    {
        QMatrix3x3 Rot = rotationMatrix(angle*M_PI/180, QVector3D(vx, vy, vz).normalized());
        mP.T = mP.T * Rot;
    }
};

inline QVector3D operator*(const QMatrix3x3 &m, const QVector3D &v)
{
    return QVector3D(m(0,0)*v.x() + m(0,1)*v.y() + m(0,2)*v.z(),
                     m(1,0)*v.x() + m(1,1)*v.y() + m(1,2)*v.z(),
                     m(2,0)*v.x() + m(2,1)*v.y() + m(2,2)*v.z());
}

inline QMatrix3x3 koso(QVector3D v)
{
    float m[9] =
        {     0, -v.z(),  v.y(),
          v.z(),      0, -v.x(),
         -v.y(),  v.x(),      0};
    return QMatrix3x3(m);
}

inline QMatrix3x3 invert(const QMatrix3x3 &m)
{
    QMatrix3x3 adj;
    adj(0,0) =   m(1,1)*m(2,2) - m(1,2)*m(2,1);
    adj(0,1) = -(m(1,0)*m(2,2) - m(1,2)*m(2,0));
    adj(0,2) =   m(1,0)*m(2,1) - m(1,1)*m(2,0);
    adj(1,0) = -(m(0,1)*m(2,2) - m(0,2)*m(2,1));
    adj(1,1) =   m(0,0)*m(2,2) - m(0,2)*m(2,0);
    adj(1,2) = -(m(0,0)*m(2,1) - m(0,1)*m(2,0));
    adj(2,0) =   m(0,1)*m(1,2) - m(0,2)*m(1,1);
    adj(2,1) = -(m(0,0)*m(1,2) - m(0,2)*m(1,0));
    adj(2,2) =   m(0,0)*m(1,1) - m(0,1)*m(1,0);
    float det = m(0,0)*adj(0,0) + m(0,1)*adj(0,1) + m(0,2)*adj(0,2);
    QMatrix3x3 res;
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            res(i,j) = det? adj(j,i) / det: 0; // and transpose
    return res;
}

inline QMatrix3x3 rotationMatrix(double angle_rad, const QVector3D &v)
{
    float x = v.x();
    float y = v.y();
    float z = v.z();
    double cf = cos(angle_rad);
    double sf = sin(angle_rad);
    QMatrix3x3 res;
    res(0,0) = (1 - cf)*x*x + cf;
    res(0,1) = (1 - cf)*x*y - sf*z;
    res(0,2) = (1 - cf)*x*z + sf*y;
    res(1,0) = (1 - cf)*x*y + sf*z;
    res(1,1) = (1 - cf)*y*y + cf;
    res(1,2) = (1 - cf)*y*z - sf*x;
    res(2,0) = (1 - cf)*x*z - sf*y;
    res(2,1) = (1 - cf)*y*z + sf*x;
    res(2,2) = (1 - cf)*z*z + cf;
    return res;
}

inline QMatrix3x3 rotat(const QVector3D &w)
{
    double x = w.x();
    double y = w.y();
    double z = w.z();
    double xx = x*x;
    double xy = x*y;
    double xz = x*z;
    double yy = y*y;
    double yz = y*z;
    double zz = z*z;
    QMatrix3x3 res;
    res(0,0) = 1 - 0.5*(yy + zz);
    res(0,1) = 0.5*xy - z;
    res(0,2) = 0.5*xz + y;
    res(1,0) = 0.5*xy + z;
    res(1,1) = 1 - 0.5*(xx + zz);
    res(1,2) = 0.5*yz - x;
    res(2,0) = 0.5*xz - y;
    res(2,1) = 0.5*yz + x;
    res(2,2) = 1 - 0.5*(xx + yy);
    return res;
}


#endif // DYNAMICMODEL_H
