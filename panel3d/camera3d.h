#ifndef CAMERA3D_H
#define CAMERA3D_H

#include "object3d.h"
#include <QVector3D>
#include <QSizeF>

class Camera3D : public Object3D
{
    Q_OBJECT

private:
    QVector3D FPosition;
    QVector3D FTarget;
    QVector3D FDirection;
    QVector3D FTopDir;
    qreal FViewAngle;
    qreal FNearPlane;
    qreal FAspect;
    qreal mAspectWidth;
    qreal FZoom;
    qreal FDistanceLimit;
    bool FOrtho;
    QSizeF FFixedViewportSize;
    bool FFixedViewport;
    bool FFollowTarget;

public:
    explicit Camera3D(QObject *parent);

    QVector3D position() {return FPosition;}
    QVector3D target() {return FTarget;}
    QVector3D direction() {return FDirection;}
    QVector3D topDir() {return FTopDir;}
    qreal viewAngle() {return FViewAngle;}
    qreal nearPlane() {return FNearPlane;}
    qreal aspect() {return FAspect;}
    qreal aspectWidth() const {return mAspectWidth;}
    qreal zoom() {return FZoom;}
    qreal distanceLimit() {return FDistanceLimit;}
    bool ortho() {return FOrtho;}
    QSizeF fixedViewportSize() {return FFixedViewportSize;}
    bool fixedViewport() {return FFixedViewport;}
    bool isFollowing() {return FFollowTarget;}

    void setPosition(QVector3D pos);
    void setTarget(QVector3D target);
    void setDirection(QVector3D dir);
    void setTopDir(QVector3D topDir);
    void setAngle(qreal angle);
    void setNearPlane(qreal dist);
    void setAspect(qreal aspect);
    void setAspectWidth(qreal aspectWidth);
    void setZoom(qreal zoom);
    void setDistanceLimit(qreal limit);
    void setOrtho(bool ortho);
    void setFixedViewportSize(QSizeF size);
    void setFixedViewport(bool value);
    void setFollowing(bool follow);
    void setTransform(GLfloat *matrix);

    //Camera3D& operator =(const Camera3D& camera);

signals:

public slots:
    //void applySettings();

};

#endif // CAMERA3D_H
