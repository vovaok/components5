#include "DonglePort.h"
#include "deviceenumerator.h"

DonglePort::DonglePort(QObject *parent) : QSerialPort(parent)
{
    setBaudRate(1000000);
    setParity(QSerialPort::EvenParity);
    setStopBits(QSerialPort::OneStop);
    setFlowControl(QSerialPort::NoFlowControl);
    connect(this, SIGNAL(readyRead()), this, SLOT(onDataReady()));

    connect(DeviceEnumerator::instance(), SIGNAL(deviceConnected(QString)), this, SLOT(onDeviceConnected(QString)));
    connect(DeviceEnumerator::instance(), SIGNAL(deviceRemoved(QString)), this, SLOT(onDeviceDisconnected(QString)));
}

DonglePort::~DonglePort()
{
    disconnect(DeviceEnumerator::instance(), SIGNAL(deviceConnected(QString)), this, SLOT(onDeviceConnected(QString)));
    disconnect(DeviceEnumerator::instance(), SIGNAL(deviceRemoved(QString)), this, SLOT(onDeviceDisconnected(QString)));
}

void DonglePort::changePort(QString portName)
{
    if (isOpen())
    {
        close();
        emit disconnected();
    }

    if (!portName.isEmpty())
    {
        setPortName(portName);
        if (open(QIODevice::ReadWrite))
            emit connected();
    }
}

void DonglePort::autoConnectTo(QString value)
{
    serial = value;
}

void DonglePort::disableAutoRead()
{
    disconnect(this, SIGNAL(readyRead()), this, SLOT(onDataReady()));
}

void DonglePort::onDataReady()
{
    QByteArray byteArray = readAll();

    emit dataReceived(byteArray);
}

void DonglePort::onDeviceConnected(QString port)
{
    const QSerialPortInfo &info = DeviceEnumerator::instance()->getInfo(port);
    if(info.description().compare(serial, Qt::CaseInsensitive) == 0 && !info.isBusy() && !isOpen())
        changePort(port);
}

void DonglePort::onDeviceDisconnected(QString)
{
    changePort("");
}
