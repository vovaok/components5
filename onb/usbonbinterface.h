#ifndef USBHIDONBINTERFACE_H
#define USBHIDONBINTERFACE_H

#include "objnetInterface.h"
#include "usbhidthread.h"
#include <QElapsedTimer>

namespace Objnet
{

class UsbOnbThread : public UsbHidThread
{
private:
    unsigned char mReadSeqNo, mSeqNo;
    bool mFirstTime;
    QQueue<QByteArray> mGetFeatureBuffer;
    QMutex mMutex;

protected:
    virtual void reportReceivedEvent(const QByteArray &ba) override
    {
        if (ba.size() >= 2)
        {
            unsigned char sz = (unsigned char)ba[0];
            mReadSeqNo = (unsigned char)ba[1];
            if (mFirstTime)
            {
                mSeqNo = mReadSeqNo - sz;
                mFirstTime = false;
            }
        }

        if (mReadSeqNo == mSeqNo)
            return;

        QByteArray ba2;
        bool success = getFeatureUnsafe(0x01, ba2);
        if (success)
        {
            mSeqNo++;
            mMutex.lock();
            if (mGetFeatureBuffer.size() > 64)
                qDebug() << "[UsbOnbThread] getFeature queue is a little big:" << mGetFeatureBuffer.size();
            mGetFeatureBuffer.enqueue(ba2);
            mMutex.unlock();
        }
    }
public:
    explicit UsbOnbThread(QObject *parent=0) :
        UsbHidThread(0x0bad, 0xcafe, parent),
        mReadSeqNo(0), mSeqNo(0), mFirstTime(true)
    {
        setPollingInterval(1);
        setCurrentReportId(0x23, 2);
    }

    bool readFeature(QByteArray &ba)
    {
        if (mGetFeatureBuffer.isEmpty())
            return false;
        mMutex.lock();
        ba = mGetFeatureBuffer.dequeue();
        mMutex.unlock();
        return true;
    }
};

class UsbHidOnbInterface : public QObject, public ObjnetInterface
{
    Q_OBJECT

private:
    UsbOnbThread *usb;
    typedef struct
    {
        uint32_t id;
        uint32_t mask;
    } Filter;
    QVector<Filter> mFilters;

public:
    explicit UsbHidOnbInterface(UsbOnbThread *usbhid);
    virtual ~UsbHidOnbInterface();

    bool write(CommonMessage &msg);
    bool read(CommonMessage &msg);
    void flush();

    int availableWriteCount();

    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);

signals:
    void message(QString, CommonMessage&);
};

}

#endif // USBHIDONBINTERFACE_H
