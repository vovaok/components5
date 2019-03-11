#include "serialonbinterface.h"

using namespace Objnet;

SerialOnbInterface::SerialOnbInterface(QIODevice *device)
{
    mMaxFrameSize = 64;
    mSerial = new SerialFrame(device);
    connect(mSerial, &SerialFrame::dataReceived, this, &SerialOnbInterface::onDataReceived);
}

bool SerialOnbInterface::write(CommonMessage &msg)
{
    QByteArray ba;
    unsigned long id = msg.rawId();
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());
    mSerial->sendData(ba);
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

}

int SerialOnbInterface::availableWriteCount()
{
    return 256;
}
//---------------------------------------------------------

int SerialOnbInterface::addFilter(unsigned long id, unsigned long mask)
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
    unsigned long id = *reinterpret_cast<const unsigned long*>(ba.data());
    msg.setId(id);
    msg.setData(ba.mid(4));
    mRxQueue << msg;
}
