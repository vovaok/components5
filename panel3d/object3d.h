#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <QObject>
#include <QGLWidget>
#include <GL/glu.h>
#include <QMatrix4x4>
#include <math.h>
#include "texture.h"
#include "QtCore"

class PickEvent : public QEvent
{
private:
    QVector<int> mNames;

    friend class QPanel3D;
    friend class QObject3D;

    void addName(int name) {mNames << name;}

public:
    PickEvent() : QEvent(QEvent::User) {}

    int count() const {return mNames.count();}
    int name(int i) const {return mNames[i];}
};

class Object3D : public QObject
{
    Q_OBJECT

private:
    QObject *mPanel;
    bool FSettingsChanged;
    bool FVisible;
    bool boundsVisible;
    bool axesVisible;

    QColor mAmbientColor;
    QColor mDiffuseColor;
    QColor mSpecularColor;
    QColor mEmissionColor;
    int mShininess;

    QColor mDefAmbientColor;
    QColor mDefDiffuseColor;
    QColor mDefSpecularColor;
    QColor mDefEmissionColor;
    int mDefShininess;

    GLfloat mAmbientfv[4];
    GLfloat mDiffusefv[4];
    GLfloat mSpecularfv[4];
    GLfloat mEmissionfv[4];

    float mFullDrawTime;

    void applyRotation();
    void computeFullTransform();

protected:
    virtual void draw();
    virtual void applySettings(){}
    void setSettingsChanged();
    virtual void pickEvent(PickEvent &) {}

    GLfloat xrot, yrot, zrot;
    GLfloat transform[16];
    GLfloat fullTransform[16];
    GLfloat xcen, ycen, zcen;
    GLfloat xori, yori, zori;
    GLfloat xsc, ysc, zsc;

    Texture3D *mTexture;

    bool mPickable;
    bool mWireframe;

    void assignDefColor(GLfloat *a, GLfloat *d, GLfloat *e, GLfloat *s, int sh);

    friend class QPanel3D;

public:
    explicit Object3D(QObject *parent);
    virtual ~Object3D() {}
    QPanel3D *scene() const;
    void setParent(QObject *parent);

    void setVisible(bool visible);
    bool isVisible() const {return FVisible;}

    void setPickable(bool pickable) {mPickable = pickable;}
    bool isPickable() const {return mPickable;}

    void setWireframe(bool wireframe) {mWireframe = wireframe;}
    bool isWireframe() const {return mWireframe;}

//    QMatrix4x4 transform() {return trans;}
//    void setTransform(QMatrix4x4 &matrix);

    GLfloat* transformMatrix() {return transform;}
    GLfloat* fullTransformMatrix() {return fullTransform;}
    void findGlobalTransform(GLfloat *matrix);
    QVector3D pos() const {return QVector3D(transform[12], transform[13], transform[14]);}
    QVector3D rot() const {return QVector3D(xrot, yrot, zrot);}
    QVector3D center() const {return QVector3D(xcen, ycen, zcen);}
    QVector3D orient() const {return QVector3D(xori, yori, zori);}

    void setTransform(const GLfloat *matrix);

    void setXPos(qreal pos);
    void setYPos(qreal pos);
    void setZPos(qreal pos);
    void setPosition(QVector3D pos);
    void setPosition(qreal x, qreal y, qreal z);

    void setXRot(qreal angle);
    void setYRot(qreal angle);
    void setZRot(qreal angle);
    void setRotation(QVector3D rot);
    void setRotation(qreal x, qreal y, qreal z);
    void setRotation(const QQuaternion &q);
    void setRotation(const QMatrix3x3 &m);
    void rotate(qreal angle, qreal vx, qreal vy, qreal vz);

    void setXCenter(qreal pos);
    void setYCenter(qreal pos);
    void setZCenter(qreal pos);
    void setCenter(QVector3D center);
    void setCenter(qreal x, qreal y, qreal z);

    void setXOrient(qreal angle);
    void setYOrient(qreal angle);
    void setZOrient(qreal angle);
    void setOrient(QVector3D orient);
    void setOrient(qreal x, qreal y, qreal z);

    void setXScale(qreal scale);
    void setYScale(qreal scale);
    void setZScale(qreal scale);
    void setUniformScale(qreal scale);

    void setColor(QColor diffuse, QColor specular=Qt::black, QColor emission=Qt::black, float ambient=0.25, int shininess=0);
    void setDiffuseColor(QColor color);
    void setSpecularColor(QColor color);
    void setEmissionColor(QColor color);
    void setAmbient(float value);
    void setShininess(int value);
    void setDefaultColor();

    void showBounds(bool value);
    void showAxes(bool value);

    void setTexture(Texture3D *texture) {mTexture = texture; emit changed();}
    void setTexture(const QImage &image);

    float fullDrawTime() const {return mFullDrawTime;}

    virtual QVector3D getMinBounds();
    virtual QVector3D getMaxBounds();

signals:
    void changed();
    void settingsChanged();

public slots:
    void update();
    void drawObject();
};

#endif // OBJECT3D_H
