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

bool UsbHidOnbInterface::send(const CommonMessage &msg)
{
    if (!usb->isReady()) //isOpen())
        return false;
    uint32_t id = msg.rawId();
    unsigned char sz = msg.data().size();
    QByteArray ba;
    ba.resize(13);
    *reinterpret_cast<uint32_t*>(ba.data()) = id;
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
        uint32_t id = *reinterpret_cast<uint32_t*>(ba.data());
        success = false;
        for (const Filter &filter: mFilters)
            if ((id & filter.mask) == (filter.id & filter.mask))
                success = true;

        if (success)
        {
            msg.setId(id);
            unsigned char sz = static_cast<unsigned char>(ba[4]);
            msg.setData(QByteArray(ba.data() + 5, sz));

            receive(std::move(msg));

            emit message("usbonb", msg); // for debug purposes
        }
    }
//    else
//        qDebug() << "ne success(";
//    return success;
    return ObjnetInterface::read(msg);
}

//void UsbHidOnbInterface::flush()
//{
//    qDebug() << "[UsbHidOnbInterface]: flush is not implemented";
//}

int UsbHidOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    mFilters << Filter{id, mask};
    return mFilters.size() - 1;
}

void UsbHidOnbInterface::removeFilter(int number)
{
    if (number >= 0 && number < mFilters.size())
        mFilters[number] = Filter{0xFFFFFFFF, 0xFFFFFFFF};
}
