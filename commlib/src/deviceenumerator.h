#ifndef DEVICEENUMERATOR_H
#define DEVICEENUMERATOR_H

#include <QObject>
#include <QCoreApplication>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <dbt.h>
#include <QDebug>
#include <QTimer>
#include <QSerialPortInfo>

class DeviceEnumerator : public QObject
{
    Q_OBJECT

private:
    class DevEventFilter : public QAbstractNativeEventFilter
    {
    private:
        DeviceEnumerator *mEnumObj;
        friend class DeviceEnumerator;
        DevEventFilter(DeviceEnumerator *enumObj);
        bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
    };

private:
    static DeviceEnumerator *mSelf;
    DeviceEnumerator();
    friend class DevEventFilter;
    QMap<QString, QSerialPortInfo> mDevs;
    QMap<QString, bool> mPresence;

public slots:
    void enumerate();

public:
    static DeviceEnumerator *instance();
    QStringList devices() const {return mDevs.keys();}
    const QSerialPortInfo &getInfo(QString path) {return mDevs[path];}

signals:
    void deviceConnected(QString path);
    void deviceRemoved(QString path);
};

#endif // USBDEVICEENUMERATOR_H
