#include "pointcloud3d.h"
#include "qpanel3d.h"

PointCloud3D::PointCloud3D(QObject *parent) :
    Object3D(parent),
    mLighting(false)
{
    pPointSize = 1;
    drawBuffer = false;
    pMeshVisible = false;
    pWireframeView = false;
}
//---------------------------------------------------------------------------

void PointCloud3D::draw()
{
    GLboolean lighting;
    glGetBooleanv(GL_LIGHTING, &lighting);
    if (mLighting)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);

    glPointSize(pPointSize);
    glScalef(100, 100, 100);
    glNormal3f(0, 0, -100);

    if (drawBuffer)
    {
        glVertexPointer(3, GL_FLOAT, pStride, pPointBuffer);
        glColorPointer(3, GL_UNSIGNED_BYTE, pStride, pColorBuffer);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDrawArrays(GL_POINTS, 0, pCount);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else if (!vertices.isEmpty())
    {
        if (normals.isEmpty())
        {
            glVertexPointer(3, GL_FLOAT, 0, vertices.data());
            glColorPointer(3, GL_UNSIGNED_BYTE, 4, colors.data());
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glDrawArrays(GL_POINTS, 0, vertices.count());
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
        else
        {
            glVertexPointer(3, GL_FLOAT, 0, vertices.data());
            glNormalPointer(GL_FLOAT, 12, normals.data());
            glColorPointer(3, GL_UNSIGNED_BYTE, 4, colors.data());
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            if (pMeshVisible)
            {
                GLint polygonMode[2];
                if (pWireframeView)
                {
                    glGetIntegerv(GL_POLYGON_MODE, polygonMode);
                    glPolygonMode(GL_FRONT, GL_LINE);
                }

                glDrawArrays(GL_TRIANGLES, 0, vertices.count());

                if (pWireframeView)
                {
                    glPolygonMode(GL_FRONT, polygonMode[0]);
                }
            }
            else
            {
                glDrawArrays(GL_POINTS, 0, vertices.count());
            }
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }

    if (lighting)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);
}
//---------------------------------------------------------------------------

void PointCloud3D::clear()
{
    vertices.clear();
    colors.clear();
}


void PointCloud3D::addXYZRGBAPoint(float *data, unsigned long rgba, float *normalData)
{
    vertices << *reinterpret_cast<xyz*>(data);
    colors << rgba;
    if (normalData)
        normals << *reinterpret_cast<xyz*>(normalData);
    drawBuffer = false;
}

void PointCloud3D::setDataBuffer(const void *data, int colorOffset, int stride, int count)
{
    pPointBuffer = data;
    pColorBuffer = reinterpret_cast<const unsigned char*>(data) + colorOffset;
    pStride = stride;
    pCount = count;
    drawBuffer = true;
}
//---------------------------------------------------------------------------
