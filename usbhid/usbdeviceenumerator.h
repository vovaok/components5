#ifndef USBDEVICEENUMERATOR_H
#define USBDEVICEENUMERATOR_H

#include <QObject>
#include <QCoreApplication>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <dbt.h>
#include "hidapi.h"
#include <QDebug>
#include <QTimer>

class UsbDeviceEnumerator : public QObject
{
    Q_OBJECT

private:
    class DevEventFilter : public QAbstractNativeEventFilter
    {
    private:
        UsbDeviceEnumerator *mEnumObj;
        friend class UsbDeviceEnumerator;
        DevEventFilter(UsbDeviceEnumerator *enumObj);
        bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
    };

public:
    struct DevInfo
    {
        bool present;
        unsigned short vid, pid;
        QString product;
        QString manufacturer;
        unsigned short revision;
        QString serial;
    };

private:
    quint16 mVid, mPid;
    static UsbDeviceEnumerator *mSelf;
    UsbDeviceEnumerator();
    friend class DevEventFilter;
    QMap<QString, DevInfo> mDevs;

private slots:
    void enumerate();

public:
    static UsbDeviceEnumerator *instance();
    void setVidPid(unsigned short vid, unsigned short pid);
    const DevInfo *deviceInfo(QString path);
    QStringList devices() const {return mDevs.keys();}

signals:
    void deviceConnected(QString path);
    void deviceRemoved(QString path);
};

#endif // USBDEVICEENUMERATOR_H
