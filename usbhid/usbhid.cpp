#include "usbhid.h"

UsbHid::UsbHid(quint16 vid, quint16 pid, QObject *parent) :
    QIODevice(parent),
    mDev(nullptr),
    mCurrentReportId(0),
    mInfo(nullptr)
{
    //hid_init();
    UsbDeviceEnumerator::instance()->setVidPid(vid, pid);
}

UsbHid::UsbHid(QString path, QObject *parent) :
    QIODevice(parent),
    mDev(nullptr),
    mPath(path),
    mCurrentReportId(0)
{
    //hid_init();
    mInfo = UsbDeviceEnumerator::instance()->deviceInfo(mPath);
}

UsbHid::~UsbHid()
{
    close();
    hid_exit();
}
//---------------------------------------------------------------------------

qint64 UsbHid::readData(char *data, qint64 maxSize)
{
    //qDebug() << "read" << maxSize;
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
        qDebug() << "Read failed";
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
        {
            qDebug() << "SetFeature error:" << QString::fromWCharArray(hid_error(mDev));
            close();
        }
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
//    qDebug() << res;
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

    if (mPath.isEmpty())
        return false;

    mDev = hid_open_path(mPath.toLatin1());
    if (!mDev)
    {
        mode = NotOpen;
        setErrorString("Open device '"+mPath+"' failed");
        QIODevice::close();
    }
    else
    {
        QIODevice::open(mode | QIODevice::Unbuffered);
    }
    //QIODevice::open(mode);

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

const QStringList UsbHid::availableDevices()
{
    return UsbDeviceEnumerator::instance()->devices();
}

void UsbHid::setDevice(QString path)
{
    QStringList devList = UsbDeviceEnumerator::instance()->devices();
    if (devList.contains(path))
        mPath = path;
    else if (!devList.isEmpty())
        mPath = devList.first();
    else
        mPath = QString();
}
//---------------------------------------------------------

QString UsbHid::deviceName() const
{
    return mInfo? mInfo->product: QString();
}

unsigned short UsbHid::releaseNumber() const
{
    return mInfo? mInfo->revision: 0;
}

QString UsbHid::serial() const
{
    return mInfo? mInfo->serial: QString();
}

QString UsbHid::manufacturer() const
{
    return mInfo? mInfo->manufacturer: QString();
}
//---------------------------------------------------------------------------
