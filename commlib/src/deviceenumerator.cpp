#include "deviceenumerator.h"

DeviceEnumerator *DeviceEnumerator::mSelf = nullptr;

DeviceEnumerator::DeviceEnumerator() : QObject(nullptr)
{
    mSelf = this;
    new DevEventFilter(this);
    QTimer::singleShot(100, this, SLOT(enumerate()));
}

void DeviceEnumerator::enumerate()
{
    foreach (QString p, mPresence.keys())
        mPresence[p] = false;

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (int i=0; i<ports.size(); i++)
    {
        QSerialPortInfo &info = ports[i];
        QString name = info.portName();
        mPresence[name] = true;
        if (!mDevs.contains(name))
        {
            mDevs[name] = info;
            emit deviceConnected(name);
        }
    }

    foreach (QString p, mDevs.keys())
    {
        if (!mPresence[p])
        {
            emit deviceRemoved(p);
            mDevs.remove(p);
            mPresence.remove(p);
        }
    }
}

DeviceEnumerator *DeviceEnumerator::instance()
{
    if (!mSelf)
        new DeviceEnumerator();
    return mSelf;
}
//---------------------------------------------------------

DeviceEnumerator::DevEventFilter::DevEventFilter(DeviceEnumerator *enumObj) :
    mEnumObj(enumObj)
{
    qApp->installNativeEventFilter(this);
}

bool DeviceEnumerator::DevEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    auto pWindowsMessage = static_cast<MSG*>(message);
    auto wParam = pWindowsMessage->wParam;
    if (wParam == DBT_DEVNODES_CHANGED)
    {
        mEnumObj->enumerate();
    }
    return false;
}
