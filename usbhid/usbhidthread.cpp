#include "usbhidthread.h"

UsbHidThread::UsbHidThread(quint16 vid, quint16 pid, QObject *parent) :
    QThread(parent),
    mVid(vid), mPid(pid),
    mPollingInterval(30),
    mReadSize(0),
    mCounter(0),
    mRealInterval(0)
{
    usb = new UsbHid(mVid, mPid);
    connect(usb, SIGNAL(stateChanged(bool)), SLOT(onUsbStateChanged(bool)));
}

UsbHidThread::~UsbHidThread()
{
    requestInterruption();
    while (!isFinished());
    delete usb;
}

void UsbHidThread::setReportData(const QByteArray &ba)
{
    // mb stoit mutex pouzat
    mOut = ba;
}

void UsbHidThread::run()
{
    mTimer.start();

    while (!isInterruptionRequested())
    {
        if (usb->isOpen())
        {
            if (mReadSize)
            {
                mIn = usb->read(mReadSize);
                if (mIn.size())
                    emit reportReceived(mIn);
            }

            if (mOut.size())
            {
//                out.append(mCounter++);
                usb->write(mOut);
            }
        }
        else // autoconnect
        {
            QStringList devs = availableDevices();
            if (devs.count() == 1)
                usb->setDevice();
            usb->open();
        }

        int usecs = mTimer.nsecsElapsed() / 1000;
        mTimer.restart();
        mRealInterval = usecs * 1e-6;
        usecs = mPollingInterval*1000 - usecs;
//        if (usecs > 0)
//            usleep(usecs);
        msleep(mPollingInterval);
    }
}
//---------------------------------------------------------

void UsbHidThread::setFeature(int id, bool val)
{
    QByteArray ba;
    ba.append(val? '\1': '\0');
    usb->setFeature(id, ba);
}

void UsbHidThread::setFeature(int id, char val)
{
    QByteArray ba;
    ba.append(val);
    usb->setFeature(id, ba);
}

void UsbHidThread::setFeature(int id, short val)
{
    QByteArray ba(reinterpret_cast<const char*>(&val), sizeof(short));
    usb->setFeature(id, ba);
}

void UsbHidThread::setFeature(int id, long val)
{
    QByteArray ba(reinterpret_cast<const char*>(&val), sizeof(long));
    usb->setFeature(id, ba);
}

void UsbHidThread::setFeature(int id, float val)
{
    QByteArray ba(reinterpret_cast<const char*>(&val), sizeof(float));
    usb->setFeature(id, ba);
}
//---------------------------------------------------------------------------

void UsbHidThread::onUsbStateChanged(bool active)
{
    if (active)
    {
        emit connected();
        emit ready();
    }
    else
    {
        emit disconnected();
    }
}
//---------------------------------------------------------------------------
