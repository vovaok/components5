#include "object3d.h"
#include "qpanel3d.h"

Object3D::Object3D(QObject *parent) :
    QObject(parent),
    mPanel(0L),
    FSettingsChanged(false),
    FVisible(true),
    boundsVisible(false),
    axesVisible(false),
    mFullDrawTime(0), mDrawTime(0),
    mTexture(0L),
    mPickable(true),
    mWireframe(false)
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
        {
            transform[i*4+j]=i==j?1:0;
            fullTransform[i*4+j]=i==j?1:0;
        }

    xrot = yrot = zrot = 0;
    xcen = ycen = zcen = 0;
    xori = yori = zori = 0;
    xsc = ysc = zsc = 1.0;

    setParent(parent);

    setColor(Qt::red);
}

QPanel3D *Object3D::scene() const
{
    return dynamic_cast<QPanel3D*>(mPanel);
}

void Object3D::setParent(QObject *parent)
{
    if (mPanel)
    {
        disconnect(this, SIGNAL(changed()), mPanel, SLOT(sceneChanged()));
        disconnect(this, SIGNAL(settingsChanged()), mPanel, SLOT(glSettings()));
        disconnect(mPanel, SIGNAL(onUpdate()), this, SLOT(update()));
    }

    mPanel = 0L;
    Object3D *par = qobject_cast<Object3D*>(parent);
    if (par)
    {
        mPanel = par->mPanel;
    }
    else
    {
        QPanel3D *panel3d = qobject_cast<QPanel3D*>(parent);
        if (panel3d)
            mPanel = panel3d;
    }

    if (mPanel)
    {
        connect(this, SIGNAL(changed()), mPanel, SLOT(sceneChanged()));
        connect(this, SIGNAL(settingsChanged()), mPanel, SLOT(glSettings()));
        connect(mPanel, SIGNAL(onUpdate()), this, SLOT(update()));
    }

    QObject::setParent(parent);
}

void Object3D::update()
{
    if (FSettingsChanged)
        applySettings();
}

void Object3D::setSettingsChanged()
{
    FSettingsChanged = true;
    emit settingsChanged();
    FSettingsChanged = false;
    emit changed();
}

void Object3D::drawObject()
{
    QElapsedTimer etimer;
    etimer.start();

    if (mPanel && scene()->mPicking)
    {
        glPushName(parent()->children().indexOf(this));
    }

    glPushMatrix();

    glMultMatrixf(transform);

//    glTranslatef(xpos, ypos, zpos);
//    glRotatef(xrot, 1, 0, 0);
//    glRotatef(yrot, 0, 1, 0);
//    glRotatef(zrot, 0, 0, 1);

    if (axesVisible && !scene()->mPicking)
    {
        QVector3D mn = getMinBounds();
        QVector3D mx = getMaxBounds();
        QVector3D mm = mx - mn;
        qreal sz = mm.length();
        qreal maxd = mx.x()>mx.y()? mx.x(): mx.y();
        maxd = mx.z()>maxd? mx.z(): maxd;
        maxd *= 1.2;
        qreal r = sz/50;
//        GLfloat par[4] = {1.0, 1.0, 1.0, 1.0};
//
//        glMaterialfv(GL_FRONT, GL_EMISSION, par);

        glEnable(GL_COLOR_MATERIAL);

        GLUquadricObj *quad = gluNewQuadric();
        glColor3f(1, 1, 1);
        gluSphere(quad, 2*r, 10, 10);
        glColor3f(0, 0, 1);
        glPushMatrix();
            gluCylinder(quad, r, r, maxd, 10, 1);
            glTranslatef(0, 0, maxd);
            gluCylinder(quad, r*2, 0, r*8, 10, 5);
            glRotatef(180, 0, 1, 0);
            gluDisk(quad, 0, 2*r, 10, 5);
        glPopMatrix();
        glColor3f(1, 0, 0);
        glPushMatrix();
            glRotatef(90, 0, 1, 0);
            gluCylinder(quad, r, r, maxd, 10, 1);
            glTranslatef(0, 0, maxd);
            gluCylinder(quad, r*2, 0, r*8, 10, 5);
            glRotatef(180, 0, 1, 0);
            gluDisk(quad, 0, 2*r, 10, 5);
        glPopMatrix();
        glColor3f(0, 1, 0);
        glPushMatrix();
            glRotatef(90, -1, 0, 0);
            gluCylinder(quad, r, r, maxd, 10, 1);
            glTranslatef(0, 0, maxd);
            gluCylinder(quad, r*2, 0, r*8, 10, 5);
            glRotatef(180, 0, 1, 0);
            gluDisk(quad, 0, 2*r, 10, 5);
        glPopMatrix();
        gluDeleteQuadric(quad);

        glDisable(GL_COLOR_MATERIAL);
    }

    glRotatef(xori, 1, 0, 0);
    glRotatef(yori, 0, 1, 0);
    glRotatef(zori, 0, 0, 1);
    glTranslatef(xcen, ycen, zcen);
    glScalef(xsc, ysc, zsc);

    bool backfacing = (xsc * ysc * zsc < 0);
    if (backfacing)
        glCullFace(GL_FRONT);

    // draw bounding box
    if (boundsVisible && !scene()->mPicking)
    {
        QVector3D mn = getMinBounds();
        QVector3D mx = getMaxBounds();

        GLboolean lighting;
        glGetBooleanv(GL_LIGHTING, &lighting);
        glDisable(GL_LIGHTING);
        glColor4f(1.0, 1.0, 1.0, 1.0);

        glBegin(GL_LINES);
            glVertex3f(mn.x(), mn.y(), mn.z()); glVertex3f(mn.x(), mn.y(), mx.z());
            glVertex3f(mn.x(), mx.y(), mn.z()); glVertex3f(mn.x(), mx.y(), mx.z());
            glVertex3f(mx.x(), mn.y(), mn.z()); glVertex3f(mx.x(), mn.y(), mx.z());
            glVertex3f(mx.x(), mx.y(), mn.z()); glVertex3f(mx.x(), mx.y(), mx.z());
        glEnd();
        glBegin(GL_LINE_LOOP);
            glVertex3f(mn.x(), mn.y(), mn.z());
            glVertex3f(mx.x(), mn.y(), mn.z());
            glVertex3f(mx.x(), mx.y(), mn.z());
            glVertex3f(mn.x(), mx.y(), mn.z());
        glEnd();
        glBegin(GL_LINE_LOOP);
            glVertex3f(mn.x(), mn.y(), mx.z());
            glVertex3f(mx.x(), mn.y(), mx.z());
            glVertex3f(mx.x(), mx.y(), mx.z());
            glVertex3f(mn.x(), mx.y(), mx.z());
        glEnd();

        if (lighting)
            glEnable(GL_LIGHTING);

//        par[0] = par[1] = par[2] = 0.0;
//        glMaterialfv(GL_FRONT, GL_EMISSION, par);
    }

    if ((FVisible && !scene()->mPicking) || (mPickable && scene()->mPicking))
    {
        if (glIsEnabled(GL_LIGHTING))
        {
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mDiffusefv);
            glMaterialfv(GL_FRONT, GL_AMBIENT, mAmbientfv);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mSpecularfv);
            glMaterialfv(GL_FRONT, GL_EMISSION, mEmissionfv);
            glMateriali(GL_FRONT, GL_SHININESS, mShininess);
        }
        else
        {
            glColor3f(mDiffuseColor.redF(), mDiffuseColor.greenF(), mDiffuseColor.blueF());
        }

        if (mTexture)
            mTexture->bind();

        QElapsedTimer et2;
        et2.start();

        if (mWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        draw();

        if (mWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        mDrawTime = et2.nsecsElapsed() * 1.0e-9;

        if (mTexture)
            mTexture->disable();
    }

    if (!scene()->mPicking || mPickable)
    {
        QObjectList::const_iterator it;
        for (it=children().begin(); it!=children().end(); it++)
        {
            Object3D *obj = qobject_cast<Object3D*>(*it);
            if (!obj)
                continue;
            obj->drawObject();
        }
    }

    if (backfacing)
        glCullFace(GL_BACK);

    glPopMatrix();

    if (mPanel && scene()->mPicking)
        glPopName();

    mFullDrawTime = etimer.nsecsElapsed() * 1.0e-9;
}

void Object3D::draw()
{
//    glBegin(GL_LINES);
//    glColor3f(1, 0, 0);
//    glVertex3f(0, 0, 0); glVertex3f(10, 0, 0);
//    glColor3f(0, 1, 0);
//    glVertex3f(0, 0, 0); glVertex3f(0, 10, 0);
//    glColor3f(0, 0, 1);
//    glVertex3f(0, 0, 0); glVertex3f(0, 0, 10);
//    glEnd();
//    glColor3f(1, 1, 1);
}

void Object3D::findGlobalTransform(GLfloat *matrix)
{
    QList<Object3D*> objs;
    for (Object3D *obj = this; obj && obj != mPanel; obj = dynamic_cast<Object3D*>(obj->parent()))
        objs.prepend(obj);

    glPushMatrix();
    glLoadIdentity();
    foreach (Object3D *o, objs)
        glMultMatrixf(o->fullTransformMatrix());
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    glPopMatrix();
}
//----------------------------------------------------------

void Object3D::setVisible(bool visible)
{
    FVisible = visible;
    emit changed();
}
//----------------------------------------------------------

void Object3D::setTransform(const GLfloat *matrix)
{
    for (int i=0; i<16; i++)
        transform[i] = matrix[i];
    computeFullTransform();
    emit changed();
}
//----------------------------------------------------------

void Object3D::setXPos(qreal pos)
{
    transform[12] = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setYPos(qreal pos)
{
    transform[13] = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setZPos(qreal pos)
{
    transform[14] = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setPosition(QVector3D pos)
{
    transform[12] = pos.x();
    transform[13] = pos.y();
    transform[14] = pos.z();
    computeFullTransform();
    emit changed();
}

void Object3D::setPosition(qreal x, qreal y, qreal z)
{
    transform[12] = x;
    transform[13] = y;
    transform[14] = z;
    computeFullTransform();
    emit changed();
}
//----------------------------------------------------------

void Object3D::setXRot(qreal angle)
{
    xrot = angle;
    applyRotation();
    emit changed();
}

void Object3D::setYRot(qreal angle)
{
    yrot = angle;
    applyRotation();
    emit changed();
}

void Object3D::setZRot(qreal angle)
{
    zrot = angle;
    applyRotation();
    emit changed();
}

void Object3D::setRotation(QVector3D rot)
{
    xrot = rot.x();
    yrot = rot.y();
    zrot = rot.z();
    applyRotation();
    emit changed();
}

void Object3D::setRotation(qreal x, qreal y, qreal z)
{
    xrot = x;
    yrot = y;
    zrot = z;
    applyRotation();
    emit changed();
}

void Object3D::setRotation(const QQuaternion &q)
{
    float w,x,y,z;
    w = q.scalar();
    x = q.x();
    y = q.y();
    z = q.z();

    transform[0]  = 1.0f - 2.0f*( y*y + z*z );
    transform[1]  = 2.0f * ( x*y + z*w);
    transform[2]  = 2.0f * ( x*z - y*w);
    transform[4]  = 2.0f * ( x*y - z*w );
    transform[5]  = 1.0f - 2.0f*( x*x + z*z );
    transform[6]  = 2.0f * ( z*y + x*w );
    transform[8]  = 2.0f * ( x*z + y*w );
    transform[9]  = 2.0f * ( y*z - x*w );
    transform[10] = 1.0f - 2.0f*( x*x + y*y );

    computeFullTransform();
    emit changed();
}

void Object3D::setRotation(const QMatrix3x3 &m)
{
    transform[0]  = m(0,0);
    transform[1]  = m(1,0);
    transform[2]  = m(2,0);
    transform[4]  = m(0,1);
    transform[5]  = m(1,1);
    transform[6]  = m(2,1);
    transform[8]  = m(0,2);
    transform[9]  = m(1,2);
    transform[10] = m(2,2);
    computeFullTransform();
    emit changed();
}

void Object3D::rotate(qreal angle, qreal vx, qreal vy, qreal vz)
{
    glPushMatrix();
    glLoadMatrixf(transform);
    glRotatef(angle, vx, vy, vz);
    glGetFloatv(GL_MODELVIEW_MATRIX, transform);
    glPopMatrix();
    computeFullTransform();
    emit changed();
}

void Object3D::applyRotation()
{
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            transform[i*4+j]=i==j?1:0;
    glPushMatrix();
    glLoadMatrixf(transform);
    glRotatef(xrot, 1, 0, 0);
    glRotatef(yrot, 0, 1, 0);
    glRotatef(zrot, 0, 0, 1);
    glGetFloatv(GL_MODELVIEW_MATRIX, transform);
    glPopMatrix();
    computeFullTransform();
}
//----------------------------------------------------------

void Object3D::setXCenter(qreal pos)
{
    xcen = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setYCenter(qreal pos)
{
    ycen = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setZCenter(qreal pos)
{
    zcen = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setCenter(QVector3D center)
{
    xcen = center.x();
    ycen = center.y();
    zcen = center.z();
    computeFullTransform();
    emit changed();
}

void Object3D::setCenter(qreal x, qreal y, qreal z)
{
    xcen = x;
    ycen = y;
    zcen = z;
    computeFullTransform();
    emit changed();
}
//----------------------------------------------------------

void Object3D::setXOrient(qreal angle)
{
    xori = angle;
    computeFullTransform();
    emit changed();
}

void Object3D::setYOrient(qreal angle)
{
    yori = angle;
    computeFullTransform();
    emit changed();
}

void Object3D::setZOrient(qreal angle)
{
    zori = angle;
    computeFullTransform();
    emit changed();
}

void Object3D::setOrient(QVector3D orient)
{
    xori = orient.x();
    yori = orient.y();
    zori = orient.z();
    computeFullTransform();
    emit changed();
}

void Object3D::setOrient(qreal x, qreal y, qreal z)
{
    xori = x;
    yori = y;
    zori = z;
    computeFullTransform();
    emit changed();
}
//---------------------------------------------------------

void Object3D::setXScale(qreal scale)
{
    xsc = scale;
    computeFullTransform();
    emit changed();
}

void Object3D::setYScale(qreal scale)
{
    ysc = scale;
    computeFullTransform();
    emit changed();
}

void Object3D::setZScale(qreal scale)
{
    zsc = scale;
    computeFullTransform();
    emit changed();
}

void Object3D::setUniformScale(qreal scale)
{
    xsc = ysc = zsc = scale;
    computeFullTransform();
    emit changed();
}
//----------------------------------------------------------

void Object3D::computeFullTransform()
{
    glPushMatrix();
    glLoadMatrixf(transform);
    glRotatef(xori, 1, 0, 0);
    glRotatef(yori, 0, 1, 0);
    glRotatef(zori, 0, 0, 1);
    glTranslatef(xcen, ycen, zcen);
    glScalef(xsc, ysc, zsc);
    glGetFloatv(GL_MODELVIEW_MATRIX, fullTransform);
    glPopMatrix();
}

void Object3D::setColor(QColor diffuse, QColor specular, QColor emission, float ambient, int shininess)
{
    mDiffuseColor = diffuse;
    mSpecularColor = specular;
    mEmissionColor = emission;
    mAmbientColor.setRed(mDiffuseColor.red() * ambient);
    mAmbientColor.setGreen(mDiffuseColor.green() * ambient);
    mAmbientColor.setBlue(mDiffuseColor.blue() * ambient);
    mAmbientColor.setAlpha(mDiffuseColor.alpha());
    mShininess = shininess;

    mAmbientfv[0] = mAmbientColor.redF(); mAmbientfv[1] = mAmbientColor.greenF(); mAmbientfv[2] = mAmbientColor.blueF(); mAmbientfv[3] = mAmbientColor.alphaF();
    mDiffusefv[0] = mDiffuseColor.redF(); mDiffusefv[1] = mDiffuseColor.greenF(); mDiffusefv[2] = mDiffuseColor.blueF(); mDiffusefv[3] = mDiffuseColor.alphaF();
    mSpecularfv[0] = mSpecularColor.redF(); mSpecularfv[1] = mSpecularColor.greenF(); mSpecularfv[2] = mSpecularColor.blueF(); mSpecularfv[3] = mSpecularColor.alphaF();
    mEmissionfv[0] = mEmissionColor.redF(); mEmissionfv[1] = mEmissionColor.greenF(); mEmissionfv[2] = mEmissionColor.blueF(); mEmissionfv[3] = mEmissionColor.alphaF();

    emit changed();
}

void Object3D::setDiffuseColor(QColor color)
{
    mDiffuseColor = color;
    mDiffusefv[0] = mDiffuseColor.redF(); mDiffusefv[1] = mDiffuseColor.greenF(); mDiffusefv[2] = mDiffuseColor.blueF(); mDiffusefv[3] = mDiffuseColor.alphaF();
    emit changed();
}

void Object3D::setSpecularColor(QColor color)
{
    mSpecularColor = color;
    mSpecularfv[0] = mSpecularColor.redF(); mSpecularfv[1] = mSpecularColor.greenF(); mSpecularfv[2] = mSpecularColor.blueF(); mSpecularfv[3] = mSpecularColor.alphaF();
    emit changed();
}

void Object3D::setEmissionColor(QColor color)
{
    mEmissionColor = color;
    mEmissionfv[0] = mEmissionColor.redF(); mEmissionfv[1] = mEmissionColor.greenF(); mEmissionfv[2] = mEmissionColor.blueF(); mEmissionfv[3] = mEmissionColor.alphaF();
    emit changed();
}

void Object3D::setAmbient(float value)
{
    mAmbientColor.setRed(mDiffuseColor.red() * value);
    mAmbientColor.setGreen(mDiffuseColor.green() * value);
    mAmbientColor.setBlue(mDiffuseColor.blue() * value);
    mAmbientColor.setAlpha(mDiffuseColor.alpha());
    mAmbientfv[0] = mAmbientColor.redF(); mAmbientfv[1] = mAmbientColor.greenF(); mAmbientfv[2] = mAmbientColor.blueF(); mAmbientfv[3] = mAmbientColor.alphaF();
    emit changed();
}

void Object3D::setShininess(int value)
{
    mShininess = value;
    emit changed();
}

void Object3D::setDefaultColor()
{
    float a = mDefDiffuseColor.redF()? mDefAmbientColor.redF() / mDefDiffuseColor.redF(): 1;
    setColor(mDefDiffuseColor, mDefSpecularColor, mDefEmissionColor, a, mDefShininess);
}

void Object3D::assignDefColor(GLfloat *a, GLfloat *d, GLfloat *e, GLfloat *s, int sh)
{
    mDefShininess = sh;

    for (int i=0; i<4; i++)
    {
        mAmbientfv[i] = a[i];
        mDiffusefv[i] = d[i];
        mSpecularfv[i] = s[i];
        mEmissionfv[i] = e[i];
    }

    mDefDiffuseColor = QColor::fromRgbF(d[0], d[1], d[2], d[3]);
    mDefAmbientColor = QColor::fromRgbF(a[0], a[1], a[2], a[3]);
    mDefEmissionColor = QColor::fromRgbF(e[0], e[1], e[2], e[3]);
    mDefSpecularColor = QColor::fromRgbF(s[0], s[1], s[2], s[3]);

    setDefaultColor();
}
//----------------------------------------------------------

void Object3D::setTexture(const QImage &image)
{
    setTexture(new StaticTexture(dynamic_cast<QPanel3D*>(mPanel), image));
}
//----------------------------------------------------------

void Object3D::showBounds(bool value)
{
    if (boundsVisible == value)
        return;
    boundsVisible = value;
    emit changed();
}

void Object3D::showAxes(bool value)
{
    if (axesVisible == value)
        return;
    axesVisible = value;
    emit changed();
}
//----------------------------------------------------------

QVector3D Object3D::getMinBounds()
{
    return QVector3D(-10, -10, -10);
}

QVector3D Object3D::getMaxBounds()
{
    return QVector3D(10, 10, 10);
}
//----------------------------------------------------------
