#include "qpanel3d.h"

QPanel3D::QPanel3D(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::AlphaChannel), parent),
    mOverpainting(false),
    mMouseZplane(0.5f),
    mRenderingToBuffer(false),
    mBufferSize(320, 240),
    mPickingEnabled(false),
    mPicking(false)
{
    FAutoUpdate = false;
    mMainCamera = new Camera3D(this);
    mCamera = mMainCamera;
    mWorld = new Object3D(this);
    mWorld->setVisible(true);
    mRoot = new Object3D(this);
    mRoot->setVisible(true);
    mViewType = object;
    mBackColor = Qt::black;

    setAutoFillBackground(false);
}

void QPanel3D::initializeGL()
{
    qglClearColor(mBackColor);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE_ARB);
    glDisable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, true);
    glEnable(GL_NORMALIZE);

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void QPanel3D::resizeGL(int width, int height)
{
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
    mCamera->setAspect(static_cast<qreal>(width) / static_cast<qreal>(height));
    glSettings();
}

void QPanel3D::paintGL()
{
    Camera3D *cam = mCamera;
    if (mRenderingToBuffer)
    {
        QGLFramebufferObjectFormat format;
        format.setAttachment(QGLFramebufferObject::Depth);
        format.setInternalTextureFormat(GL_RGBA8);
        format.setSamples(4);

        mFrameBufferObject = new QGLFramebufferObject(mBufferSize, format);
        mFrameBufferObject->bind();

        qglClearColor(Qt::transparent);
        glViewport(0, 0, static_cast<GLint>(mBufferSize.width()), static_cast<GLint>(mBufferSize.height()));
        if (mBufferCamera)
        {
            mCamera = mBufferCamera;
            mCamera->setAspect(static_cast<qreal>(mBufferSize.width()) / static_cast<qreal>(mBufferSize.height()));
        }
        gluPerspective(mCamera->viewAngle() / mCamera->zoom(), mCamera->aspect(), mCamera->nearPlane(), mCamera->distanceLimit());
    }

    if (this != mCamera->parent())
    {
        GLfloat matrix[16];
        mCamera->findGlobalTransform(matrix);
        mCamera->setTransform(matrix);
    }

    qglClearColor(mBackColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    mWorld->drawObject();
    mRoot->drawObject();

    if (mRenderingToBuffer)
    {
        mFrameBufferObject->release();
        qglClearColor(mBackColor);
        glViewport(0, 0, static_cast<GLint>(width()), static_cast<GLint>(height()));
        if (mBufferCamera)
        {
            mCamera = cam;
            mCamera->setAspect(static_cast<qreal>(width()) / static_cast<qreal>(height()));
        }
    }
}

void QPanel3D::paintEvent(QPaintEvent *)
{
    makeCurrent();

    paintGL();

    if (mOverpainting)
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_CULL_FACE);
        QPainter p(this);
        emit overpaint(p);
        p.end();
        glPopAttrib();
        glEnable(GL_MULTISAMPLE_ARB);
    }
    else
    {
        swapBuffers();
    }
}

void QPanel3D::setCamera(Camera3D *value)
{
    mCamera = value;
    mCamera->setAspect((GLfloat)width() / (GLfloat)height());
    updateGL();
}

void QPanel3D::setAutoUpdate(bool on)
{
    FAutoUpdate = on;
}

void QPanel3D::setViewType(EViewType type)
{
    mViewType = type;
    if (mViewType == fly)
        mCamera->setFollowing(false);
    else if (mViewType == object)
        mCamera->setFollowing(true);
}

void QPanel3D::setBackColor(QColor color)
{
    mBackColor = color;
    makeCurrent();
    qglClearColor(mBackColor);
    updateGL();
}

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

    if (mCamera->ortho())
    {
        int w, h, d;
        d = mCamera->distanceLimit();
        if (mCamera->fixedViewport())
        {
                w = mCamera->fixedViewportSize().width() / 2;
                h = mCamera->fixedViewportSize().height() / 2;
        }
        else
        {
                h = d / mCamera->zoom();
                w = h * mCamera->aspect();
        }
        glOrtho(-w,w, -h,h, -d,d);
    }
    else
    {
        gluPerspective(mCamera->viewAngle() / mCamera->zoom(), mCamera->aspect(), mCamera->nearPlane(), mCamera->distanceLimit());
    }
    gluLookAt(mCamera->position().x(), mCamera->position().y(), mCamera->position().z(),
              mCamera->target().x(), mCamera->target().y(), mCamera->target().z(),
              mCamera->topDir().x(), mCamera->topDir().y(), mCamera->topDir().z());

    glMatrixMode(GL_MODELVIEW);
}

void QPanel3D::sceneChanged()
{
    if (FAutoUpdate)
        updateGL();
}

QVector3D QPanel3D::cursorToPoint(int x, int y)
{
    makeCurrent();

    GLdouble mm[16];
    GLdouble pm[16];
    GLint vp[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, mm);
    glGetDoublev(GL_PROJECTION_MATRIX, pm);
    glGetIntegerv(GL_VIEWPORT, vp);

        GLdouble obj[3];

    GLfloat mouse[3] = {(float)x, (float)(vp[3]-y-1), 0};
    QVector3D p1;

    if (gluUnProject(mouse[0], mouse[1], mouse[2], mm, pm, vp, obj, obj+1, obj+2))
    {
        p1 = QVector3D(obj[0], obj[1], obj[2]);
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
    glMultMatrixf(mRoot->transformMatrix());
    glGetDoublev(GL_MODELVIEW_MATRIX, mm);
    glPopMatrix();
    glGetDoublev(GL_PROJECTION_MATRIX, pm);
    glGetIntegerv(GL_VIEWPORT, vp);

    GLfloat mouse[3] = {(float)x, (float)(vp[3]-y-1), 0};
    GLdouble obj[3];

    QVector3D p1, p2;
    QVector3D v;

    if (gluUnProject(mouse[0], mouse[1], mouse[2], mm, pm, vp, obj, obj+1, obj+2))
    {
        p1 = QVector3D(obj[0], obj[1], obj[2]);
        glReadPixels(mouse[0], mouse[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, mouse+2);
        gluUnProject(mouse[0], mouse[1], mouse[2], mm, pm, vp, obj, obj+1, obj+2);
        p2 = QVector3D(obj[0], obj[1], obj[2]);
        v = p2 - p1;
    }

    mMousePoint = p2;
    mMouseVector = v;
}

void QPanel3D::setMousePlane(QVector3D pointOnPlane)
{
    makeCurrent();

    GLdouble mm[16];
    GLdouble pm[16];
    GLint vp[4];

    glPushMatrix();
    glMultMatrixf(mRoot->transformMatrix());
    glGetDoublev(GL_MODELVIEW_MATRIX, mm);
    glPopMatrix();
    glGetDoublev(GL_PROJECTION_MATRIX, pm);
    glGetIntegerv(GL_VIEWPORT, vp);

    GLdouble win[3];

    if (gluProject(pointOnPlane.x(), pointOnPlane.y(), pointOnPlane.z(), mm, pm, vp, win, win+1, win+2))
    {
        mMouseZplane = win[2];
    }
}

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
    QMap<int, PickEvent> evList;
    int i = 0;
    for (int hh=0; hh<hits; hh++)
    {
        QString s;
        QObject *obj = this;
        int names = mSelectBuf[i++];
        int min = mSelectBuf[i++];
        int max = mSelectBuf[i++];
        Q_UNUSED(max);

        PickEvent e;
        for (int j=0; j<names; j++)
        {
            int oidx = mSelectBuf[i++];
            if (obj)
            {
                if (oidx < obj->children().size())
                    obj = obj->children()[oidx];
                else
                    e.addName(oidx);
            }
        }
        Object3D *obj3d = qobject_cast<Object3D*>(obj);
        objList[min] = obj3d;
        evList[min] = e;
    }
    if (objList.count())
    {
        objList.first()->pickEvent(evList.first());
        emit clicked(objList.first());
    }
    else
        emit clicked(nullptr);
}

void QPanel3D::mousePressEvent(QMouseEvent *event)
{
    mMousePos = event->pos();
    if (event->buttons() & Qt::LeftButton)
    {
        cursorToRootPoint(mMousePos.x(), mMousePos.y());
        emit clicked(mMousePoint, mMouseVector);

        if (mPickingEnabled)
            pick(mMousePos.x(), mMousePos.y());
    }
}

void QPanel3D::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - mMousePos.x();
    int dy = event->y() - mMousePos.y();

    mMousePos = event->pos();

    if (event->buttons() & Qt::LeftButton)
    {
        cursorToRootPoint(mMousePos.x(), mMousePos.y());
        emit moved(mMousePoint, mMouseVector);
    }

    if (this == mCamera->parent() && event->buttons())
    {
        if (mCamera->ortho())
        {
            GLfloat zoom = 2 * mCamera->distanceLimit() / this->height() / mCamera->zoom();
            QVector3D vx;
            vx = QVector3D::normal(mCamera->topDir(), mCamera->direction()) * zoom;
            QVector3D vy = mCamera->topDir();
            vy.normalize();
            vy = vy * zoom;

            if (event->buttons() & Qt::MidButton)
            {
                mCamera->setPosition(mCamera->position() + vx*dx + vy*dy);
            }
        }
        else
        {
            QVector3D vx, vy;
            QVector3D dir = mCamera->direction();
            QVector3D top = mCamera->topDir();
            vx = QVector3D::normal(top, dir);
            vy = QVector3D::normal(dir, vx);

            if (event->buttons() & Qt::LeftButton)
            {
                GLdouble mm[16];
                GLdouble pm[16];
                GLint vp[4];

                glPushMatrix();
                glMultMatrixf(mRoot->transformMatrix());
                glGetDoublev(GL_MODELVIEW_MATRIX, mm);
                glPopMatrix();
                glGetDoublev(GL_PROJECTION_MATRIX, pm);
                glGetIntegerv(GL_VIEWPORT, vp);

                GLdouble obj[3];
                QVector3D p1, p2;
                QVector3D v;

                if (gluUnProject(event->x()-dx, vp[3]-event->y()+dy-1, mMouseZplane, mm, pm, vp, obj, obj+1, obj+2))
                {
                    p1 = QVector3D(obj[0], obj[1], obj[2]);
                    gluUnProject(event->x(), vp[3]-event->y()-1, mMouseZplane, mm, pm, vp, obj, obj+1, obj+2);
                    p2 = QVector3D(obj[0], obj[1], obj[2]);
                    gluUnProject(event->x(), vp[3]-event->y()-1, 0, mm, pm, vp, obj, obj+1, obj+2);
                    QVector3D n = (QVector3D(obj[0], obj[1], obj[2]) - p2).normalized();
                    v = p2 - p1;
                    emit mouse(v, n);
                }
            }
            else if (event->buttons() & Qt::RightButton)
            {
                if (mViewType == object)
                {

                    QVector3D vv = vx*dx + vy*dy;
                    qreal angle = vv.length() * 0.5;
                    vv = QVector3D::normal(dir, vv);
                    GLfloat *mat = mRoot->transformMatrix();
                    QVector3D vm;
                    vm.setX(mat[0]*vv.x()+mat[1]*vv.y()+mat[2]*vv.z());
                    vm.setY(mat[4]*vv.x()+mat[5]*vv.y()+mat[6]*vv.z());
                    vm.setZ(mat[8]*vv.x()+mat[9]*vv.y()+mat[10]*vv.z());

                    mRoot->rotate(angle, vm.x(), vm.y(), vm.z());
                }
                else if (mViewType == fly)
                {
                    mCamera->setTopDir(vy);
                    vx = vx * (dx/360.0);
                    vy = vy * (dy/360.0);
                    mCamera->setDirection(mCamera->direction() + vy + vx);
                }
            }
            else if (event->buttons() & Qt::MidButton)
            {
                if (mViewType == object)
                {
                    qreal d = (mCamera->position() - mRoot->pos()).length();
                    vx = vx * (dx/10.0) * d * 0.015;
                    vy = vy * (dy/10.0) * d * 0.015;
                    mCamera->setPosition(mCamera->position() + vx + vy);
                }
                else if (mViewType == fly)
                {
                    vx = vx * (dx/10.0) * mCamera->distanceLimit()/250.0;
                    vy = vy * (dy/10.0) * mCamera->distanceLimit()/250.0;
                    mCamera->setPosition(mCamera->position() + vx + vy);
                }
            }
        }
    }
}

void QPanel3D::wheelEvent(QWheelEvent *event)
{
    QVector3D p1 = cursorToPoint(event->x(), event->y());
    if (this == mCamera->parent())
    {
        if (mCamera->ortho())
        {
            mCamera->setZoom(mCamera->zoom() * (1 + event->delta()/5000.0));
        }
        else
        {
            if (mViewType == object)
            {
                qreal d = mCamera->position().length();
                QVector3D dir = (p1 - mCamera->position()).normalized();
                QVector3D v = dir * (event->delta()/100.0) * d * 0.01;
                mCamera->setPosition(mCamera->position() + v);
            }
            else if (mViewType == fly)
            {
                QVector3D v = mCamera->direction() * (event->delta()/100.0 * mCamera->distanceLimit()/250.0);
                mCamera->setPosition(mCamera->position() + v);
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

void QPanel3D::renderToImage(QImage &img)
{
    mRenderingToBuffer = true;
    updateGL();
    if (mFrameBufferObject)
    {
        img = mFrameBufferObject->toImage();
        delete mFrameBufferObject;
    }
    mRenderingToBuffer = false;
}

void QPanel3D::renderText(double x, double y, double z, const QString &str, const QFont &font)
{
    if (mRenderingToBuffer)
    {
        float mm[16];
        float pm[16];
        int vp[4];
        float col[4];
        glGetFloatv(GL_MODELVIEW_MATRIX, mm);
        glGetFloatv(GL_PROJECTION_MATRIX, pm);
        glGetIntegerv(GL_VIEWPORT, vp);
        glGetFloatv(GL_CURRENT_COLOR, col);

        QMatrix4x4 modelview = QMatrix4x4(mm).transposed();
        QMatrix4x4 projection = QMatrix4x4(pm).transposed();

        QMatrix4x4 m = projection * modelview;
        QVector4D point(x, y, z, 1);
        point = m.map(point);

        int winX = lrintf(((point.x() + 1) / 2.0) * vp[2]);
        int winY = lrintf(((1 - point.y()) / 2.0) * vp[3]);

        QPainter painter(mFrameBufferObject);
        painter.setFont(font);
        QColor qcol = QColor(col[0] * 255, col[1] * 255, col[2] * 255);
        painter.setPen(qcol);
        painter.drawText(winX, winY, str);
        painter.end();
    }
    else
    {
        QGLWidget::renderText(x, y, z, str, font);
    }
}
