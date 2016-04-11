#ifndef USBHID_H
#define USBHID_H

#include <QtCore>
#include <QIODevice>
#include "hidapi.h"

class UsbHid : public QIODevice
{
    Q_OBJECT

private:
    hid_device *mDev;
    QString mName;
    unsigned char mCurrentReportId;
//    QByteArray mOutReportBuffer;
//    TheReport *mOutReport;

    static quint16 mVid, mPid;
    static QMap<QString, QString> mBoardMap; // serial -> path_to_open
    QMap<QString, QVariant> mBoardProperties;

    void enumerateBoards();

protected:
    virtual qint64 readData(char *data, qint64 maxSize);
    virtual qint64 readLineData (char *data, qint64 maxSize);
    virtual qint64 writeData(const char *data, qint64 maxSize);

public:
    explicit UsbHid(quint16 vid=0, quint16 pid=0, QObject *parent = 0);
    virtual ~UsbHid();

    void setVidPid(quint16 vid, quint16 pid) {mVid = vid; mPid = pid;}
    quint16 vid() const {return mVid;}
    quint16 pid() const {return mPid;}

    const QStringList availableDevices() {enumerateBoards(); return mBoardMap.keys();}
    void setDevice(QString name=QString()) {mName = mBoardMap.contains(name)? name: mBoardMap.count()? mBoardMap.keys().first(): "";}
    QString deviceName() const {return mName;}

    virtual bool isSequential() const {return true;} // doesn't have random access

    void setCurrentReportId(unsigned char reportId) {mCurrentReportId = reportId;}
    unsigned char currentReportId() const {return mCurrentReportId;}

    void setFeature(unsigned char reportId, const QByteArray &data);
    bool getFeature(unsigned char reportId, QByteArray &data);

    QString getString(unsigned char index);

    unsigned short releaseNumber() const {return mBoardProperties["release number"].toInt();}

signals:
//    void connected(QString boardName);      // new device enumerated
//    void disconnected(QString boardName);   // device removed
    void stateChanged(bool active);
    void readyRead();

public slots:
    virtual bool open(OpenMode mode = ReadWrite);
    virtual void close();
};

#endif // USBHID_H
