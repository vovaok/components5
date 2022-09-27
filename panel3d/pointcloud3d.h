#ifndef POINTCLOUD3D_H
#define POINTCLOUD3D_H

#include "object3d.h"

typedef struct
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
} xyz;

class PointCloud3D : public Object3D
{
    Q_OBJECT

private:
    QVector<xyz> vertices;
    QVector<xyz> normals;
    QVector<GLuint> colors;
    float pPointSize;

    bool drawBuffer;
    const void *pPointBuffer;
    const void *pColorBuffer;
    int pStride;
    int pCount;
    bool pMeshVisible;
    bool pWireframeView;
    bool mLighting;

protected:
    virtual void draw();
//    QVector3D getMinBounds();
//    QVector3D getMaxBounds();

public:
    explicit PointCloud3D(QObject *parent = 0);

    void clear();
    void addXYZRGBAPoint(float *data, uint32_t rgba, float *normalData=0L);
    void setDataBuffer(const void *data, int colorOffset, int stride, int count);
    
    float pointSize() const {return pPointSize;}
    void setPointSize(float size) {pPointSize = size;}

    bool isMeshVisible() const {return pMeshVisible;}
    void setMeshVisible(bool value) {pMeshVisible = value;}

    bool isWireframeView() const {return pWireframeView;}
    void setWireframeView(bool value) {pWireframeView = value;}

    bool isLightingEnabled() const {return mLighting;}
    void setLightingEnabled(bool enabled) {mLighting = enabled;}

    int pointCount() const {return vertices.count();}
    GLuint& pointColor(int idx) {return colors[idx];}
    GLfloat* pointData(int idx) {return &(vertices[idx].x);}
    GLfloat* pointNormals(int idx) {if (normals.count()) return &(normals[idx].x); return 0L;}

signals:
    
public slots:
    
};

#endif // POINTCLOUD3D_H
