#include "camera3d.h"

Camera3D::Camera3D(QObject *parent) :
    Object3D(parent),
    mAspectWidth(1.0),
    m_lockDirection(false)
{
    FPosition = QVector3D(100, 100, 100);
    FTarget = QVector3D();
    FDirection = QVector3D(-1, -1, -1).normalized();
    FTopDir = QVector3D(0, 1, 0);
    FViewAngle = 45.0;
    FNearPlane = 1.0;
    FAspect = 640.0 / 480.0;
    FZoom = 1.0;
    FDistanceLimit = 1000;
    FOrtho = false;
    FFixedViewportSize = QSizeF(320, 240);
    FFixedViewport = false;
    FFollowTarget = false;
}

void Camera3D::setPosition(QVector3D pos)
{
    FPosition = pos;
    if (FFollowTarget)
        FDirection = (FTarget - FPosition).normalized();
    else
        FTarget = FPosition + FDirection;
    setSettingsChanged();
}

void Camera3D::setTarget(QVector3D target)
{
    FTarget = target;
    FDirection = (FTarget - FPosition).normalized();
//    FFollowTarget = true;
    setSettingsChanged();
}

void Camera3D::setDirection(QVector3D dir)
{
    if (m_lockDirection)
        return;
    FDirection = dir.normalized();
    FTarget = FPosition + FDirection;
//    FFollowTarget = false;
    setSettingsChanged();
}

void Camera3D::setTopDir(QVector3D topDir)
{
    if (m_lockDirection)
        return;
    FTopDir = topDir;
    setSettingsChanged();
}

void Camera3D::setAngle(qreal angle)
{
    FViewAngle = angle;
    setSettingsChanged();
}

void Camera3D::setNearPlane(qreal dist)
{
    FNearPlane = dist;
    setSettingsChanged();
}

void Camera3D::setAspect(qreal aspect)
{
    FAspect = aspect * mAspectWidth;
    setSettingsChanged();
}

void Camera3D::setAspectWidth(qreal aspectWidth)
{
    mAspectWidth = aspectWidth;
    setAspect(FAspect);
}

void Camera3D::setZoom(qreal zoom)
{
    FZoom = zoom;
    if (!FOrtho)
    {
        GLfloat angle = FViewAngle / FZoom;
        if (angle > 180)
            FZoom = FViewAngle / 180;
    }
    setSettingsChanged();
}

void Camera3D::setDistanceLimit(qreal limit)
{
    FDistanceLimit = limit;
    setSettingsChanged();
}

void Camera3D::setOrtho(bool ortho)
{
    FOrtho = ortho;
    setSettingsChanged();
}

void Camera3D::setFixedViewportSize(QSizeF size)
{
    FFixedViewportSize = size;
    setSettingsChanged();
}

void Camera3D::setFixedViewport(bool value)
{
    FFixedViewport = value;
    setSettingsChanged();
}

void Camera3D::setFollowing(bool follow)
{
    FFollowTarget = follow;
}

void Camera3D::setTransform(GLfloat *matrix)
{
    FFollowTarget = false;
    FPosition = QVector3D(matrix[12], matrix[13], matrix[14]);
    FDirection = QVector3D(matrix[0], matrix[1], matrix[2]);
    FTopDir = QVector3D(matrix[8], matrix[9], matrix[10]);
    FTarget = FPosition + FDirection;
    setSettingsChanged();
}
//---------------------------------------------------------------
