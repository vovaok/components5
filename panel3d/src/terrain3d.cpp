#include "terrain3d.h"
#include "qpanel3d.h"

Terrain3D::Terrain3D(Object3D *parent) :
    Object3D(parent),
    mStrips(0),
    mMeshVisible(true),
    mWireframeView(false)
{
}
//---------------------------------------------------------

void Terrain3D::draw()
{
    if (!mStrips)
        return;
    int cnt = mVertices.count() / mStrips;
    for (int i=0; i<mStrips; i++)
    {
        glVertexPointer(3, GL_FLOAT, 0, mVertices.data() + cnt*i);
        glNormalPointer(GL_FLOAT, 12, mNormals.data() + cnt*i);
    //    glColorPointer(3, GL_UNSIGNED_BYTE, 4, mColors.data());
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
    //    glEnableClientState(GL_COLOR_ARRAY);
        if (mMeshVisible)
        {
            GLint polygonMode[2];
            if (mWireframeView)
            {
                glGetIntegerv(GL_POLYGON_MODE, polygonMode);
                glPolygonMode(GL_FRONT, GL_LINE);
            }

            glDrawArrays(GL_QUAD_STRIP, 0, cnt);

            if (mWireframeView)
            {
                glPolygonMode(GL_FRONT, polygonMode[0]);
            }
        }
        else
        {
            glDrawArrays(GL_POINTS, 0, cnt);//mVertices.count());
        }
    //    glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}
//---------------------------------------------------------

void Terrain3D::loadHeightMap(QImage &map, float min, float max, float grid)
{
    mVertices.clear();
    mNormals.clear();

    int w = map.width();
    int h = map.height();

    mBounds = QRectF(-w/2*grid, -h/2*grid, w*grid, h*grid);

    float k = (max - min) / 255;
    for (int j=0; j<h-1; j++)
    {
        for (int i=0; i<w-1; i++)
        {
            int i1 = i;
            int i0 = i1>0? i1-1: i1;
            int i2 = i1<(w-1)? i1+1: i1;

            for (int jj=0; jj<2; jj++)
            {
                int j1 = j + jj;
                int j0 = j1>0? j1-1: j1;
                int j2 = j1<(h-1)? j1+1: j1;

//                for (int ii=0; ii<2; ii++)
//                {
//                    int i1 = jj? i + ii: i - ii + 1;
//                    int i0 = i1>0? i1-1: i1;
//                    int i2 = i1<(w-1)? i1+1: i1;

                    xyz p11;
                    p11.x = (i1 - w/2) * grid;
                    p11.y = (h/2 - j1) * grid;
                    int mz11 = (map.pixel(i1, j1) & 0xFF);
                    p11.z = min + mz11 * k;

                    mVertices << p11;

                    int mz10 = (map.pixel(i1, j0) & 0xFF);
                    float p10z = min + mz10 * k;

                    int mz12 = (map.pixel(i1, j2) & 0xFF);
                    float p12z = min + mz12 * k;

                    int mz01 = (map.pixel(i0, j1) & 0xFF);
                    float p01z = min + mz01 * k;

                    int mz21 = (map.pixel(i2, j1) & 0xFF);
                    float p21z = min + mz21 * k;

                    float dz_dx1 = (p11.z - p01z) / grid;
                    float dz_dx2 = (p21z - p11.z) / grid;
                    float dz_dx = (dz_dx1 + dz_dx2) / 2;
                    float dz_dy1 = (p11.z - p10z) / grid;
                    float dz_dy2 = (p12z - p11.z) / grid;
                    float dz_dy = (dz_dy1 + dz_dy2) / 2;

                    QVector3D n(dz_dx, dz_dy, 1);
                    n.normalize();
                    xyz n11;
                    n11.x = n.x();
                    n11.y = n.y();
                    n11.z = n.z();

                    mNormals << n11;
//                }
            }
        }
    }
    mStrips = h-1;
}

void Terrain3D::loadRealHeightMap(QImage &map, float grid)
{
    mVertices.clear();
    mNormals.clear();

    int w = map.width();
    int h = map.height();

    mBounds = QRectF(-w/2*grid, -h/2*grid, w*grid, h*grid);

    for (int j=0; j<h-1; j++)
    {
        for (int i=0; i<w-1; i++)
        {
            int i1 = i;
            int i0 = i1>0? i1-1: i1;
            int i2 = i1<(w-1)? i1+1: i1;

            for (int jj=0; jj<2; jj++)
            {
                int j1 = j + jj;
                int j0 = j1>0? j1-1: j1;
                int j2 = j1<(h-1)? j1+1: j1;

//                for (int ii=0; ii<2; ii++)
//                {
//                    int i1 = jj? i + ii: i - ii + 1;
//                    int i0 = i1>0? i1-1: i1;
//                    int i2 = i1<(w-1)? i1+1: i1;

                    xyz p11;
                    p11.x = (i1 - w/2) * grid;
                    p11.y = (h/2 - j1) * grid;
                    p11.z = *reinterpret_cast<float*>(map.scanLine(j1) + i1*4);

                    mVertices << p11;

                    float p10z = *reinterpret_cast<float*>(map.scanLine(j0) + i1*4);
                    float p12z = *reinterpret_cast<float*>(map.scanLine(j2) + i1*4);
                    float p01z = *reinterpret_cast<float*>(map.scanLine(j1) + i0*4);
                    float p21z = *reinterpret_cast<float*>(map.scanLine(j1) + i2*4);

                    float dz_dx1 = (p11.z - p01z) / grid;
                    float dz_dx2 = (p21z - p11.z) / grid;
                    float dz_dx = (dz_dx1 + dz_dx2) / 2;
                    float dz_dy1 = (p11.z - p10z) / grid;
                    float dz_dy2 = (p12z - p11.z) / grid;
                    float dz_dy = (dz_dy1 + dz_dy2) / 2;

                    QVector3D n(dz_dx, dz_dy, 1);
                    n.normalize();
                    xyz n11;
                    n11.x = n.x();
                    n11.y = n.y();
                    n11.z = n.z();

                    mNormals << n11;
//                }
            }
        }
    }
    mStrips = h-1;
}
//---------------------------------------------------------

void Terrain3D::setMouseTracking(bool enable)
{
    if (enable)
        connect(scene(), SIGNAL(moved(QVector3D,QVector3D)), SLOT(onMouseMove(QVector3D,QVector3D)));
    else
        disconnect(scene(), SIGNAL(moved(QVector3D,QVector3D)), this, SLOT(onMouseMove(QVector3D,QVector3D)));
}
//---------------------------------------------------------

void Terrain3D::onMouseClick(QVector3D p, QVector3D v)
{
    if (!isVisible())
        return;
    GLfloat *t = fullTransformMatrix();
    QMatrix4x4 m(t);
    p = m * p;
    v = m * v;
    if (!v.z())
        return;
    float k = -p.z() / v.z();
    QPointF p2d = (p + k*v).toPointF();
    if ((p2d.x() >= mBounds.left() && p2d.x() <= mBounds.right() && p2d.y() >= mBounds.top() && p2d.y() <= mBounds.bottom()))
    {
        emit clicked(p2d);
    }
}

void Terrain3D::onMouseMove(QVector3D p, QVector3D v)
{
    if (!isVisible())
        return;
    GLfloat *t = fullTransformMatrix();
    QMatrix4x4 m(t);
    p = m * p;
    v = m * v;
    if (!v.z())
        return;
    float k = -p.z() / v.z();
    QPointF p2d = (p + k*v).toPointF();
    if ((p2d.x() >= mBounds.left() && p2d.x() <= mBounds.right() && p2d.y() >= mBounds.top() && p2d.y() <= mBounds.bottom()))
    {
        emit moved(p2d);
    }
}
//---------------------------------------------------------
