#ifndef MESH_H
#define MESH_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QImage>
#include "vrml2parser.h"
#include <GL/gl.h>

typedef QVector3D GLfloat3;
typedef QVector2D GLfloat2;

//class GLfloat3
//{
//private:
//    GLfloat x;
//    GLfloat y;
//    GLfloat z;
//public:
//    GLfloat3() : x(0), y(0), z(0) {}
//    GLfloat3(GLfloat X, GLfloat Y, GLfloat Z) : x(X), y(Y), z(Z) {}
//};

//class MeshVertex
//{
//public:
//    GLfloat point[3];
//    GLfloat normal[3];
//    GLfloat color[3];
//};

//class MeshFace
//{
//public:
//    QVector<MeshVertex*> vertices;
//    GLfloat normal[3];
//    GLfloat color[3];
//};

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
//    QVector<MeshVertex*> vertices;
//    QVector<MeshFace*> faces;
    QVector<GLfloat3> vertices;
    QVector<GLfloat3> normals;
    QVector<GLfloat2> texCoord;
    MeshMaterial material;
    bool normalPerVertex;

    QImage texture;

//    ~MeshShape()
//    {
//        for (int i=0; i<faces.count(); i++)
//            delete faces[i];
//        faces.clear();
//        for (int i=0; i<vertices.count(); i++)
//            delete vertices[i];
//        vertices.clear();
//    }
};

class Mesh
{
private:
    QVector<MeshShape*> Shapes;
    QString mUrlPrefix;

    void loadVrml2(QTextStream *stream);
    void loadChildren(GroupingNode *par);
    void loadShape(Shape *node);

public:
    explicit Mesh();
    Mesh(QTextStream *stream);
    virtual ~Mesh();

    void clear();
    void setUrlPrefix(QString s) {mUrlPrefix = s;}
    void load(QTextStream *stream);
    inline QVector<MeshShape*>& shapes() {return Shapes;}

    void scaleUniform(float factor);

    //void operator =(Mesh &mesh);
//    void copy(Mesh *mesh);
};

#endif // MESH_H
