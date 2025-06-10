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

    virtual bool isBusPresent() const override {return m_connected;}

signals:
    void message(QString, const CommonMessage&); // for debug purposes

protected:
    virtual void setMasterMode(bool mode) override;

    virtual bool send(const CommonMessage &msg) override;
    bool read(CommonMessage &msg) override;
    virtual void flush() override;

    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF) override;
    void removeFilter(int number) override;

    virtual void reconnect() override;

private:
    QUdpSocket *m_socket;
    QElapsedTimer etimer;
    QTimer *m_advertiseTimer = nullptr;
    QHostAddress m_networkAddr; // broadcast address for global msgs

    typedef QPair<QHostAddress, int> Peer;
    QMap<uint8_t, Peer> m_peerMap;
    QVector<Peer> m_peers;
    bool m_connected = false;

    void receiveMsg();
    void advertise();
};

#endif // UDPTHREAD_H
