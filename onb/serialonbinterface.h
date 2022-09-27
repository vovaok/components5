#ifndef SERIALONBINTERFACE_H
#define SERIALONBINTERFACE_H

#include "objnetInterface.h"
#include "serialframe.h"
#include <QQueue>

namespace Objnet
{

class SerialOnbInterface : public ObjnetInterface
{
    Q_OBJECT

private:
    SerialFrame *mSerial;
    static const int mRxQueueMaxSize = 64;
    QQueue<CommonMessage> mRxQueue;

public:
    explicit SerialOnbInterface(QIODevice *device);

    bool write(CommonMessage &msg);
    bool read(CommonMessage &msg);
    void flush();

    int availableWriteCount();

    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);

signals:
    void message(QString, CommonMessage&);

private slots:
    void onDataReceived(const QByteArray &ba);
};

}

#endif // SERIALONBINTERFACE_H
