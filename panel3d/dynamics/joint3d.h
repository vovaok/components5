#ifndef JOINT3D_H
#define JOINT3D_H

#include "dynamicmodel.h"

class Joint3D
{
protected:
    DynamicModel *mModel1;
    DynamicModel *mModel2;

    QVector3D jz1, jz2;
    QVector3D jx1, jx2;
    QVector3D r1, r2;

    QVector3D F, M;
    float mQ, mQold, mDq, mQinit;
    float mSign;

    float kF_x, kF_v;
    float kM_angle, kMv_normal, kMv_collinear;

    void solve();

public:
    explicit Joint3D(DynamicModel *model1, DynamicModel *model2);

    void setRotationAxes(QVector3D model1_axis, QVector3D model2_axis);
    void setDirectionAxes(QVector3D model1_axis, QVector3D(model2_axis)) {jx1 = model1_axis; jx2 = model2_axis; solve();}
    void setApplicationPoints(QVector3D model1_point, QVector3D model2_point) {r1 = model1_point; r2 = model2_point; solve();}
    void setInitAngle(float q0_deg) {mQinit = q0_deg; solve();}
    void setSign(float sign) {mSign = sign>=0? 1: -1; solve();}

    void applyConstraints();

    void reset() {solve();}

    float q() const {return mQ;} // joint coordinate
    float dq() const {return mDq;}

    const QVector3D &reactionForce() const {return F;}
    const QVector3D &reactionMomentum() const {return M;}
    float reactionTorqueOfModel1() const {return QVector3D::dotProduct((mModel1->rotation() * M), jz1);}
    float reactionTorqueOfModel2() const {return QVector3D::dotProduct((mModel2->rotation() * M), jz2);}
};

#endif // JOINT3D_H
