#include "udponbinterface.h"
#include <QNetworkInterface>

UdpOnbInterface::UdpOnbInterface(QObject *parent) :
    QObject(parent)
{
    mMaxFrameSize = 1024;
    mBusType = BusEthernet;

    m_socket = new QUdpSocket(this);
    m_socket->setSocketOption(QUdpSocket::LowDelayOption, true);
//    m_socket->bind(QHostAddress::AnyIPv4, 51967, QAbstractSocket::ReuseAddressHint);
}

void UdpOnbInterface::setMasterMode(bool mode)
{
    ObjnetInterface::setMasterMode(mode);

    if (isMaster)
    {
        m_socket->bind(QHostAddress::AnyIPv4, 51967);

        connect(m_socket, &QUdpSocket::readyRead, [=]()
        {
            if (m_networkAddr.isNull())
            {
                QNetworkDatagram datagram = m_socket->receiveDatagram();
                if (datagram.data().startsWith("ONB"))
                {
                    m_peers << Peer{datagram.senderAddress(), datagram.senderPort()};
                    QNetworkInterface iface = QNetworkInterface::interfaceFromIndex(datagram.interfaceIndex());
//                    qDebug() << datagram.destinationAddress();
                    for (QNetworkAddressEntry e: iface.addressEntries())
                    {
                        m_networkAddr = e.broadcast();
                        if (!m_networkAddr.isNull())
                        {
                            m_socket->disconnectFromHost();
                            m_socket->bind(e.ip(), 51967);
                            m_connected = true;
                            break;
                        }
                    }
                    qDebug() << "[UdpOnbInterface] network address:" << m_socket->localAddress().toString();
                    qDebug() << "[UdpOnbInterface] broadcast address:" << m_networkAddr.toString();
                }
            }
            else
            {
                receiveMsg();
            }
//            else if (onReceive)
//                onReceive();
        });
    }
    else
    {
        m_socket->bind(); // node mode (not master)
        m_advertiseTimer = new QTimer(this);
        connect(m_advertiseTimer, &QTimer::timeout, this, &UdpOnbInterface::advertise);
        m_advertiseTimer->start(1000);

        connect(m_socket, &QUdpSocket::readyRead, [=]()
        {
            if (!m_connected)
            {
                QNetworkDatagram datagram = m_socket->receiveDatagram();
                if (datagram.data().startsWith("ONB1"))
                {
                    m_peerMap[0] = Peer{datagram.senderAddress(), datagram.senderPort()};
//                    m_socket->connectToHost(datagram.senderAddress(), datagram.senderPort());
                    m_advertiseTimer->stop();
                    m_connected = true;
                }
            }
            else
            {
                receiveMsg();
            }
        });
    }
}

bool UdpOnbInterface::send(const CommonMessage &msg)
{
    if (!isBusPresent())
        return false;
    emit message("udp_w", msg); // for debug purposes
    QByteArray ba;
    ba.append("ONB1");
    uint32_t id = msg.rawId();
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());
    int written = 0;
    QNetworkDatagram dgram;
    dgram.setData(ba);
    if (msg.isGlobal())
    {
        dgram.setDestination(m_networkAddr, 51967);
        written = m_socket->writeDatagram(dgram);
        for (Peer &peer: m_peers)
        {
            if (peer.second != 51967)
            {
                dgram.setDestination(peer.first, peer.second);
                written = m_socket->writeDatagram(dgram);
            }
        }
    }
    else
    {
        uint8_t mac = msg.localId().mac;
        Peer peer = m_peerMap[mac];
        dgram.setDestination(peer.first, peer.second);
        written = m_socket->writeDatagram(dgram);
    }
    return (written == ba.size());
}

bool UdpOnbInterface::read(CommonMessage &msg)
{
    if (m_socket->state() != QUdpSocket::ConnectedState)
        return false;

    receiveMsg();

    return ObjnetInterface::read(msg);
}

void UdpOnbInterface::flush()
{
    ObjnetInterface::flush();
    m_socket->flush();
}
//---------------------------------------------------------

int UdpOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    Q_UNUSED(id);
    Q_UNUSED(mask);
    return 0;
}

void UdpOnbInterface::removeFilter(int number)
{
    Q_UNUSED(number);
}

void UdpOnbInterface::reconnect()
{
    m_socket->disconnectFromHost();
    m_connected = false;
    if (m_advertiseTimer && !m_advertiseTimer->isActive())
        m_advertiseTimer->start();
}

void UdpOnbInterface::receiveMsg()
{
    while (m_socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_socket->receiveDatagram();

        // discard own broadcast message
        if (datagram.senderAddress() == m_socket->localAddress())
            continue;

        QByteArray ddata = datagram.data();
        if (ddata.startsWith("ONB1"))
        {
            if (ddata.size() >= 8)
            {
                uint32_t id = *reinterpret_cast<uint32_t *>(ddata.data() + 4);
                QByteArray ba = ddata.mid(8);
                CommonMessage msg;
                msg.setId(id);
                msg.setData(std::move(ba));
                if (isMaster && msg.isLocal())
                {
                    if (msg.localId().svc)
                    {
                        if (msg.localId().oid == svcHello && msg.data().size() == 1)
                        {
                            uint8_t mac = msg.data()[0];
                            m_peerMap[mac] = Peer{datagram.senderAddress(), datagram.senderPort()};
                        }
                    }
                }
                receive(std::move(msg));
                emit message("udp_r", msg); // for debug purposes
            }
        }
    }
}

void UdpOnbInterface::advertise()
{
    if (!m_connected)
    {
        QHostAddress broadcast = QHostAddress::Broadcast;
        m_socket->writeDatagram("ONB1", 4, broadcast, 51967);
    }
}
