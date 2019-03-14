#include "deviceenumerator.h"
#include <QSerialPort>

DeviceEnumerator *DeviceEnumerator::mSelf = nullptr;

DeviceEnumerator::DeviceEnumerator() : QObject(nullptr)
{
    mSelf = this;
    new DevEventFilter(this);
    QTimer::singleShot(100, this, SLOT(enumerate()));
}

void DeviceEnumerator::enumerate()
{
    for (auto& name : mPresence.keys()) mPresence[name] = false;

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for(auto& info : ports)
    {
        QString name = info.portName();

        mPresence[name] = info.isValid();
        if (!mDevs.contains(name))
        {
            mDevs[name] = info;
            emit deviceConnected(name);
        }

    }

    for (auto& name : mDevs.keys())
    {
        if (!mPresence[name])
        {
            emit deviceRemoved(name);
            mDevs.remove(name);
            mPresence.remove(name);
        }
    }
}

DeviceEnumerator *DeviceEnumerator::instance()
{
    if (!mSelf) new DeviceEnumerator();

    return mSelf;
}

DeviceEnumerator::DevEventFilter::DevEventFilter(DeviceEnumerator *enumObj) : mEnumObj(enumObj)
{
    qApp->installNativeEventFilter(this);
}

bool DeviceEnumerator::DevEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    auto pWindowsMessage = static_cast<MSG*>(message);
    auto uMsg = pWindowsMessage->message;

    auto wParam = pWindowsMessage->wParam;
    if (wParam == DBT_DEVNODES_CHANGED && uMsg == WM_DEVICECHANGE)
    {
        mEnumObj->enumerate();
    }
    return false;
}
