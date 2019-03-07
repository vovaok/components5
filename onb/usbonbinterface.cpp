#include "usbonbinterface.h"

using namespace Objnet;

UsbHidOnbInterface::UsbHidOnbInterface(UsbOnbThread *usbhid) :
    usb(usbhid)
{
    mMaxFrameSize = 8;
    usb->start(QThread::HighPriority);
}

UsbHidOnbInterface::~UsbHidOnbInterface()
{    
//    usb->close();
//    usb->requestInterruption();
//    while (!usb->isFinished());
}
//---------------------------------------------------------

bool UsbHidOnbInterface::write(Objnet::CommonMessage &msg)
{
    if (!usb->isReady()) //isOpen())
        return false;
    unsigned long id = msg.rawId();
    unsigned char sz = msg.data().size();
    QByteArray ba;
    ba.resize(13);
    *reinterpret_cast<unsigned long*>(ba.data()) = id;
    ba[4] = sz;
    for (int i=0; i<sz; i++)
        ba[5+i] = msg.data()[i];
    //qDebug() << "start set feature";
    usb->setFeature(0x01, ba);
    //qDebug() << "end";

    emit message("usbonb", msg); // for debug purposes
    return true;
}

bool UsbHidOnbInterface::read(Objnet::CommonMessage &msg)
{
    QByteArray ba;
    bool success = usb->readFeature(ba);
    if (success)
    {
        unsigned long id = *reinterpret_cast<unsigned long*>(ba.data());
        msg.setId(id);
        unsigned char sz = ba[4];
        msg.setData(QByteArray(ba.data() + 5, sz));

        emit message("usbonb", msg); // for debug purposes
    }
//    else
//        qDebug() << "ne success(";
    return success;
}

int UsbHidOnbInterface::availableWriteCount()
{
    return 256;
}

void UsbHidOnbInterface::flush()
{
    qDebug() << "[UsbHidOnbInterface]: flush is not implemented";
}

int UsbHidOnbInterface::addFilter(unsigned long id, unsigned long mask)
{
    qDebug() << "[UsbHidOnbInterface]: Filter is not implemented. id=" << id << "mask=" << mask;
    return 0;
}

void UsbHidOnbInterface::removeFilter(int number)
{
    qDebug() << "[UsbHidOnbInterface]: Filter is not implemented. number=" << number;
}
