#include "udponbinterface.h"

UdpOnbInterface::UdpOnbInterface(QObject *parent) :
    QObject(parent)
{
    mMaxFrameSize = 1024;
    mBusType = BusEthernet;

    m_socket = new QUdpSocket(this);
//    m_socket->moveToThread(this);
    m_socket->setSocketOption(QUdpSocket::LowDelayOption, true);
    m_socket->bind(QHostAddress::Any, 51967);
    connect(m_socket, &QUdpSocket::readyRead, [=]()
    {
        if (m_socket->state() != QUdpSocket::ConnectedState)
        {
            QNetworkDatagram datagram = m_socket->receiveDatagram();
            if (datagram.data() == "ONB1")
            {
                m_socket->connectToHost(datagram.senderAddress(), datagram.senderPort());
            }
        }
        else if (onReceive)
            onReceive();
    });

//    m_socket->open(QIODevice::ReadWrite);
//    advertiseTimer = new QTimer(this);
//    connect(advertiseTimer, &QTimer::timeout, [=]()
//    {
//        qDebug() << "broadcast";
//        m_socket->writeDatagram("preved", 7, QHostAddress("239.255.0.1"), 51967);
//    });
//    advertiseTimer->start(1000);
}

bool UdpOnbInterface::send(const CommonMessage &msg)
{
    if (m_socket->state() != QUdpSocket::ConnectedState)
        return false;
    emit message("serial", msg); // for debug purposes
    QByteArray ba;
    ba.append("ONB1");
    uint32_t id = msg.rawId();
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());
    int written = m_socket->write(ba);
    return (written == ba.size());
}

bool UdpOnbInterface::read(CommonMessage &msg)
{
    if (m_socket->state() != QUdpSocket::ConnectedState)
        return false;
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
                msg.setId(id);
                msg.setData(ba);
                receive(msg);
                emit message("udp", msg); // for debug purposes
            }
        }
    }
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
