#ifndef SERIALPORTWIDGET_H
#define SERIALPORTWIDGET_H

#include <QtWidgets>
#include <qserialport.h>
#include <qserialportinfo.h>
#include "uartinterface.h"

class SerialPortWidget : public QWidget, public UartInterface
{
    Q_OBJECT

private:
    QComboBox *mPorts;
    QSerialPort *mCom;
    int mAutoIdx;
    QString mAutoDesc;

protected:
    //virtual void read(QByteArray &ba) {Q_UNUSED(ba);}

public:
    explicit SerialPortWidget(QWidget *parent = 0);

    void setBaudrate(int baudrate) {mCom->setBaudRate(baudrate);}
    void setBaudRate(int baudrate) {mCom->setBaudRate(baudrate);}
    void setParity(QSerialPort::Parity parity) {mCom->setParity(parity);}
    void setParity(QString parity);
    void setStopBits(QSerialPort::StopBits stopBits) {mCom->setStopBits(stopBits);}
    void setFlowControl(QSerialPort::FlowControl flowControl) {mCom->setFlowControl(flowControl);}

    void autoConnect(QString description);

    virtual void write(const QByteArray &ba) override {mCom->write(ba);}

    QSerialPort *device() {return mCom;}

signals:
    void connected();
    void disconnected();

private slots:
    void onTimer();
    void onDataReady();

public slots:
    void pollPorts();
    void onPortChanged(QString portname);
};

#endif // SERIALPORTWIDGET_H
