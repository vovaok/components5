#include "qpanel3d.h"

QPanel3D::QPanel3D(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::AlphaChannel), parent),
    mOverpainting(false),
    mFbo(0L),
    mRenderingToBuffer(false),
    mBufferSize(320, 240),
    mBufferCamera(0L),
    mPickingEnabled(false),
    mPicking(false)
{
    FAutoUpdate = false;
    MainCam = new Camera3D(this);
    Camera = MainCam;
    pRoot = new Object3D(this);
    pWorld = new Object3D(this);
    pRoot->setVisible(false);
    pWorld->setVisible(false);
    ViewType = object;
    BackColor = Qt::black;

    setAutoFillBackground(false);
}
//----------------------------------------------------------

//void QPanel3D::childEvent(QChildEvent *e)
//{
//    Object3D *child = qobject_cast<Object3D*>(e->child());
//    connect(child, SIGNAL(changed()), this, SLOT(updateGL()));
//    connect(child, SIGNAL(settingsChanged()), this, SLOT(glSettings()));
//    connect(this, SIGNAL(onUpdate()), child, SLOT(update()));
//}
//----------------------------------------------------------

void QPanel3D::initializeGL()
{
    qglClearColor(BackColor);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_MULTISAMPLE_ARB);
    glDisable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);
    glEnable(GL_NORMALIZE);

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

//    QGLFormat fmt;
//    fmt.setStereo(true);
//    setFormat(fmt);
}

void QPanel3D::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLint)width, (GLint)height);
    Camera->setAspect((GLfloat)width / (GLfloat)height);
    glSettings();
//    if (!FAutoUpdate)
//        updateGL();
}

void QPanel3D::paintGL()
{
    Camera3D *cam = Camera;
    if (mRenderingToBuffer)
    {
        QGLFramebufferObjectFormat fmt;
        fmt.setAttachment(QGLFramebufferObject::Depth);
        fmt.setInternalTextureFormat(GL_RGBA8);
        fmt.setSamples(4);
        mFbo = new QGLFramebufferObject(mBufferSize, fmt);
        mFbo->bind();
        qglClearColor(Qt::transparent);
        glViewport(0, 0, (GLint)mBufferSize.width(), (GLint)mBufferSize.height());
        if (mBufferCamera)
        {
            Camera = mBufferCamera;
            Camera->setAspect((float)mBufferSize.width() / (float)mBufferSize.height());
        }
        gluPerspective(Camera->viewAngle() / Camera->zoom(), Camera->aspect(), Camera->nearPlane(), Camera->distanceLimit());
    }

    if (this != Camera->parent())
    {
        GLfloat matrix[16];
        Camera->findGlobalTransform(matrix);
        Camera->setTransform(matrix);
    }

//    if (mFrame)
//        Camera->setPosition(Camera->position()+QVector3D(10, 0, 0));
//    else
//        Camera->setPosition(Camera->position()-QVector3D(10, 0, 0));
//    mFrame = !mFrame;
//    glSettings();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    pWorld->drawObject();
    pRoot->drawObject();

    if (mRenderingToBuffer)
    {
        mFbo->release();
        qglClearColor(BackColor);
        glViewport(0, 0, (GLint)width(), (GLint)height());
        if (mBufferCamera)
        {
            Camera = cam;
            Camera->setAspect((GLfloat)width() / (GLfloat)height());
        }
    }
}

void QPanel3D::paintEvent(QPaintEvent *)
{
    makeCurrent();

    paintGL();

    if (mOverpainting)
    {
//        glMatrixMode(GL_MODELVIEW);
//        glPushMatrix();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_CULL_FACE);
        QPainter p(this);
        emit overpaint(p);
        p.end();
        glPopAttrib();
        glEnable(GL_MULTISAMPLE_ARB);
//        glMatrixMode(GL_MODELVIEW);
//        glPopMatrix();
    }
    else
    {
        swapBuffers();
    }
}
//----------------------------------------------------------

void QPanel3D::setCamera(Camera3D *camera)
{
    Camera = camera;
    Camera->setAspect((GLfloat)width() / (GLfloat)height());
    updateGL();
}

void QPanel3D::setAutoUpdate(bool on)
{
    FAutoUpdate = on;
}

void QPanel3D::setViewType(EViewType type)
{
    ViewType = type;
    if (ViewType == fly)
        Camera->setFollowing(false);
    else if (ViewType == object)
        Camera->setFollowing(true);

    //updateGL();
}

void QPanel3D::setBackColor(QColor color)
{
    BackColor = color;
    makeCurrent();
    qglClearColor(BackColor);
    updateGL();
}
//----------------------------------------------------------

void QPanel3D::glSettings()
{
    makeCurrent();

    Light3D *light = qobject_cast<Light3D*>(sender());
    if (light)
    {
        if (!mLights.contains(light))
        {
            mLights << light;
            light->setLightNo(mLights.count() - 1);
        }
    }

    emit onUpdate();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (mPicking)
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT,viewport);
        gluPickMatrix(mPickPoint.x(), viewport[3]-mPickPoint.y(), 3, 3, viewport);
    }

    if (Camera->ortho())
    {
        int w, h, d;
        d = Camera->distanceLimit();
        if (Camera->fixedViewport())
        {
                w = Camera->fixedViewportSize().width() / 2;
                h = Camera->fixedViewportSize().height() / 2;
        }
        else
        {
                h = d / Camera->zoom();
                w = h * Camera->aspect();
        }
        glOrtho(-w,w, -h,h, -d,d);
    }
    else
    {
        gluPerspective(Camera->viewAngle() / Camera->zoom(), Camera->aspect(), Camera->nearPlane(), Camera->distanceLimit());
    }
    gluLookAt(Camera->position().x(), Camera->position().y(), Camera->position().z(),
              Camera->target().x(), Camera->target().y(), Camera->target().z(),
              Camera->topDir().x(), Camera->topDir().y(), Camera->topDir().z());

    glMatrixMode(GL_MODELVIEW);
}

void QPanel3D::sceneChanged()
{
    if (FAutoUpdate)
        updateGL();
}
//----------------------------------------------------------

QVector3D QPanel3D::cursorToPoint(int x, int y)
{
    makeCurrent();

    GLdouble mm[16];
    GLdouble pm[16];
    GLint vp[4];

//    glPushMatrix();
//    glMultMatrixf(pRoot->transformMatrix());
    glGetDoublev(GL_MODELVIEW_MATRIX, mm);
//    glPopMatrix();
    glGetDoublev(GL_PROJECTION_MATRIX, pm);
    glGetIntegerv(GL_VIEWPORT, vp);

    GLfloat maus[3] = {(float)x, (float)(vp[3]-y-1), 0};
    GLdouble obj[3];

    QVector3D p1;//, p2;
//    QVector3D v;

    if (gluUnProject(maus[0], maus[1], maus[2], mm, pm, vp, obj, obj+1, obj+2))
    {
        p1 = QVector3D(obj[0], obj[1], obj[2]);
//        glReadPixels(maus[0], maus[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, maus+2);
//        gluUnProject(maus[0], maus[1], maus[2], mm, pm, vp, obj, obj+1, obj+2);
//        p2 = QVector3D(obj[0], obj[1], obj[2]);
//        v = p2 - p1;
//
//            float kv = -(Vector3D(p1) * Camera->Direction) / (v * Camera->Direction);
//            p2 = p1 + v*kv;
//
//            if (Event)
//            Event(this, Shift, p2, v);
    }

    return p1;
}

void QPanel3D::cursorToRootPoint(int x, int y)
{
    makeCurrent();

    GLdouble mm[16];
    GLdouble pm[16];
    GLint vp[4];

    glPushMatrix();
    glMultMatrixf(pRoot->transformMatrix());
    glGetDoublev(GL_MODELVIEW_MATRIX, mm);
    glPopMatrix();
    glGetDoublev(GL_PROJECTION_MATRIX, pm);
    glGetIntegerv(GL_VIEWPORT, vp);

    GLfloat maus[3] = {(float)x, (float)(vp[3]-y-1), 0};
    GLdouble obj[3];

    QVector3D p1, p2;
    QVector3D v;

    if (gluUnProject(maus[0], maus[1], maus[2], mm, pm, vp, obj, obj+1, obj+2))
    {
        p1 = QVector3D(obj[0], obj[1], obj[2]);
        glReadPixels(maus[0], maus[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, maus+2);
        gluUnProject(maus[0], maus[1], maus[2], mm, pm, vp, obj, obj+1, obj+2);
        p2 = QVector3D(obj[0], obj[1], obj[2]);
        v = p2 - p1;
//
//            float kv = -(Vector3D(p1) * Camera->Direction) / (v * Camera->Direction);
//            p2 = p1 + v*kv;
//
//            if (Event)
//            Event(this, Shift, p2, v);
    }

    mousePoint = p2;
    mouseVector = v;
}
//----------------------------------------------------------

void QPanel3D::pick(int x, int y)
{
    mPicking = true;
    mPickPoint = QPoint(x, y);
    makeCurrent();

    glSelectBuffer(mSelectBufSize, mSelectBuf);
    glRenderMode(GL_SELECT);

    // set picking matrix
    glSettings();

    updateGL();

    int hits;
    // restoring the original projection matrix
    mPicking = false;
    glSettings();

    // returning to normal rendering mode
    hits = glRenderMode(GL_RENDER);

    // if there are hits process them
    QMap<int, Object3D*> objList;
    int i = 0;
    for (int hh=0; hh<hits; hh++)
    {
        QString s;
        QObject *obj = this;
        int names = mSelectBuf[i++];
        int min = mSelectBuf[i++];
        int max = mSelectBuf[i++];
        Q_UNUSED(max);
        for (int j=0; j<names; j++)
        {
            int oidx = mSelectBuf[i++];
            if (obj && (oidx < obj->children().size()))
            {
                obj = obj->children()[oidx];
            }
        }
        objList[min] = qobject_cast<Object3D*>(obj);
    }
    if (objList.count())
        emit clicked(objList.first());
    else
        emit clicked((Object3D*)0L);
}
//---------------------------------------------------------

void QPanel3D::mousePressEvent(QMouseEvent *event)
{
    mousePos = event->pos();
    if (event->buttons() & Qt::LeftButton)
    {
        cursorToRootPoint(mousePos.x(), mousePos.y());
        emit clicked(mousePoint, mouseVector);

        if (mPickingEnabled)
            pick(mousePos.x(), mousePos.y());
    }
}

void QPanel3D::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - mousePos.x();
    int dy = event->y() - mousePos.y();

//    QVector3D p0 = cursorToPoint(mousePos.x(), mousePos.y());
//    QVector3D p1 = cursorToPoint(event->x(), event->y());

//    QPoint x0 = mousePos;
    mousePos = event->pos();

    if (hasMouseTracking())
    {
        cursorToRootPoint(mousePos.x(), mousePos.y());
        emit moved(mousePoint, mouseVector);
    }

//    emit mouse(p0);

//    if (!FMouseView)
//    {
//        if (OnMouseMove)
//            OnMouseMove(this, Shift, X, Y);
//
//        if (FOnMouseMove3D)
//            Panel3DPointEvent(Shift, X, Y, FOnMouseMove3D);
//
//        return;
//    }

    if (this == Camera->parent() && event->buttons())
    {
        if (Camera->ortho())
        {
            GLfloat zoom = 2*Camera->distanceLimit() / this->height() / Camera->zoom();
            QVector3D vx;
            vx = QVector3D::normal(Camera->topDir(), Camera->direction()) * zoom;
            QVector3D vy = Camera->topDir();
            vy.normalize();
            vy = vy * zoom;
            if (event->buttons() & Qt::RightButton)
            {
                //CurrentCam->Pivot = CurrentCam->Pivot + vx*(X-ox) + vy*(Y-oy);
            }
            else if (event->buttons() & Qt::MidButton)
            {
                Camera->setPosition(Camera->position() + vx*dx + vy*dy);
            }
        }
        else
        {
            QVector3D vx, vy;
            QVector3D dir = Camera->direction();
            QVector3D top = Camera->topDir();
            vx = QVector3D::normal(top, dir);
            vy = QVector3D::normal(dir, vx);

            if (event->buttons() & Qt::RightButton)
            {
                if (ViewType == object)
                {
//                    QVector3D v0 = p0 - pRoot->pos();
//                    QVector3D v1 = p1 - pRoot->pos();
//                    QVector3D vv = QVector3D::normal(v1, v0);
//                    qreal angle = QVector3D::crossProduct(v0, v1).length() / (v0.length() * v1.length());
//                    if (fabs(angle) <= 1)
//                        pRoot->rotate(asin(angle)*180/M_PI, vv.x(), vv.y(), vv.z());

                    QVector3D vv = vx*dx + vy*dy;
                    qreal angle = vv.length() * 0.5;
                    vv = QVector3D::normal(dir, vv);
                    GLfloat *mat = pRoot->transformMatrix();
                    QVector3D vm;
                    vm.setX(mat[0]*vv.x()+mat[1]*vv.y()+mat[2]*vv.z());
                    vm.setY(mat[4]*vv.x()+mat[5]*vv.y()+mat[6]*vv.z());
                    vm.setZ(mat[8]*vv.x()+mat[9]*vv.y()+mat[10]*vv.z());
                    //pRoot->rotate(angle, -0.707, 0, 0.707);
//                    pRoot->rotate(angle, vy.x(), vy.y(), vy.z());
                    pRoot->rotate(angle, vm.x(), vm.y(), vm.z());
                     emit mouse(vv);
//                    Camera->setTopDir(vy);
//                    qreal d = Camera->position().length();
//                    vx = vx * (dx/10.0) * d * 0.1;
//                    vy = vy * (dy/10.0) * d * 0.1;
//                    QVector3D nv = Camera->position() + vx + vy;
//                    nv = nv.normalized() * d;
//                    Camera->setPosition(nv);
                }
                else if (ViewType == fly)
                {
                    Camera->setTopDir(vy);
                    vx = vx * (dx/360.0);
                    vy = vy * (dy/360.0);
                    Camera->setDirection(Camera->direction() + vy + vx);
                }
            }
            else if (event->buttons() & Qt::MidButton)
            {
                if (ViewType == object)
                {
                    qreal d = (Camera->position() - pRoot->pos()).length();
                    vx = vx * (dx/10.0) * d * 0.015;
                    vy = vy * (dy/10.0) * d * 0.015;
                    Camera->setPosition(Camera->position() + vx + vy);
                }
                else if (ViewType == fly)
                {
                    vx = vx * (dx/10.0) * Camera->distanceLimit()/250.0;
                    vy = vy * (dy/10.0) * Camera->distanceLimit()/250.0;
                    Camera->setPosition(Camera->position() + vx + vy);
                }
            }
        }
    }
}

void QPanel3D::wheelEvent(QWheelEvent *event)
{
    QVector3D p1 = cursorToPoint(event->x(), event->y());
    if (this == Camera->parent())
    {
        if (Camera->ortho())
        {
            Camera->setZoom(Camera->zoom() * (1 + event->delta()/5000.0));
        }
        else
        {
            if (ViewType == object)
            {
    //            Camera->setZoom(Camera->zoom() * (1 + event->delta()/5000.0));
                qreal d = Camera->position().length();
    //            QVector3D dir = Camera->direction();
                QVector3D dir = (p1 - Camera->position()).normalized();
                QVector3D v = dir * (event->delta()/100.0) * d * 0.01;
                Camera->setPosition(Camera->position() + v);
            }
            else if (ViewType == fly)
            {
                QVector3D v = Camera->direction() * (event->delta()/100.0 * Camera->distanceLimit()/250.0);
                Camera->setPosition(Camera->position() + v);
            }
        }
    }
}

void QPanel3D::setLightingEnabled(bool enabled)
{
    makeCurrent();
    if (enabled)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);
}
//---------------------------------------------------------------------------

void QPanel3D::renderToImage(QImage &img)
{
    mRenderingToBuffer = true;
    updateGL();
    if (mFbo)
    {
        img = mFbo->toImage();
        delete mFbo;
    }
    mRenderingToBuffer = false;
}
//---------------------------------------------------------------------------
