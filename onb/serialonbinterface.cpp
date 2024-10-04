#include "serialonbinterface.h"

using namespace Objnet;

SerialOnbInterface::SerialOnbInterface(QIODevice *device, bool half_duplex)
{
    mMaxFrameSize = 64;
    if (half_duplex)
        mBusType = BusSwonb;
    else
        mBusType = BusWifi;
    mSerial = new SerialFrame(device);
//    disconnect(device, SIGNAL(readyRead()));
    connect(mSerial, &SerialFrame::dataReceived, this, &SerialOnbInterface::onDataReceived);

    m_timeoutTimer.setSingleShot(true);
}

bool SerialOnbInterface::send(const CommonMessage &msg)
{
    if (mBusType == BusSwonb && m_timeoutTimer.isActive())
    {
        return false;
    }

    QByteArray ba;
    uint32_t id = msg.rawId();
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());
    mSerial->sendData(ba);
    if (mSerial->isActive())
        emit message("serial", msg); // for debug purposes

    if (msg.isLocal())
        m_timeoutTimer.start(5);
    return true;
}
//---------------------------------------------------------

int SerialOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    Q_UNUSED(id);
    Q_UNUSED(mask);
    return 0;
}

void SerialOnbInterface::removeFilter(int number)
{
    Q_UNUSED(number);
}
//---------------------------------------------------------

void SerialOnbInterface::onDataReceived(const QByteArray &ba)
{
    m_timeoutTimer.stop();
    CommonMessage msg;
    uint32_t id = *reinterpret_cast<const uint32_t*>(ba.data());
    msg.setId(id);
    msg.setData(ba.mid(4));
    receive(msg);
    emit message("serial", msg); // for debug purposes
}
