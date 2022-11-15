#include "object3d.h"
#include "qpanel3d.h"

Object3D::Object3D(QObject *parent) :
    QObject(parent),
    mPanel(nullptr),
    FSettingsChanged(false),
    FVisible(true),
    boundsVisible(false),
    axesVisible(false),
    mFullDrawTime(0),
    mTexture(nullptr),
    mPickable(true),
    mWireframe(false),
    m_depthTestDisabled(false)
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

    mPanel = nullptr;
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
    if (FSettingsChanged) applySettings();
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
        glPushName(uint(parent()->children().indexOf(this)));
    }

    glPushMatrix();

    glMultMatrixf(transform);

    if (axesVisible && !scene()->mPicking)
    {
        QVector3D mn = getMinBounds();
        QVector3D mx = getMaxBounds();
        QVector3D mm = mx - mn;
        float sz = mm.length();
        float maxd = mx.x() > mx.y() ? mx.x() : mx.y();
        maxd = mx.z() > maxd? mx.z() : maxd;
        maxd *= 1.2f;
        qreal r = qreal(sz / 50);

        glEnable(GL_COLOR_MATERIAL);

        GLUquadricObj *quad = gluNewQuadric();
        glColor3f(1, 1, 1);
        gluSphere(quad, 2*r, 10, 10);
        glColor3f(0, 0, 1);
        glPushMatrix();
            gluCylinder(quad, r, r, double(maxd), 10, 1);
            glTranslatef(0, 0, maxd);
            gluCylinder(quad, r*2, 0, r*8, 10, 5);
            glRotatef(180, 0, 1, 0);
            gluDisk(quad, 0, 2*r, 10, 5);
        glPopMatrix();
        glColor3f(1, 0, 0);
        glPushMatrix();
            glRotatef(90, 0, 1, 0);
            gluCylinder(quad, r, r, double(maxd), 10, 1);
            glTranslatef(0, 0, maxd);
            gluCylinder(quad, r * 2, 0, r * 8, 10, 5);
            glRotatef(180, 0, 1, 0);
            gluDisk(quad, 0, 2*r, 10, 5);
        glPopMatrix();
        glColor3f(0, 1, 0);
        glPushMatrix();
            glRotatef(90, -1, 0, 0);
            gluCylinder(quad, r, r, double(maxd), 10, 1);
            glTranslatef(0, 0, maxd);
            gluCylinder(quad, r * 2, 0, r * 8, 10, 5);
            glRotatef(180, 0, 1, 0);
            gluDisk(quad, 0, 2 * r, 10, 5);
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
            glColor3f(float(mDiffuseColor.redF()),
                      float(mDiffuseColor.greenF()),
                      float(mDiffuseColor.blueF()));
        }

        if (mTexture)
            mTexture->bind();

        if (mWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (m_depthTestDisabled)
            glDisable(GL_DEPTH_TEST);

        draw();

        if (m_depthTestDisabled)
            glEnable(GL_DEPTH_TEST);

        if (mWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (mTexture)
            mTexture->disable();
    }

    if (!scene()->mPicking || mPickable)
    {
        QObjectList::const_iterator it;
        for (it=children().begin(); it!=children().end(); it++)
        {
            Object3D *obj = qobject_cast<Object3D*>(*it);

            if (!obj) continue;

            obj->drawObject();
        }
    }

    if (backfacing)
        glCullFace(GL_BACK);

    glPopMatrix();

    if (mPanel && scene()->mPicking)
        glPopName();

    mFullDrawTime = etimer.nsecsElapsed() * 1.0e-9f;
}

void Object3D::draw()
{
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

void Object3D::findRootTransform(GLfloat *matrix)
{
    Object3D *root = dynamic_cast<QPanel3D*>(mPanel)->root();

    QList<Object3D*> objs;
    for (Object3D *obj = this; obj && obj != root; obj = dynamic_cast<Object3D*>(obj->parent()))
        objs.prepend(obj);

    glPushMatrix();
    glLoadIdentity();
    foreach (Object3D *o, objs)
        glMultMatrixf(o->fullTransformMatrix());
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    glPopMatrix();
}

void Object3D::setVisible(bool visible)
{
    FVisible = visible;
    emit changed();
}

void Object3D::setTransform(const GLfloat *matrix)
{
    for (int i=0; i<16; i++)
        transform[i] = matrix[i];
    computeFullTransform();
    emit changed();
}

void Object3D::setXPos(float pos)
{
    transform[12] = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setYPos(float pos)
{
    transform[13] = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setZPos(float pos)
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

void Object3D::setPosition(float x, float y, float z)
{
    transform[12] = x;
    transform[13] = y;
    transform[14] = z;
    computeFullTransform();
    emit changed();
}

void Object3D::setXRot(float angle)
{
    xrot = angle;
    applyRotation();
    emit changed();
}

void Object3D::setYRot(float angle)
{
    yrot = angle;
    applyRotation();
    emit changed();
}

void Object3D::setZRot(float angle)
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

void Object3D::setRotation(float x, float y, float z)
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

void Object3D::rotate(float angle, float vx, float vy, float vz)
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

    QMatrix4x4 T;
    T.rotate(xrot, 1, 0, 0);
    T.rotate(yrot, 0, 1, 0);
    T.rotate(zrot, 0, 0, 1);

    transform[0]  = T(0, 0);
    transform[1]  = T(1, 0);
    transform[2]  = T(2, 0);
    transform[4]  = T(0, 1);
    transform[5]  = T(1, 1);
    transform[6]  = T(2, 1);
    transform[8]  = T(0, 2);
    transform[9]  = T(1, 2);
    transform[10] = T(2, 2);

    computeFullTransform();
}

void Object3D::setXCenter(float pos)
{
    xcen = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setYCenter(float pos)
{
    ycen = pos;
    computeFullTransform();
    emit changed();
}

void Object3D::setZCenter(float pos)
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

void Object3D::setCenter(float x, float y, float z)
{
    xcen = x;
    ycen = y;
    zcen = z;
    computeFullTransform();
    emit changed();
}
//----------------------------------------------------------

void Object3D::setXOrient(float angle)
{
    xori = angle;
    computeFullTransform();
    emit changed();
}

void Object3D::setYOrient(float angle)
{
    yori = angle;
    computeFullTransform();
    emit changed();
}

void Object3D::setZOrient(float angle)
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

void Object3D::setOrient(float x, float y, float z)
{
    xori = x;
    yori = y;
    zori = z;
    computeFullTransform();
    emit changed();
}

void Object3D::setXScale(float scale)
{
    xsc = scale;
    computeFullTransform();
    emit changed();
}

void Object3D::setYScale(float scale)
{
    ysc = scale;
    computeFullTransform();
    emit changed();
}

void Object3D::setZScale(float scale)
{
    zsc = scale;
    computeFullTransform();
    emit changed();
}

void Object3D::setUniformScale(float scale)
{
    xsc = ysc = zsc = scale;
    computeFullTransform();
    emit changed();
}

void Object3D::computeFullTransform()
{

    QMatrix4x4 T(transform);
    T.rotate(xori, 1, 0, 0);
    T.rotate(yori, 0, 1, 0);
    T.rotate(zori, 0, 0, 1);
    T.translate(xcen, ycen, zcen);
    T.scale(xsc, ysc, zsc);
    T.copyDataTo(fullTransform);
}

void Object3D::setColor(QColor diffuse, QColor specular, QColor emission, float ambient, int shininess)
{
    mDiffuseColor = diffuse;
    mSpecularColor = specular;
    mEmissionColor = emission;
    mAmbientColor.setRed(int(mDiffuseColor.red() * ambient));
    mAmbientColor.setGreen(int(mDiffuseColor.green() * ambient));
    mAmbientColor.setBlue(int(mDiffuseColor.blue() * ambient));
    mAmbientColor.setAlpha(mDiffuseColor.alpha());
    mShininess = shininess;

    mAmbientfv[0]  = float(mAmbientColor.redF());  mAmbientfv[1]  = float(mAmbientColor.greenF());  mAmbientfv[2]  = float(mAmbientColor.blueF());  mAmbientfv[3]  = float(mAmbientColor.alphaF());
    mDiffusefv[0]  = float(mDiffuseColor.redF());  mDiffusefv[1]  = float(mDiffuseColor.greenF());  mDiffusefv[2]  = float(mDiffuseColor.blueF());  mDiffusefv[3]  = float(mDiffuseColor.alphaF());
    mSpecularfv[0] = float(mSpecularColor.redF()); mSpecularfv[1] = float(mSpecularColor.greenF()); mSpecularfv[2] = float(mSpecularColor.blueF()); mSpecularfv[3] = float(mSpecularColor.alphaF());
    mEmissionfv[0] = float(mEmissionColor.redF()); mEmissionfv[1] = float(mEmissionColor.greenF()); mEmissionfv[2] = float(mEmissionColor.blueF()); mEmissionfv[3] = float(mEmissionColor.alphaF());

    emit changed();
}

void Object3D::setDiffuseColor(QColor color)
{
    mDiffuseColor = color;
    mDiffusefv[0] = float(mDiffuseColor.redF()); mDiffusefv[1] = float(mDiffuseColor.greenF()); mDiffusefv[2] = float(mDiffuseColor.blueF()); mDiffusefv[3] = float(mDiffuseColor.alphaF());
    emit changed();
}

void Object3D::setSpecularColor(QColor color)
{
    mSpecularColor = color;
    mSpecularfv[0] = float(mSpecularColor.redF()); mSpecularfv[1] = float(mSpecularColor.greenF()); mSpecularfv[2] = float(mSpecularColor.blueF()); mSpecularfv[3] = float(mSpecularColor.alphaF());
    emit changed();
}

void Object3D::setEmissionColor(QColor color)
{
    mEmissionColor = color;
    mEmissionfv[0] = float(mEmissionColor.redF()); mEmissionfv[1] = float(mEmissionColor.greenF()); mEmissionfv[2] = float(mEmissionColor.blueF()); mEmissionfv[3] = float(mEmissionColor.alphaF());
    emit changed();
}

void Object3D::setAmbient(float value)
{
    mAmbientColor.setRed(int(mDiffuseColor.red() * value));
    mAmbientColor.setGreen(int(mDiffuseColor.green() * value));
    mAmbientColor.setBlue(int(mDiffuseColor.blue() * value));
    mAmbientColor.setAlpha(int(mDiffuseColor.alpha()));
    mAmbientfv[0] = float(mAmbientColor.redF()); mAmbientfv[1] = float(mAmbientColor.greenF()); mAmbientfv[2] = float(mAmbientColor.blueF()); mAmbientfv[3] = float(mAmbientColor.alphaF());
    emit changed();
}

void Object3D::setShininess(int value)
{
    mShininess = value;
    emit changed();
}

void Object3D::setDefaultColor()
{
    float a = mDefDiffuseColor.redF() != 0.0 ? float(mDefAmbientColor.redF() / mDefDiffuseColor.redF()) : 1.0f;
    setColor(mDefDiffuseColor, mDefSpecularColor, mDefEmissionColor, a, mDefShininess);
}

void Object3D::assignDefColor(GLfloat *a, GLfloat *d, GLfloat *e, GLfloat *s, int sh)
{
    mDefShininess = sh;

    for (int i = 0; i < 4; i++)
    {
        mAmbientfv[i] = a[i];
        mDiffusefv[i] = d[i];
        mSpecularfv[i] = s[i];
        mEmissionfv[i] = e[i];
    }

    mDefDiffuseColor  = QColor::fromRgbF(qreal(d[0]), qreal(d[1]), qreal(d[2]), qreal(d[3]));
    mDefAmbientColor  = QColor::fromRgbF(qreal(a[0]), qreal(a[1]), qreal(a[2]), qreal(a[3]));
    mDefEmissionColor = QColor::fromRgbF(qreal(e[0]), qreal(e[1]), qreal(e[2]), qreal(e[3]));
    mDefSpecularColor = QColor::fromRgbF(qreal(s[0]), qreal(s[1]), qreal(s[2]), qreal(s[3]));

    setDefaultColor();
}
//----------------------------------------------------------

void Object3D::setTexture(const QImage &image)
{
    auto scene = dynamic_cast<QPanel3D*>(mPanel);
    setTexture(new StaticTexture(scene, image));
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
