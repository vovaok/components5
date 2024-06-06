#include "udponbinterface.h"
#include <QNetworkInterface>

UdpOnbInterface::UdpOnbInterface(QObject *parent) :
    QObject(parent)
{
    mMaxFrameSize = 1024;
    mBusType = BusEthernet;

    m_socket = new QUdpSocket(this);
//    m_socket->moveToThread(this);
    m_socket->setSocketOption(QUdpSocket::LowDelayOption, true);
    m_socket->bind(QHostAddress::AnyIPv4, 51967);

    connect(m_socket, &QUdpSocket::readyRead, [=]()
    {
        if (m_networkAddr.isNull())
        {
            QNetworkDatagram datagram = m_socket->receiveDatagram();
            if (datagram.data() == "ONB1")
            {
                QNetworkInterface iface = QNetworkInterface::interfaceFromIndex(datagram.interfaceIndex());
                m_networkAddr = iface.addressEntries().first().broadcast();
                qDebug() << "[UdpOnbInterface] network address:" << m_networkAddr.toString();
            }
        }
        else
        {
            receiveMsg();
        }
//        else if (onReceive)
//            onReceive();
    });
}

bool UdpOnbInterface::send(const CommonMessage &msg)
{
    if (!isBusPresent())
        return false;
    emit message("udp", msg); // for debug purposes
    QByteArray ba;
    ba.append("ONB1");
    uint32_t id = msg.rawId();
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());
    int written = 0;
    if (msg.isGlobal())
        written = m_socket->writeDatagram(ba, m_networkAddr, 51967);
    else
    {
        uint8_t mac = msg.localId().mac;
        written = m_socket->writeDatagram(ba, m_addrMap[mac], 51967);
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
}

void UdpOnbInterface::receiveMsg()
{
    if (m_socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        QByteArray ba = datagram.data();
        if (ba.startsWith("ONB1"))
        {
            if (datagram.data().size() >= 8)
            {
                uint32_t id = *reinterpret_cast<uint32_t *>(datagram.data().data() + 4);
                QByteArray ba = datagram.data().mid(8);
                CommonMessage msg;
                msg.setId(id);
                msg.setData(std::move(ba));
                if (msg.isLocal())
                {
                    if (msg.localId().svc)
                    {
                        if (msg.localId().oid == svcHello && msg.data().size() == 1)
                        {
                            uint8_t mac = msg.data()[0];
                            m_addrMap[mac] = datagram.senderAddress();
                        }
                    }
                }
                receive(std::move(msg));
                emit message("udp", msg); // for debug purposes
            }
        }
    }
}
