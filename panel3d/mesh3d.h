#ifndef MESH3D_H
#define MESH3D_H

#include "object3d.h"
#include "vrml2parser.h"
#include "mesh3dcache.h"
#include <QFile>

using namespace VRML2;

class Mesh3D : public Object3D
{
    Q_OBJECT

public:
    typedef enum {UseModelColors, AssignOneColor} ColorPolicy;

private:
    GLuint listNo;
    QString mFilename;
    Mesh *pmesh;
    bool meshOwner;
    ColorPolicy mColorPolicy;

    void clearMesh();
    void drawMesh();

protected:
    void applySettings();
    QVector3D getMinBounds();
    QVector3D getMaxBounds();

public:
    explicit Mesh3D(QObject *parent = 0);
    virtual ~Mesh3D();

    Mesh* mesh() {return pmesh;}
//    void copyMesh();

    ColorPolicy colorPolicy() const {return mColorPolicy;}

    void loadModel(QString filename, ColorPolicy colorPolicy = UseModelColors);
    void updateModel();

signals:

public slots:
    void draw();
};

#endif // MESH3D_H
