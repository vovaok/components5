#ifndef DYNAMICOBJECT3D_H
#define DYNAMICOBJECT3D_H

#include "object3d.h"
#include "camera3d.h"
#include "dynamicmodel.h"
#include <QTimer>

class DynamicObject3D : public Object3D
{
    Q_OBJECT

private:
    DynamicModel *mModel;
    QTimer *mTimer;
    bool mIntegrationEnabled;

protected:
    void draw();

public:
    explicit DynamicObject3D(QObject *parent = 0, DynamicModel *model = 0);

    DynamicModel *model() {return mModel;}
    void setModel(DynamicModel *model);

    virtual Camera3D *camera() const {return 0L;}

    bool isIntegrationEnabled() const {return mIntegrationEnabled;}
    
signals:

private slots:
    void onTimer();
    
public slots:
    void setIntegrationEnabled(bool enable);
    
};

#endif // DYNAMICOBJECT3D_H
