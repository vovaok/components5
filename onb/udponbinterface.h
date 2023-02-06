#ifndef UDPTHREAD_H
#define UDPTHREAD_H

#include <objnetInterface.h>
#include <QUdpSocket>
#include <QNetworkDatagram>

using namespace Objnet;

class UdpOnbInterface : public QObject, public ObjnetInterface
{
    Q_OBJECT

public:
    UdpOnbInterface(QObject *parent=nullptr);

signals:
    void message(QString, const CommonMessage&); // for debug purposes

protected:
    virtual bool send(const CommonMessage &msg) override;
    bool read(CommonMessage &msg) override;
    virtual void flush() override;

    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF) override;
    void removeFilter(int number) override;

private:
    QUdpSocket *m_socket;
    QElapsedTimer etimer;
};

#endif // UDPTHREAD_H
