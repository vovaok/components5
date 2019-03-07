#ifndef MESH3DCACHE_H
#define MESH3DCACHE_H

#include <QObject>
#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include "mesh.h"

class MeshLoaderThread : public QThread
{
    Q_OBJECT
private:
    Mesh *m_mesh;
    QString m_filename;
public:
    MeshLoaderThread(QString filename, Mesh *mesh, QObject *parent=nullptr) : QThread(parent), m_mesh(mesh), m_filename(filename) {}
    void run();
signals:
    void loaded(QString, Mesh*);
};

class Mesh3DCache : public QObject
{
    Q_OBJECT

private:
    QMap <QString, Mesh*> mMeshCache;
    QMap<QString, int> mListId;

private:
    explicit Mesh3DCache(QObject *parent = 0);
    virtual ~Mesh3DCache();

    static Mesh3DCache *mSelf;
    Q_DISABLE_COPY(Mesh3DCache)

public:
    static Mesh3DCache *instance();

    Mesh* loadMesh(QString filename);
    void setListIdForMesh(QString filename, int listId);
    int listId(QString filename) const {return mListId.value(filename, 0);}

    bool isMeshLoaded(QString filename) const {return mMeshCache.contains(filename);}

public slots:
    void onMeshLoaded(QString filename, Mesh *mesh);

};

#endif // MESH3DCACHE_H
