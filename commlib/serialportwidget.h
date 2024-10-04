#ifndef SERIALPORTWIDGET_H
#define SERIALPORTWIDGET_H

#include <QtWidgets>
#include <qserialport.h>
#include <qserialportinfo.h>
#include "deviceenumerator.h"
#include "uartinterface.h"

class SerialPortWidget : public QWidget, public UartInterface
{
    Q_OBJECT

private:
    QComboBox *mPorts;
    QSerialPort *mCom = nullptr;
    DeviceEnumerator *mEnum;
    QString mAutoDesc;
//    QThread mThread;

protected:
    virtual void read(QByteArray &ba) {Q_UNUSED(ba);}

public:
    explicit SerialPortWidget(QSerialPort *device=nullptr, QWidget *parent=nullptr);
    virtual ~SerialPortWidget();

    QIODevice *getDevice() {return mCom;}
    void setBaudrate(int baudrate) {if (mCom) mCom->setBaudRate(baudrate);}
    void setParity(QSerialPort::Parity parity) {if (mCom) mCom->setParity(parity);}
    void setParity(QString parity);
    void setStopBits(QSerialPort::StopBits stopBits) {if (mCom) mCom->setStopBits(stopBits);}
    void setStopBits(float stopbits);
    void setDataBits(QSerialPort::DataBits databits) {if (mCom) mCom->setDataBits(databits);}
    void setFlowControl(QSerialPort::FlowControl flowControl) {if (mCom) mCom->setFlowControl(flowControl);}

    void disableAutoRead();
    void autoConnect(QString description);

    virtual void write(const QByteArray &ba) override {if (mCom) mCom->write(ba);}
    //qint64 write(const QByteArray &ba) {return mCom->isOpen()? mCom->write(ba): 0;}

    bool isActive() const {return mCom && mCom->isOpen();}
    QSerialPort *device() {return mCom;}

    QComboBox *getPortsComboBox() {return mPorts;}

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &ba);
    void portChanged(QString name);

private slots:
    void onDataReady();
    void onDeviceConnected(QString port);
    void onDeviceDisconnected(QString port);

public slots:
    void onPortChanged(QString portname);
};

#endif // SERIALPORTWIDGET_H
