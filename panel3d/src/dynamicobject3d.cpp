#include "dynamicobject3d.h"

DynamicObject3D::DynamicObject3D(QObject *parent, DynamicModel *model) :
    Object3D(parent),
    mIntegrationEnabled(true)
{
    if (model)
        mModel = model;
    else
        mModel = new DynamicModel();

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), SLOT(onTimer()));
    mTimer->start(30);
}

void DynamicObject3D::setModel(DynamicModel *model)
{
    delete mModel;
    mModel = model;
}

void DynamicObject3D::onTimer()
{
    mModel->integrate(mTimer->interval() * 0.001);
}

void DynamicObject3D::setIntegrationEnabled(bool enable)
{
    if (enable && !mIntegrationEnabled)
    {
        mIntegrationEnabled = true;
        mTimer->start();
    }
    else if (!enable && mIntegrationEnabled)
    {
        mIntegrationEnabled = false;
        mTimer->stop();
    }
}

void DynamicObject3D::draw()
{
    setPosition(mModel->position() * 100);
    setRotation(mModel->rotation());
    Object3D::draw();
}
