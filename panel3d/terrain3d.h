#ifndef TERRAIN3D_H
#define TERRAIN3D_H

#include "object3d.h"
#include "pointcloud3d.h"

struct XyzRgbPoint
{
    float x, y, z;
    unsigned char r, g, b, _dummy;
};

class Terrain3D : public Object3D
{
    Q_OBJECT

private:
    QVector<xyz> mVertices;
    QVector<xyz> mNormals;
    QVector<GLuint> mColors;

    int mStrips;
    QRectF mBounds;

    bool mMeshVisible;
    bool mWireframeView;

protected:
    virtual void draw();

public:
    explicit Terrain3D(Object3D *parent);
    virtual ~Terrain3D() {}

    void loadHeightMap(QImage &map, float min, float max, float grid);
    void loadRealHeightMap(QImage &map, float grid);

    bool isMeshVisible() const {return mMeshVisible;}
    void setMeshVisible(bool visible) {mMeshVisible = visible;}
    bool isWireframeView() const {return mWireframeView;}
    void setWireframeView(bool enable) {mWireframeView = enable;}

    void setMouseTracking(bool enable);

signals:
    void clicked(QPointF p);
    void moved(QPointF p);

private slots:
    void onMouseMove(QVector3D p, QVector3D v);
    void onMouseClick(QVector3D p, QVector3D v);
};

#endif // TERRAIN3D_H
