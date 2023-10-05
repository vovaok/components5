#ifndef SERIALONBINTERFACE_H
#define SERIALONBINTERFACE_H

#include "objnetInterface.h"
#include "serialframe.h"
#include <QQueue>

namespace Objnet
{

class SerialOnbInterface : public QObject, public ObjnetInterface
{
    Q_OBJECT

private:
    SerialFrame *mSerial;
    QTimer m_timeoutTimer;

public:
    explicit SerialOnbInterface(QIODevice *device, bool half_duplex=false);

    virtual bool send(const CommonMessage &msg) override;

    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);

signals:
    void message(QString, const CommonMessage&);

private slots:
    void onDataReceived(const QByteArray &ba);
};

}

#endif // SERIALONBINTERFACE_H
