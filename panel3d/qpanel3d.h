#ifndef QPANEL3D_H
#define QPANEL3D_H

#include <QGLWidget>
#include "object3d.h"
#include "camera3d.h"
#include "light3d.h"
#include <QMouseEvent>
//#include <QGLFramebufferObject>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

class QPanel3D : public QGLWidget
{
    Q_OBJECT

public:
    typedef enum {ViewFly, ViewObject, ViewPlane} EViewType;

private:
    bool FAutoUpdate;
    bool mOverpainting;
    Camera3D *mMainCamera = nullptr;
    Camera3D *mCamera = nullptr;
    QVector<Light3D*> mLights;
    Object3D *mRoot = nullptr;
    Object3D *mWorld = nullptr;

    QColor mBackColor;

    EViewType mViewType;

    QPoint mMousePos;
    QVector3D mMousePoint;
    QVector3D mMouseVector;
    float mMouseZplane;
    float m_mouseSensitivity = 1.f;

    QVector3D cursorToPoint(int x, int y);
    void cursorToRootPoint(int x, int y);

    QGLFramebufferObject *mFrameBufferObject = nullptr;
    bool mRenderingToBuffer;
    QSize mBufferSize;
    Camera3D *mBufferCamera = nullptr;

    const static int mSelectBufSize = 512;
    GLuint mSelectBuf[mSelectBufSize];
    bool mPickingEnabled;
    bool mPicking;
    QPoint mPickPoint;

    friend class Object3D;

protected:
    void initializeGL();
    void paintGL();
    void paintEvent(QPaintEvent *);
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

public:
    explicit QPanel3D(QWidget *parent = nullptr);

    Camera3D* camera() {return mCamera;}
    void setCamera(Camera3D *value);

    Object3D* root() {return mRoot;}
    Object3D* world() {return mWorld;}

    EViewType viewType() {return mViewType;}
    void setViewType(EViewType type);

    QColor backColor() {return mBackColor;}
    void setBackColor(QColor color);

    bool isOverpaintingEnabled() const {return mOverpainting;}
    void setOverpaintingEnabled(bool enable=true) {mOverpainting = enable;}
    
    void setAutoUpdate(bool on);

    void setLightingEnabled(bool enabled=true);

    void setPickingEnabled(bool enabled) {mPickingEnabled = enabled;}
    bool isPickingEnabled() const {return mPickingEnabled;}
    bool isPicking() const {return mPicking;}

    void renderToImage(QImage &img);
    void setBufferSize(int width, int height) {mBufferSize = QSize(width, height);}
    void setBufferCamera(Camera3D *cam) {mBufferCamera = cam;}

    void renderText(double x, double y, double z, const QString & str, const QFont & fnt = QFont());

    bool isRenderingToBuffer() {return mRenderingToBuffer;}

    void setMousePlane(QVector3D pointOnPlane);

    void setMouseSensitivity(float value) {if (value > 0) m_mouseSensitivity = value;}
    float mouseSensitivity() const {return m_mouseSensitivity;}

signals:
    void onUpdate();
    void onDraw();
    void overpaint(QPainter &p);
    void mouse(QVector3D v, QVector3D n);
    void clicked(QVector3D point, QVector3D vector);
    void clicked(Object3D *object);
    void moved(QVector3D point, QVector3D vector);

public slots:
    void glSettings();
    void sceneChanged();
    void pick(int x, int y);
};

#endif // QPANEL3D_H
