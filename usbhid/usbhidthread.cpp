#include "usbhidthread.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

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
    // zachem bilo vipilivat mutex?? komu on meshal?
    mAccessMutex.lock();
    mOut = ba;
    mAccessMutex.unlock();
}

void UsbHidThread::run()
{
    mTimer.start();

    while (!isInterruptionRequested())
    {
        mAccessMutex.lock();
        QStringList devs = availableDevices();
        if (devs.isEmpty())
            usb->close();

        if (usb->isOpen())
        {
            if (mReadSize)
            {
                mIn = usb->read(mReadSize);
                if (mIn.size())
                {
                    reportReceivedEvent(mIn);
                    emit reportReceived(mIn);
                }
            }

            if (mOut.size())
            {
                usb->write(mOut);
            }

            while (!mSetFeatureBuffer.isEmpty())
            {
                mCurFe = mSetFeatureBuffer.dequeue();
                usb->setFeature(mCurFe.id, mCurFe.ba);
            }
        }
        else // autoconnect
        {
//            QStringList devs = availableDevices();
            if (devs.count() == 1)
                usb->setDevice();
            usb->open();
        }
        mAccessMutex.unlock();

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
    setFeature(id, ba);
}

void UsbHidThread::setFeature(int id, char val)
{
    QByteArray ba;
    ba.append(val);
    setFeature(id, ba);
}

void UsbHidThread::setFeature(int id, short val)
{
    QByteArray ba(reinterpret_cast<const char*>(&val), sizeof(short));
    setFeature(id, ba);
}

void UsbHidThread::setFeature(int id, long val)
{
    QByteArray ba(reinterpret_cast<const char*>(&val), sizeof(long));
    setFeature(id, ba);
}

void UsbHidThread::setFeature(int id, float val)
{
    QByteArray ba(reinterpret_cast<const char*>(&val), sizeof(float));
    setFeature(id, ba);
}

bool UsbHidThread::setFeature(int id, const QByteArray &ba)
{
    if (usb->isOpen() && mSetFeatureBuffer.size() < 64)
    {
        QByteArray bacopy(ba.data(), ba.size());
        Feature fe = {(unsigned char)id, bacopy};
        mAccessMutex.lock();
        mSetFeatureBuffer.enqueue(fe);
        mAccessMutex.unlock();
        return true;
    }
    return false;
}

bool UsbHidThread::getFeature(int id, QByteArray &ba)
{
    mAccessMutex.lock();
    bool result = usb->getFeature(id, ba);
    mAccessMutex.unlock();
    return result;
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
