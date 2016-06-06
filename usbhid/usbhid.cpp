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
        bytesRead = hid_read_timeout(mDev, reinterpret_cast<unsigned char*>(data), maxSize, 0);
    }
    else
    {
        QByteArray temp(65, '\0');
        temp[0] = mCurrentReportId;
        bytesRead = hid_read_timeout(mDev, reinterpret_cast<unsigned char*>(temp.data()), 65, 0);
        if (!bytesRead)
            return 0;
        --bytesRead;
        for (int i=0; i<bytesRead; i++)
            data[i] = temp[i+1];

//        unsigned char *buf = new unsigned char[maxSize + 1];
//        bytesRead = hid_read_timeout(mDev, buf, maxSize+1, 100);
//        bytesRead--;
//        for (int i=0; i<bytesRead; i++)
//            data[i] = buf[i+1];
//        delete [] buf;
    }

    if (bytesRead < 0)
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
    QByteArray temp(maxSize+1, '\0');
    temp[0] = mCurrentReportId; // report ID;
    for (int i=0; i<maxSize; i++)
        temp[i+1] = data[i];
    int bytesWritten = hid_write(mDev, reinterpret_cast<const unsigned char*>(temp.constData()), maxSize+1);

//    unsigned char *idData = new unsigned char[maxSize+1];
//    idData[0] = mCurrentReportId; // report ID;
//    for (int i=0; i<maxSize; i++)
//        idData[i+1] = data[i];
//    int bytesWritten = hid_write(mDev, reinterpret_cast<const unsigned char*>(idData), maxSize+1);
//    delete [] idData;

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
    //qDebug() << "UsbHid: setFeature";
    if (!mDev)
        return;
    QByteArray ba;
    ba.append((char)reportId);
    ba.append(data);
    int bytesWritten = hid_send_feature_report(mDev, reinterpret_cast<const unsigned char*>(ba.data()), ba.size());
    //qDebug() << "UsbHid: written =" << bytesWritten;
    if (bytesWritten == -1)
    {
        if (mDev)
            qDebug() << "SetFeature error:" << QString::fromWCharArray(hid_error(mDev));
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

int UsbHid::getNumInputBuffers()
{
    return hid_get_num_input_buffers(mDev);
}

QByteArray UsbHid::getInputReport(int length)
{
    static QByteArray ba;
    ba.resize(length+1);
    ba[0] = mCurrentReportId;
    unsigned char res = hid_get_input_report(mDev, ba.data(), length+1);
    qDebug() << res;
    if (res)
    {
        ba = ba.remove(0, 1);
//        ba.resize(length);
    }
    else
        ba.clear();
    return ba;
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

    mBoardProperties.clear();

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

    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(mVid, mPid); // this is dynamic list

    for (cur_dev=devs; cur_dev; cur_dev=cur_dev->next)
    {
        QString serial = QString::fromWCharArray(cur_dev->serial_number);
        mBoardMap[serial] = cur_dev->path;

        mBoardProperties["interface number"] = cur_dev->interface_number;
        mBoardProperties["manufacturer_string"] = QString::fromWCharArray(cur_dev->manufacturer_string);
        mBoardProperties["path"] = cur_dev->path;
        mBoardProperties["product id"] = cur_dev->product_id;
        mBoardProperties["product string"] = QString::fromWCharArray(cur_dev->product_string);
        mBoardProperties["release number"] = cur_dev->release_number;
        mBoardProperties["serial number"] = QString::fromWCharArray(cur_dev->serial_number);
        mBoardProperties["usage"] = cur_dev->usage;
        mBoardProperties["usage page"] = cur_dev->usage_page;
        mBoardProperties["vendor id"] = cur_dev->vendor_id;

        //qDebug() << mBoardProperties;
    }

    hid_free_enumeration(devs);
}
//---------------------------------------------------------------------------
