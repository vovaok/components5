#include "qpanel3d.h"

QPanel3D::QPanel3D(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::AlphaChannel), parent),
    mOverpainting(false),
    mouseZplane(0.5f),
    mRenderingToBuffer(false),
    mBufferSize(320, 240),
    mPickingEnabled(false),
    mPicking(false)
{
    FAutoUpdate = false;
    mainCamera = new Camera3D(this);
    camera = mainCamera;
    pRoot = new Object3D(this);
    pRoot->setVisible(true);
    ViewType = object;
    BackColor = Qt::black;

    setAutoFillBackground(false);
}

void QPanel3D::initializeGL()
{
    qglClearColor(BackColor);

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
    camera->setAspect(static_cast<qreal>(width) / static_cast<qreal>(height));
    glSettings();
}

void QPanel3D::paintGL()
{
    Camera3D *cam = camera;
    if (mRenderingToBuffer)
    {
        QGLFramebufferObjectFormat format;
        format.setAttachment(QGLFramebufferObject::Depth);
        format.setInternalTextureFormat(GL_RGBA8);
        format.setSamples(4);

        frameBufferObject = new QGLFramebufferObject(mBufferSize, format);
        frameBufferObject->bind();

        qglClearColor(Qt::transparent);
        glViewport(0, 0, static_cast<GLint>(mBufferSize.width()), static_cast<GLint>(mBufferSize.height()));
        if (mBufferCamera)
        {
            camera = mBufferCamera;
            camera->setAspect(static_cast<qreal>(mBufferSize.width()) / static_cast<qreal>(mBufferSize.height()));
        }
        gluPerspective(camera->viewAngle() / camera->zoom(), camera->aspect(), camera->nearPlane(), camera->distanceLimit());
    }

    if (this != camera->parent())
    {
        GLfloat matrix[16];
        camera->findGlobalTransform(matrix);
        camera->setTransform(matrix);
    }

    qglClearColor(BackColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    pRoot->drawObject();

    if (mRenderingToBuffer)
    {
        frameBufferObject->release();
        qglClearColor(BackColor);
        glViewport(0, 0, static_cast<GLint>(width()), static_cast<GLint>(height()));
        if (mBufferCamera)
        {
            camera = cam;
            camera->setAspect(static_cast<qreal>(width()) / static_cast<qreal>(height()));
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
    camera = value;
    camera->setAspect((GLfloat)width() / (GLfloat)height());
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
        camera->setFollowing(false);
    else if (ViewType == object)
        camera->setFollowing(true);
}

void QPanel3D::setBackColor(QColor color)
{
    BackColor = color;
    makeCurrent();
    qglClearColor(BackColor);
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

    if (camera->ortho())
    {
        int w, h, d;
        d = camera->distanceLimit();
        if (camera->fixedViewport())
        {
                w = camera->fixedViewportSize().width() / 2;
                h = camera->fixedViewportSize().height() / 2;
        }
        else
        {
                h = d / camera->zoom();
                w = h * camera->aspect();
        }
        glOrtho(-w,w, -h,h, -d,d);
    }
    else
    {
        gluPerspective(camera->viewAngle() / camera->zoom(), camera->aspect(), camera->nearPlane(), camera->distanceLimit());
    }
    gluLookAt(camera->position().x(), camera->position().y(), camera->position().z(),
              camera->target().x(), camera->target().y(), camera->target().z(),
              camera->topDir().x(), camera->topDir().y(), camera->topDir().z());

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
    glMultMatrixf(pRoot->transformMatrix());
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

    mousePoint = p2;
    mouseVector = v;
}

void QPanel3D::setMousePlane(QVector3D pointOnPlane)
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

    GLdouble win[3];

    if (gluProject(pointOnPlane.x(), pointOnPlane.y(), pointOnPlane.z(), mm, pm, vp, win, win+1, win+2))
    {
        mouseZplane = win[2];
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

    mousePos = event->pos();

    if (event->buttons() & Qt::LeftButton)
    {
        cursorToRootPoint(mousePos.x(), mousePos.y());
        emit moved(mousePoint, mouseVector);
    }

    if (this == camera->parent() && event->buttons())
    {
        if (camera->ortho())
        {
            GLfloat zoom = 2 * camera->distanceLimit() / this->height() / camera->zoom();
            QVector3D vx;
            vx = QVector3D::normal(camera->topDir(), camera->direction()) * zoom;
            QVector3D vy = camera->topDir();
            vy.normalize();
            vy = vy * zoom;

            if (event->buttons() & Qt::MidButton)
            {
                camera->setPosition(camera->position() + vx*dx + vy*dy);
            }
        }
        else
        {
            QVector3D vx, vy;
            QVector3D dir = camera->direction();
            QVector3D top = camera->topDir();
            vx = QVector3D::normal(top, dir);
            vy = QVector3D::normal(dir, vx);

            if (event->buttons() & Qt::LeftButton)
            {
                GLdouble mm[16];
                GLdouble pm[16];
                GLint vp[4];

                glPushMatrix();
                glMultMatrixf(pRoot->transformMatrix());
                glGetDoublev(GL_MODELVIEW_MATRIX, mm);
                glPopMatrix();
                glGetDoublev(GL_PROJECTION_MATRIX, pm);
                glGetIntegerv(GL_VIEWPORT, vp);

                GLdouble obj[3];
                QVector3D p1, p2;
                QVector3D v;

                if (gluUnProject(event->x()-dx, vp[3]-event->y()+dy-1, mouseZplane, mm, pm, vp, obj, obj+1, obj+2))
                {
                    p1 = QVector3D(obj[0], obj[1], obj[2]);
                    gluUnProject(event->x(), vp[3]-event->y()-1, mouseZplane, mm, pm, vp, obj, obj+1, obj+2);
                    p2 = QVector3D(obj[0], obj[1], obj[2]);
                    gluUnProject(event->x(), vp[3]-event->y()-1, 0, mm, pm, vp, obj, obj+1, obj+2);
                    QVector3D n = (QVector3D(obj[0], obj[1], obj[2]) - p2).normalized();
                    v = p2 - p1;
                    emit mouse(v, n);
                }
            }
            else if (event->buttons() & Qt::RightButton)
            {
                if (ViewType == object)
                {

                    QVector3D vv = vx*dx + vy*dy;
                    qreal angle = vv.length() * 0.5;
                    vv = QVector3D::normal(dir, vv);
                    GLfloat *mat = pRoot->transformMatrix();
                    QVector3D vm;
                    vm.setX(mat[0]*vv.x()+mat[1]*vv.y()+mat[2]*vv.z());
                    vm.setY(mat[4]*vv.x()+mat[5]*vv.y()+mat[6]*vv.z());
                    vm.setZ(mat[8]*vv.x()+mat[9]*vv.y()+mat[10]*vv.z());

                    pRoot->rotate(angle, vm.x(), vm.y(), vm.z());
                }
                else if (ViewType == fly)
                {
                    camera->setTopDir(vy);
                    vx = vx * (dx/360.0);
                    vy = vy * (dy/360.0);
                    camera->setDirection(camera->direction() + vy + vx);
                }
            }
            else if (event->buttons() & Qt::MidButton)
            {
                if (ViewType == object)
                {
                    qreal d = (camera->position() - pRoot->pos()).length();
                    vx = vx * (dx/10.0) * d * 0.015;
                    vy = vy * (dy/10.0) * d * 0.015;
                    camera->setPosition(camera->position() + vx + vy);
                }
                else if (ViewType == fly)
                {
                    vx = vx * (dx/10.0) * camera->distanceLimit()/250.0;
                    vy = vy * (dy/10.0) * camera->distanceLimit()/250.0;
                    camera->setPosition(camera->position() + vx + vy);
                }
            }
        }
    }
}

void QPanel3D::wheelEvent(QWheelEvent *event)
{
    QVector3D p1 = cursorToPoint(event->x(), event->y());
    if (this == camera->parent())
    {
        if (camera->ortho())
        {
            camera->setZoom(camera->zoom() * (1 + event->delta()/5000.0));
        }
        else
        {
            if (ViewType == object)
            {
                qreal d = camera->position().length();
                QVector3D dir = (p1 - camera->position()).normalized();
                QVector3D v = dir * (event->delta()/100.0) * d * 0.01;
                camera->setPosition(camera->position() + v);
            }
            else if (ViewType == fly)
            {
                QVector3D v = camera->direction() * (event->delta()/100.0 * camera->distanceLimit()/250.0);
                camera->setPosition(camera->position() + v);
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
    if (frameBufferObject)
    {
        img = frameBufferObject->toImage();
        delete frameBufferObject;
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

        QPainter painter(frameBufferObject);
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
