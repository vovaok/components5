#ifndef SERIALONBINTERFACE_H
#define SERIALONBINTERFACE_H

#include "objnetInterface.h"
#include "serialframe.h"
#include <QSerialPort>
#include <QQueue>

namespace Objnet
{

class SerialThread : public QThread
{
    Q_OBJECT
public:
    SerialThread(QObject *parent=nullptr);
    bool sendData(const QByteArray &ba);
    bool isActive() const;

public slots:
    void setPort(QString name);

signals:
    void dataReceived(const QByteArray &ba);

//    void internalSendData(const QByteArray &ba);
//    void internalPortChange(QString name);

protected:
    virtual void run() override;

private:
    QSerialPort *m_dev = nullptr;
//    SerialFrame *m_serial = nullptr;
    QElapsedTimer m_timer;
//    QByteArray m_buffer;
    QString m_portName;
    QByteArray m_buffer;
    QMutex m_mutex;
    bool m_busy = false;

    QByteArray encode(const QByteArray &ba);
    QByteArray decode(const QByteArray &ba);

    void changePort(QString name);
//private slots:
//    void onDataReceived(const QByteArray &ba);
};

class SerialOnbInterface : public QObject, public ObjnetInterface
{
    Q_OBJECT

private:
//    SerialFrame *mSerial;
    SerialThread *m_serial;
//    QElapsedTimer m_timeoutTimer;

public:
    explicit SerialOnbInterface(/*QIODevice *device,*/ bool half_duplex=false);
    virtual ~SerialOnbInterface();

    virtual bool isBusPresent() const override;

    virtual bool send(const CommonMessage &msg) override;

    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF);
    void removeFilter(int number);

public slots:
    void setPort(QString name) {m_serial->setPort(name);}

signals:
    void message(QString, const CommonMessage&);

private slots:
    void onDataReceived(const QByteArray &ba);
};

}

#endif // SERIALONBINTERFACE_H
