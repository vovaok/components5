#ifndef USBHIDTHREAD_H
#define USBHIDTHREAD_H

#include <QThread>
#include "usbhid.h"

class UsbHidThread : public QThread
{
    Q_OBJECT

private:
    UsbHid *usb;
    QByteArray mIn, mOut;
    QElapsedTimer mTimer;
    quint16 mVid, mPid;
    int mPollingInterval;
    int mReadSize;
    int mCounter;
    float mRealInterval;
    QMutex mAccessMutex;

    struct Feature
    {
        unsigned char id;
        QByteArray ba;
    };
    QQueue<Feature> mSetFeatureBuffer;
    Feature mCurFe;

protected:
    void run();// override;
    virtual void reportReceivedEvent(const QByteArray &ba) {Q_UNUSED(ba);}
    bool getFeatureUnsafe(int id, QByteArray &ba) {return usb->getFeature(id, ba);}

private slots:
    void onUsbStateChanged(bool active);

public:
    explicit UsbHidThread(quint16 vid=0, quint16 pid=0, QObject *parent=0);
    virtual ~UsbHidThread();

    const QStringList availableDevices() {return usb->availableDevices();}
    void setPollingInterval(int ms) {mPollingInterval = ms;}
    int pollingInterval() const {return mPollingInterval;}

    void setFeature(int id, bool val);
    void setFeature(int id, char val);
    void setFeature(int id, short val);
    void setFeature(int id, long val);
    void setFeature(int id, float val);
    bool setFeature(int id, const QByteArray &ba);
    bool getFeature(int id, QByteArray &ba);

    bool isReady() const {return usb->isOpen();} // temporary!!!!
    float realInterval() const {return mRealInterval;}

    void setCurrentReportId(char id, int readsize) {usb->setCurrentReportId(id); mReadSize = readsize;}
    void setReportData(const QByteArray &ba);

signals:
    void connected();
    void disconnected();
    void ready();
    void reportReceived(const QByteArray &ba);
};

#endif // USBHIDTHREAD_H
