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
    QSerialPort *mCom;
    DeviceEnumerator *mEnum;
    QString mAutoDesc;

protected:
    virtual void read(QByteArray &ba) {Q_UNUSED(ba);}

public:
    explicit SerialPortWidget(QWidget *parent = 0);

    QIODevice *getDevice() {return mCom;}
    void setBaudrate(int baudrate) {mCom->setBaudRate(baudrate);}
    void setParity(QSerialPort::Parity parity) {mCom->setParity(parity);}
    void setParity(QString parity);
    void setStopBits(QSerialPort::StopBits stopBits) {mCom->setStopBits(stopBits);}
    void setStopBits(float stopbits);
    void setDataBits(QSerialPort::DataBits databits) {mCom->setDataBits(databits);}
    void setFlowControl(QSerialPort::FlowControl flowControl) {mCom->setFlowControl(flowControl);}

    void disableAutoRead() {disconnect(mCom, SIGNAL(readyRead()), this, SLOT(onDataReady()));}
    void autoConnect(QString description);

    virtual void write(const QByteArray &ba) override {mCom->write(ba);}
    //qint64 write(const QByteArray &ba) {return mCom->isOpen()? mCom->write(ba): 0;}

    bool isActive() const {return mCom->isOpen();}
    QSerialPort *device() {return mCom;}

    QComboBox *getPortsComboBox() {return mPorts;}

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &ba);

private slots:
    void onDataReady();
    void onDeviceConnected(QString port);
    void onDeviceDisconnected(QString port);

public slots:
    void onPortChanged(QString portname);
};

#endif // SERIALPORTWIDGET_H
