#ifndef USBHID_H
#define USBHID_H

#include <QtCore>
#include <QIODevice>
#include "hidapi.h"
#include "usbdeviceenumerator.h"

class UsbHid : public QIODevice
{
    Q_OBJECT

private:
    hid_device *mDev;
    QString mPath;
    unsigned char mCurrentReportId;
    const UsbDeviceEnumerator::DevInfo *mInfo;

protected:
    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 readLineData (char *data, qint64 maxSize);
    virtual qint64 writeData(const char *data, qint64 maxSize);

public:
    explicit UsbHid(quint16 vid=0, quint16 pid=0, QObject *parent = 0);
    explicit UsbHid(QString path, QObject *parent = 0);
    virtual ~UsbHid();

//    void setVidPid(quint16 vid, quint16 pid) {mVid = vid; mPid = pid;}
//    quint16 vid() const {return mVid;}
//    quint16 pid() const {return mPid;}

    const QStringList availableDevices();
    void setDevice(QString path = QString());
    QString deviceName() const;
    unsigned short releaseNumber() const;
    QString serial() const;
    QString manufacturer() const;

    virtual bool isSequential() const {return true;} // doesn't have random access

    void setCurrentReportId(unsigned char reportId) {mCurrentReportId = reportId;}
    unsigned char currentReportId() const {return mCurrentReportId;}

    void setFeature(unsigned char reportId, const QByteArray &data);
    bool getFeature(unsigned char reportId, QByteArray &data);

    QString getString(unsigned char index);

    int getNumInputBuffers();
    QByteArray getInputReport(int length);

signals:
    void stateChanged(bool active);
    void readyRead();

public slots:
    virtual bool open(OpenMode mode = ReadWrite);
    virtual void close();
};

#endif // USBHID_H
