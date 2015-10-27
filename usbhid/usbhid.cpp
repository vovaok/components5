#include "usbhid.h"

quint16 UsbHid::mVid = 0;
quint16 UsbHid::mPid = 0;
QMap<QString, QString> UsbHid::mBoardMap;

UsbHid::UsbHid(quint16 vid, quint16 pid, QObject *parent) :
    QIODevice(parent),
    mDev(0L),
    mCurrentReportId(0)
{
    mVid = vid;
    mPid = pid;
    hid_init();
}

UsbHid::~UsbHid()
{
    close();
    hid_exit();
}
//---------------------------------------------------------------------------

qint64 UsbHid::readData(char *data, qint64 maxSize)
{
    if (!maxSize)
        return 0; // FUCKING HACK!!!!

    int bytesRead;
    if (!mCurrentReportId)
    {
        bytesRead = hid_read_timeout(mDev, reinterpret_cast<unsigned char*>(data), maxSize, 100);
    }
    else
    {
        unsigned char *buf = new unsigned char[maxSize + 1];
        bytesRead = hid_read_timeout(mDev, buf, maxSize+1, 100);
        bytesRead--;
        for (int i=0; i<bytesRead; i++)
            data[i] = buf[i+1];
        delete [] buf;
    }

    if (bytesRead == -1)
    {
        close();
        setErrorString("Read failed");
    }
    return bytesRead;
}

qint64 UsbHid::readLineData(char *data, qint64 maxSize)
{
    return readData(data, maxSize);
}

qint64 UsbHid::writeData(const char *data, qint64 maxSize)
{
    unsigned char *idData = new unsigned char[maxSize+1];
    idData[0] = mCurrentReportId; // report ID;
    for (int i=0; i<maxSize; i++)
        idData[i+1] = data[i];
    int bytesWritten = hid_write(mDev, reinterpret_cast<const unsigned char*>(idData), maxSize+1);
    delete [] idData;
    if (bytesWritten == -1)
    {
        close();
        setErrorString("Write failed");
    }
    return bytesWritten;
}
//---------------------------------------------------------------------------

void UsbHid::setFeature(unsigned char reportId, const QByteArray &data)
{
    if (!mDev)
        return;
    QByteArray ba;
    ba.append((char)reportId);
    ba.append(data);
    int bytesWritten = hid_send_feature_report(mDev, reinterpret_cast<const unsigned char*>(ba.data()), ba.size());
    if (bytesWritten == -1)
    {
        setErrorString("Set feature failed");
    }
}

bool UsbHid::getFeature(unsigned char reportId, QByteArray &data)
{
    if (!mDev)
        return false;
    QByteArray ba = QByteArray(65, 0);
    ba[0] = reportId;
    int bytesRead = hid_get_feature_report(mDev, reinterpret_cast<unsigned char*>(ba.data()), ba.size());
    if (bytesRead == -1 || ba.size() <= 1 || ((unsigned char)(ba.data()[0])) != reportId)
    {
        setErrorString("Get feature failed");
        return false;
    }
    else
    {
        data = ba.mid(1, bytesRead-1);
        return true;
    }
}
//---------------------------------------------------------------------------

QString UsbHid::getString(unsigned char index)
{
    QString s;
    if (!mDev)
        return s;
    wchar_t *buf = new wchar_t[256];
    int fail = hid_get_indexed_string(mDev, index, buf, 256);
    if (fail)
    {
        setErrorString("Get indexed string failed");
    }
    else
    {
        s = QString::fromWCharArray(buf);
        delete [] buf;
    }
    return s;
}
//---------------------------------------------------------------------------

bool UsbHid::open(QIODevice::OpenMode mode)
{
    if (mDev)
        close();

    mDev = hid_open_path(mBoardMap[mName].toLatin1());
    if (!mDev)
    {
        mode = NotOpen;
        setErrorString("Open device '"+mName+"' failed");
    }
    QIODevice::open(mode);

    if (isOpen())
        emit stateChanged(true);

    return mDev;
}

void UsbHid::close()
{
    if (mDev)
    {
        hid_close(mDev);
        mDev = 0L;
    }
    QIODevice::close();

    emit stateChanged(false);
}
//---------------------------------------------------------------------------

void UsbHid::enumerateBoards()
{
    QString curPath;
    if (mDev)
        curPath = mBoardMap[mName];
    mBoardMap.clear();
    if (!curPath.isEmpty())
        mBoardMap[mName] = curPath;
    //QString manufacturer_string("Neurobotics");
    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(mVid, mPid); // this is dynamic list
//    qDebug() << devs;
    for (cur_dev=devs; cur_dev; cur_dev=cur_dev->next)
    {
//        if (QString::fromWCharArray(cur_dev->manufacturer_string) == manufacturer_string)
//        {
            QString serial = QString::fromWCharArray(cur_dev->serial_number);
//            qDebug() << "serial=" << serial;
            mBoardMap[serial] = cur_dev->path;
//        }
                qDebug()<<"interface_number" << cur_dev->interface_number <<"manufacturer_string" <<cur_dev->manufacturer_string <<"next" <<cur_dev->next<< "path" <<cur_dev->path
                    <<"product id"  <<cur_dev->product_id <<"product string" <<cur_dev->product_string <<"release number" <<cur_dev->release_number <<"serial number" <<cur_dev->serial_number
                       <<"usage" <<cur_dev->usage <<"usage page" <<cur_dev->usage_page <<"vendor id" <<cur_dev->vendor_id;
    }
    hid_free_enumeration(devs);
//    foreach (QString key, mBoardMap.keys())
//        qDebug() << "map_key=" << key;



}
//---------------------------------------------------------------------------
