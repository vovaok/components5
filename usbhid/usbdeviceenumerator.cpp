#include "usbdeviceenumerator.h"

UsbDeviceEnumerator *UsbDeviceEnumerator::mSelf = nullptr;

UsbDeviceEnumerator::UsbDeviceEnumerator() : QObject(nullptr)
{
    hid_init();
    mSelf = this;
    new DevEventFilter(this);
    QTimer::singleShot(100, this, SLOT(enumerate()));
}

void UsbDeviceEnumerator::enumerate()
{
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(mVid, mPid); // this is dynamic list

    foreach (QString p, mDevs.keys())
        mDevs[p].present = false;

    for (cur_dev=devs; cur_dev; cur_dev=cur_dev->next)
    {
        QString path = cur_dev->path;
        DevInfo info;
        info.present = true;
        info.vid = cur_dev->vendor_id;
        info.pid = cur_dev->product_id;
        info.product = QString::fromWCharArray(cur_dev->product_string);
        info.manufacturer = QString::fromWCharArray(cur_dev->manufacturer_string);
        info.revision = cur_dev->release_number;
        info.serial = QString::fromWCharArray(cur_dev->serial_number);
        if (!mDevs.contains(path))
        {
            mDevs[path] = info;
            emit deviceConnected(path);
        }
        else
        {
            mDevs[path].present = true;
        }

//        QString serial = QString::fromWCharArray(cur_dev->serial_number);
//        mBoardMap[serial] = cur_dev->path;

//        mBoardProperties["interface number"] = cur_dev->interface_number;
//        mBoardProperties["path"] = cur_dev->path;
//        mBoardProperties["usage"] = cur_dev->usage;
//        mBoardProperties["usage page"] = cur_dev->usage_page;

        //qDebug() << mBoardProperties;
    }

    hid_free_enumeration(devs);

    foreach (QString p, mDevs.keys())
    {
        if (!mDevs[p].present)
        {
            emit deviceRemoved(p);
            mDevs.remove(p);
        }
    }
}

UsbDeviceEnumerator *UsbDeviceEnumerator::instance()
{
    if (!mSelf)
        new UsbDeviceEnumerator();
    return mSelf;
}

void UsbDeviceEnumerator::setVidPid(unsigned short vid, unsigned short pid)
{
    mVid = vid;
    mPid = pid;
}

const UsbDeviceEnumerator::DevInfo *UsbDeviceEnumerator::deviceInfo(QString path)
{
    if (mDevs.contains(path))
        return &mDevs[path];
    return nullptr;
}
//---------------------------------------------------------

UsbDeviceEnumerator::DevEventFilter::DevEventFilter(UsbDeviceEnumerator *enumObj) :
    mEnumObj(enumObj)
{
    qApp->installNativeEventFilter(this);
}

bool UsbDeviceEnumerator::DevEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    auto pWindowsMessage = static_cast<MSG*>(message);
    auto wParam = pWindowsMessage->wParam;
    if (wParam == DBT_DEVNODES_CHANGED)
    {
        mEnumObj->enumerate();
//        qDebug() << "DBT_DEVNODES_CHANGED";
    }
    return false;
}
