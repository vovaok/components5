#include "udponbinterface.h"

UdpOnbInterface::UdpOnbInterface(QObject *parent) :
    QObject(parent)
{
    mMaxFrameSize = 1024;
    mBusType = BusEthernet;

    m_socket = new QUdpSocket();
//    m_socket->moveToThread(this);
    m_socket->setSocketOption(QUdpSocket::LowDelayOption, true);
    m_socket->bind(QHostAddress::Any, 51967);
    m_socket->connectToHost(QHostAddress("192.168.0.10"), 51967);
}

bool UdpOnbInterface::write(CommonMessage &msg)
{
    QByteArray ba;
    uint32_t id = msg.rawId();
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());
    int written = m_socket->write(ba);
    return (written == ba.size());
}

bool UdpOnbInterface::read(CommonMessage &msg)
{
//    QByteArray data = m_socket->readAll();

//    if (data.size() >= 4)
//    {
//        uint32_t id = *reinterpret_cast<uint32_t *>(data.data());
//        QByteArray ba = data.mid(4);
//        msg.setId(id);
//        msg.setData(ba);
//        return true;
//    }

    if (m_socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        if (datagram.data().size() >= 4)
        {
            uint32_t id = *reinterpret_cast<uint32_t *>(datagram.data().data());
            QByteArray ba = datagram.data().mid(4);
            msg.setId(id);
            msg.setData(ba);
            return true;
        }
    }
    return false;
}

void UdpOnbInterface::flush()
{
    m_socket->flush();
}

int UdpOnbInterface::availableWriteCount()
{
    return 1;
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
