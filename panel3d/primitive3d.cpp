#include "primitive3d.h"

Primitive3D::Primitive3D(QObject *parent) :
    Object3D(parent)
{
    ptype = box;
    pslices = 10;
    pstacks = 10;
}

Primitive3D::Primitive3D(Type type, QObject *parent) :
    Object3D(parent)
{
    ptype = type;
    pslices = 10;
    pstacks = 10;
}
//----------------------------------------------------------

void Primitive3D::draw()
{
    if (ptype == polygon)
    {
        glNormal3f(0, 0, 1);
        glBegin(GL_POLYGON);
        foreach (QPointF p, mPoints)
        {
            glVertex3f(p.x(), p.y(), 0);
        }
        glEnd();
    }
    else if (ptype == plane)
    {
        QVector3D vx = QVector3D(data[0], data[1], data[2]);
        QVector3D vy = QVector3D(data[3], data[4], data[5]);
        QVector3D n = QVector3D::crossProduct(vx, vy);
        n.normalize();
        glNormal3f(n.x(), n.y(), n.z());
        QVector3D p0 = (-vx - vy) * 0.5;
        float dx = 1.0 / pslices;
        float dy = 1.0 / pstacks;
        for (int y=0; y<pstacks; y++)
        {
            glBegin(GL_QUAD_STRIP);
            for (int x=pslices; x>=0; x--)
            {
                QVector3D p = p0 + vx*(x*dx) + vy*(y*dy);
                if (mTexture)
                    glTexCoord2f(x*dx, y*dy);
                glVertex3f(p.x(), p.y(), p.z());
                if (mTexture)
                    glTexCoord2f(x*dx, (y+1)*dy);
                p += vy*dy;
                glVertex3f(p.x(), p.y(), p.z());
            }
            glEnd();
        }
        return;
    }
//    else if (ptype == box)
//    {
//        QVector3D v1 = QVector3D(-data[0]/2, -data[1]/2, -data[2]/2);
//        QVector3D v2 = QVector3D(data[0]/2, data[1]/2, data[2]/2);
//        QVector3D dv = v2 - v1;
//        int N = pslices * pstacks;
//        float p = data[0] + data[1] + data[2];
//        int Nx = abs(lrintf(N * data[0] / p));
//        int Ny = abs(lrintf(N * data[1] / p));
//        int Nz = abs(lrintf(N * data[2] / p));
//        glNormal3f(0, 0, 1);
//        for (int i=0; i<Ny; i++)
//        {
//            glBegin(GL_QUAD_STRIP);
//            for (int j=0; j<=Nx; j++)
//            {
//                float px = v1.x() + dv.x() * j / Nx;
//                float py = v1.y() + dv.y() * i / Ny;
//                float py1 = v1.y() + dv.y() * (i+1) / Ny;
//                if (mTexture)
//                    glTexCoord2f((float)j / Nx, (float)(i+1) / Ny);
//                glVertex3f(px, py1, v2.z());
//                if (mTexture)
//                    glTexCoord2f((float)j / Nx, (float)(i) / Ny);
//                glVertex3f(px, py, v2.z());
//            }
//            glEnd();
//        }

//        return;
//    }

    glPushMatrix();
    GLUquadricObj *quad = gluNewQuadric();
    switch (ptype)
    {
      case box:
        if (mTexture)
            gluQuadricTexture(quad, true);
        glNormal3f(0, 0, 1);
        glTranslatef(-data[0]/2, -data[1]/2, data[2]/2);
        for (int i=0; i<2; i++)
        {
            glRectf(0, 0, data[0], data[1]);
            glTranslatef(data[0], 0, 0);
            glRotatef(90, 0, 1, 0);
            glRectf(0, 0, data[2], data[1]);
            glTranslatef(data[2], 0, 0);
            glRotatef(90, 0, 1, 0);
        }
        glRotatef(90, 1, 0, 0);
        glTranslatef(0, -data[2], 0);
        glRectf(0, 0, data[0], data[2]);
        glTranslatef(0, 0, -data[1]);
        glRotatef(180, 1, 0, 0);
        glTranslatef(0, -data[2], 0);
        glRectf(0, 0, data[0], data[2]);
        break;

      case sphere:
        if (mTexture)
            gluQuadricTexture(quad, true);
        gluSphere(quad, data[0], pslices, pstacks);
        break;

      case cylinder:
        if (mTexture)
            gluQuadricTexture(quad, true);
        gluCylinder(quad, data[0], data[0], data[1], pslices, pstacks);
        if (mTexture)
            gluQuadricTexture(quad, false);
        glPushMatrix();
        glRotatef(180, 0, 1, 0);
        gluDisk(quad, 0, data[0], pslices, pstacks);
        glPopMatrix();
        glTranslatef(0, 0, data[1]);
        gluDisk(quad, 0, data[0], pslices, pstacks);
        break;

      case cone:
        if (mTexture)
            gluQuadricTexture(quad, true);
        gluCylinder(quad, data[0], data[1], data[2], pslices, pstacks);
        if (mTexture)
            gluQuadricTexture(quad, false);
        if (data[0])
        {
            glPushMatrix();
            glRotatef(180, 0, 1, 0);
            gluDisk(quad, 0, data[0], pslices, pstacks);
            glPopMatrix();
        }
        if (data[1])
        {
            glTranslatef(0, 0, data[2]);
            gluDisk(quad, 0, data[1], pslices, pstacks);
        }
        break;

      default:
        ;
    }
    gluDeleteQuadric(quad);
    glPopMatrix();
}
//----------------------------------------------------------

void Primitive3D::setPlane(QVector3D vectorX, QVector3D vectorY)
{
    ptype = plane;
    data[0] = vectorX.x(); data[1] = vectorX.y(); data[2] = vectorX.z();
    data[3] = vectorY.x(); data[4] = vectorY.y(); data[5] = vectorY.z();
    emit changed();
}

void Primitive3D::setBox(qreal x, qreal y, qreal z)
{
    ptype = box;
    data[0] = x;
    data[1] = y;
    data[2] = z;
    emit changed();
}

void Primitive3D::setSphere(qreal radius)
{
    ptype = sphere;
    data[0] = radius;
    emit changed();
}

void Primitive3D::setCylinder(qreal radius, qreal height)
{
    ptype = cylinder;
    data[0] = radius;
    data[1] = height;
    emit changed();
}

void Primitive3D::setCone(qreal baseRadius, qreal topRadius, qreal height)
{
    ptype = cone;
    data[0] = baseRadius;
    data[1] = topRadius;
    data[2] = height;
    emit changed();
}

void Primitive3D::setPolygon(QVector<QPointF> points)
{
    ptype = polygon;
    mPoints.clear();
    mPoints << points;
    emit changed();
}
//----------------------------------------------------------

void Primitive3D::setDetalization(int stacks, int slices)
{
    pstacks = stacks;
    pslices = slices;
    emit changed();
}
//----------------------------------------------------------

QVector3D Primitive3D::getMinBounds()
{
    switch (ptype)
    {
      case plane: return QVector3D(-fabs(data[0]+data[3]), -fabs(data[1]+data[4]), -fabs(data[2]+data[5]));
      case box: return QVector3D(-data[0]/2, -data[1]/2, -data[2]/2);
      case sphere: return QVector3D(-data[0], -data[0], -data[0]);
      case cylinder: return QVector3D(-data[0], -data[0], 0);
      case cone: return QVector3D(data[0]>data[1]?-data[0]:-data[1], data[0]>data[1]?-data[0]:-data[1], 0);
      case polygon: return QVector3D(0, 0, 0); // I'm too lazy to implement it
    }
    return QVector3D(-10, -10, -10);
}

QVector3D Primitive3D::getMaxBounds()
{
    switch (ptype)
    {
      case plane: return QVector3D(fabs(data[0]+data[3]), fabs(data[1]+data[4]), fabs(data[2]+data[5]));
      case box: return QVector3D(data[0]/2, data[1]/2, data[2]/2);
      case sphere: return QVector3D(data[0], data[0], data[0]);
      case cylinder: return QVector3D(data[0], data[0], data[1]);
      case cone: return QVector3D(data[0]>data[1]?data[0]:data[1], data[0]>data[1]?data[0]:data[1], data[2]);
      case polygon: return QVector3D(0, 0, 0); // I'm too lazy to implement it
    }
    return QVector3D(10, 10, 10);
}
//----------------------------------------------------------
