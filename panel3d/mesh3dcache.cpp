#include "mesh3dcache.h"
#include <QApplication>

Mesh3DCache *Mesh3DCache::self = 0L;

Mesh3DCache::Mesh3DCache(QObject *parent) :
    QObject(parent)
{
}

Mesh3DCache::~Mesh3DCache()
{
    // clear the cache
    QMap <QString, Mesh*>::iterator it;
    for (it=MeshCache.begin(); it!=MeshCache.end(); it++)
        delete (*it);
    MeshCache.clear();
}

Mesh3DCache *Mesh3DCache::instance()
{
    if (!self)
        self = new Mesh3DCache();
    return self;
}
//----------------------------------------------------------

Mesh* Mesh3DCache::loadMesh(QString filename)
{
    Mesh *mesh = MeshCache.value(filename, 0L);
    if (mesh)
        return mesh;

    mesh = new Mesh();
    MeshLoaderThread *mloader = new MeshLoaderThread(filename, mesh, this);
    connect(mloader, SIGNAL(loaded(QString,Mesh*)), SLOT(onMeshLoaded(QString,Mesh*)));
    connect(mloader, SIGNAL(finished()), mloader, SLOT(deleteLater()));
    mloader->start();

    while (mloader->isRunning())
        qApp->processEvents(); // wait

    return mesh;
}

void Mesh3DCache::setListIdForMesh(QString filename, int listId)
{
    if (MeshCache.contains(filename))
        mListId[filename] = listId;
}

void Mesh3DCache::onMeshLoaded(QString filename, Mesh *mesh)
{
    if (!mesh)
    {
        qDebug() << "file '" + filename + "' open failed.";
        return;
    }

//    mesh->setMinMaxValues();
    MeshCache[filename] = mesh;
    //qDebug() << "model '" + filename + "' loaded";
}
//---------------------------------------------------------

void MeshLoaderThread::run()
{
    QFile file(f);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m = 0L;
    }
    else
    {
        QTextStream stream(&file);
        QFileInfo fi(f);
        m->setUrlPrefix(fi.path());
        m->load(&stream);
//        mesh = new Mesh(&stream);
        file.close();
    }

    emit loaded(f, m);
}
//---------------------------------------------------------
