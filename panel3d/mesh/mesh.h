#ifndef MESH_H
#define MESH_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QImage>
#include <QMatrix4x4>
#include "vrml2parser.h"
#include <Windows.h>
#include <GL/gl.h>

typedef QVector3D GLfloat3;
typedef QVector2D GLfloat2;

class MeshMaterial
{
public:
    GLfloat ambientColor[4];
    GLfloat diffuseColor[4];
    GLfloat emissiveColor[4];
    GLfloat specularColor[4];
    GLfloat shininess;
};

class MeshShape
{
public:
    QVector<GLfloat3> points;
    QVector<GLfloat3> normals;
    QVector<GLfloat2> texCoord;
    MeshMaterial material;
    bool normalPerVertex;

    typedef struct
    {
        QVector3D point;
        QVector3D normal;
        QVector2D texCoord;
    } Vertex;

    QVector<Vertex> vertices;
    QVector<int> indices;

    QImage texture;
};

class Mesh
{
private:
    QVector<MeshShape*> Shapes;
    QString mUrlPrefix;

    void loadVrml2(QTextStream *stream);
    void loadChildren(GroupingNode *par);
    MeshShape *loadShape(Shape *node);

    QMatrix4x4 mCurTransform;

public:
    explicit Mesh();
    Mesh(QTextStream *stream);
    virtual ~Mesh();

    void clear();
    void setUrlPrefix(QString s) { mUrlPrefix = s; }
    void load(QTextStream *stream);
    inline QVector<MeshShape*>& shapes() {return Shapes;}

    void scaleUniform(float factor);
    void translate(QVector3D p);
    void rotate(float angle, QVector3D axis);
};

#endif // MESH_H
