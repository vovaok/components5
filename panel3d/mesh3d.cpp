#include "mesh3d.h"

Mesh3D::Mesh3D(QObject *parent) :
    Object3D(parent),
    mTex(0L)
{
    listNo = 0;
    meshOwner = false;
}

Mesh3D::~Mesh3D()
{
    clearMesh();
}

void Mesh3D::clearMesh()
{
    if (listNo)
        glDeleteLists(listNo, 1);

    if (pmesh && meshOwner)
        delete pmesh;
    pmesh = 0L;
    meshOwner = false;
}
//----------------------------------------------------------

//void Mesh3D::copyMesh()
//{
//    Mesh *theCopy = new Mesh;
//    theCopy->copy(pmesh);
//    pmesh = theCopy;
//    meshOwner = true;
//}
//----------------------------------------------------------

void Mesh3D::loadModel(QString filename, ColorPolicy colorPolicy)
{
    mFilename = filename;
    clearMesh();
    Mesh3DCache *cache = Mesh3DCache::instance();
    pmesh = cache->loadMesh(mFilename);
    if (!pmesh->shapes().isEmpty())
    {
        listNo = cache->listId(mFilename);
        mColorPolicy = colorPolicy;
        if (!pmesh->shapes()[0]->texture.isNull())
            mTex = new StaticTexture(scene(), pmesh->shapes()[0]->texture);
    }

    setSettingsChanged(); // implements:    emit changed();
}

void Mesh3D::updateModel()
{
    setSettingsChanged();
}
//----------------------------------------------------------

void Mesh3D::applySettings()
{
    if (!listNo)
        listNo = glGenLists(1);
//    else
//        return;

    if (!listNo)
        return; // fucking error!!!!

    glNewList(listNo, GL_COMPILE);
    if (pmesh)
        drawMesh();
    glEndList();

    Mesh3DCache *cache = Mesh3DCache::instance();
    cache->setListIdForMesh(mFilename, listNo);
}
//----------------------------------------------------------

void Mesh3D::drawMesh()
{
    for (int i=0; i<pmesh->shapes().count(); i++)
    {
        MeshShape *shape = pmesh->shapes()[i];

        if (mColorPolicy == AssignOneColor)
        {
            assignDefColor(shape->material.ambientColor, shape->material.diffuseColor, shape->material.emissiveColor, shape->material.specularColor, shape->material.shininess);
        }
        else if (mColorPolicy == UseModelColors)
        {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, shape->material.diffuseColor);
            glMaterialfv(GL_FRONT, GL_AMBIENT, shape->material.ambientColor);
            glMaterialfv(GL_FRONT, GL_EMISSION, shape->material.emissiveColor);
            glMaterialfv(GL_FRONT, GL_SPECULAR, shape->material.specularColor);
            glMaterialf(GL_FRONT, GL_SHININESS, shape->material.shininess);
        }

//        glPushMatrix();
//        glMultMatrixf(shape->transform.data());
        if (shape->normals.isEmpty())
        {
            glVertexPointer(3, GL_FLOAT, 0, shape->points.data());
//            glColorPointer(3, GL_UNSIGNED_BYTE, 4, colors.data());
            glEnableClientState(GL_VERTEX_ARRAY);
//            glEnableClientState(GL_COLOR_ARRAY);
            glDrawArrays(GL_POINTS, 0, shape->points.count());
//            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
        else
        {
            if (mTex)
                mTex->bind();
            glVertexPointer(3, GL_FLOAT, 0, shape->points.data());
            glNormalPointer(GL_FLOAT, 12, shape->normals.data());
            if (mTex)
                glTexCoordPointer(2, GL_FLOAT, 0, shape->texCoord.data());
//            glColorPointer(3, GL_UNSIGNED_BYTE, 4, colors.data());
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            if (mTex)
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//            glEnableClientState(GL_COLOR_ARRAY);
            glDrawArrays(GL_TRIANGLES, 0, shape->points.count());
//            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            if (mTex)
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            if (mTex)
                mTex->disable();
        }
//        glPopMatrix();

//        for (int j=0; j<shape->faces.count(); j++)
//        {
//            MeshFace *face = shape->faces[j];
//            glBegin(GL_POLYGON);
//            if (shape->normalPerVertex)
//            {
//                for (int k=0; k<face->vertices.count(); k++)
//                {
//                    MeshVertex *vertex = face->vertices[k];
//                    glNormal3fv(vertex->normal);
//                    glVertex3fv(vertex->point);
//                }
//            }
//            glEnd();
//        }
    }
}

//----------------------------------------------------------

void Mesh3D::draw()
{
    if (listNo)
    {
        //glDisable(GL_COLOR_MATERIAL);
        glCallList(listNo);
        //glEnable(GL_COLOR_MATERIAL);
    }
}
//----------------------------------------------------------

QVector3D Mesh3D::getMinBounds()
{
//    if (pmesh)
//        return QVector3D(pmesh->getXMin(), pmesh->getYMin(), pmesh->getZMin());
    return QVector3D();
}

QVector3D Mesh3D::getMaxBounds()
{
//    if (pmesh)
//        return QVector3D(pmesh->getXMax(), pmesh->getYMax(), pmesh->getZMax());
    return QVector3D();
}
//----------------------------------------------------------
