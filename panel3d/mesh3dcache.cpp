#include "mesh3dcache.h"
#include <QApplication>

Mesh3DCache *Mesh3DCache::self = nullptr;

Mesh3DCache::Mesh3DCache(QObject *parent) : QObject(parent)
{
}

Mesh3DCache::~Mesh3DCache()
{
    for(auto mesh : MeshCache) delete mesh;
    MeshCache.clear();
}

Mesh3DCache *Mesh3DCache::instance()
{
    if (!self) self = new Mesh3DCache();

    return self;
}

Mesh* Mesh3DCache::loadMesh(QString filename)
{
    Mesh *mesh = MeshCache.value(filename, nullptr);
    if (mesh)
        return mesh;

    mesh = new Mesh();
    MeshLoaderThread *mloader = new MeshLoaderThread(filename, mesh, this);
    connect(mloader, SIGNAL(loaded(QString,Mesh*)), SLOT(onMeshLoaded(QString,Mesh*)));
    connect(mloader, SIGNAL(finished()), mloader, SLOT(deleteLater()));
    mloader->start();

    while (mloader->isRunning())
        qApp->processEvents();

    return mesh;
}

void Mesh3DCache::setListIdForMesh(QString filename, int listId)
{
    if (MeshCache.contains(filename)) mListId[filename] = listId;
}

void Mesh3DCache::onMeshLoaded(QString filename, Mesh *mesh)
{
    if (!mesh)
    {
        qDebug() << "file '" + filename + "' open failed.";
        return;
    }

    MeshCache[filename] = mesh;
    qDebug() << "model '" + filename + "' loaded";
}

void MeshLoaderThread::run()
{
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_mesh = nullptr;
    }
    else
    {
        QTextStream stream(&file);
        QFileInfo fi(m_filename);
        m_mesh->setUrlPrefix(fi.path());
        m_mesh->load(&stream);
        file.close();
    }

    emit loaded(m_filename, m_mesh);
}
