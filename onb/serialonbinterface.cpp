#include "serialonbinterface.h"

using namespace Objnet;

SerialOnbInterface::SerialOnbInterface(QIODevice *device)
{
    mMaxFrameSize = 64;
    mSerial = new SerialFrame(device);
    disconnect(device, SIGNAL(readyRead()));
    connect(mSerial, &SerialFrame::dataReceived, this, &SerialOnbInterface::onDataReceived);
}

bool SerialOnbInterface::write(CommonMessage &msg)
{
    QByteArray ba;
    uint32_t id = msg.rawId();
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());
    mSerial->sendData(ba);
    if (mSerial->isActive())
        emit message("serial", msg); // for debug purposes
    return true;
}

bool SerialOnbInterface::read(CommonMessage &msg)
{
    if (mRxQueue.isEmpty())
        return false;
    msg = mRxQueue.dequeue();
    emit message("serial", msg); // for debug purposes
    return true;
}

void SerialOnbInterface::flush()
{
    mRxQueue.clear();
}

int SerialOnbInterface::availableWriteCount()
{
    return 256;
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
    CommonMessage msg;
    uint32_t id = *reinterpret_cast<const uint32_t*>(ba.data());
    msg.setId(id);
    msg.setData(ba.mid(4));
    mRxQueue << msg;
}
