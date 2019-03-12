#include "serialportwidget.h"

SerialPortWidget::SerialPortWidget(QWidget *parent) :
    QWidget(parent)
{
    mCom = new QSerialPort();
    mCom->setBaudRate(62500);
    mCom->setParity(QSerialPort::EvenParity);
    mCom->setStopBits(QSerialPort::OneStop);
    mCom->setFlowControl(QSerialPort::NoFlowControl);
    connect(mCom, SIGNAL(readyRead()), SLOT(onDataReady()));

    mPorts = new QComboBox();
    mPorts->setMinimumWidth(100);
    mPorts->addItem("- Выберите порт -");
    connect(mPorts, SIGNAL(activated(QString)), SLOT(onPortChanged(QString)));

    mEnum = DeviceEnumerator::instance();
    connect(mEnum, SIGNAL(deviceConnected(QString)), SLOT(onDeviceConnected(QString)));
    connect(mEnum, SIGNAL(deviceRemoved(QString)), SLOT(onDeviceDisconnected(QString)));

    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(mPorts);
    setLayout(lay);
}

void SerialPortWidget::setParity(QString parity)
{
    if (parity.toLower() == "no" || parity.toLower() == "none")
        mCom->setParity(QSerialPort::NoParity);
    else if (parity.toLower() == "even")
        mCom->setParity(QSerialPort::EvenParity);
    else if (parity.toLower() == "odd")
        mCom->setParity(QSerialPort::OddParity);
    else if (parity.toLower() == "space")
        mCom->setParity(QSerialPort::SpaceParity);
    else if (parity.toLower() == "Mark")
        mCom->setParity(QSerialPort::MarkParity);
}

void SerialPortWidget::setStopBits(float stopbits)
{
    QSerialPort::StopBits bits = QSerialPort::UnknownStopBits;
    if (stopbits == 1.0)
        bits = QSerialPort::OneStop;
    else if (stopbits == 1.5)
        bits = QSerialPort::OneAndHalfStop;
    else if (stopbits == 2.0)
        bits = QSerialPort::TwoStop;
    mCom->setStopBits(bits);
}

void SerialPortWidget::autoConnect(QString serial)
{
    mAutoSerial = serial;
}

void SerialPortWidget::onDataReady()
{
    QByteArray ba = mCom->readAll();
    read(ba);
    emit dataReceived(ba);
}
//---------------------------------------------------------

void SerialPortWidget::onDeviceConnected(QString port)
{
    QString text = port;
    const QSerialPortInfo &info = mEnum->getInfo(port);
//    if (info.isBusy())
//        text += " (занят)";
//    if (!info.isValid())
//        text += " (ошибка)";
    mPorts->addItem(text);

    if (!mAutoSerial.isEmpty())
    {
        if (info.serialNumber().contains(mAutoSerial) && !info.isBusy() && !mCom->isOpen())
        {
            mPorts->setCurrentText(port);
            onPortChanged(port);
        }
    }
}

void SerialPortWidget::onDeviceDisconnected(QString port)
{
    int idx = mPorts->findText(port);
//    qDebug() << port << "removed";
    if (idx > 0)
    {
        if (mPorts->currentText() == port)
        {
//            qDebug() << port << "removed while in use!!";
            mPorts->setCurrentIndex(0);
            onPortChanged("");
        }
        mPorts->removeItem(idx);
    }
}
//---------------------------------------------------------

void SerialPortWidget::onPortChanged(QString portname)
{
    if (mCom->isOpen())
    {
        mCom->close();
//        qDebug() << mCom->portName() << "closed";
        emit disconnected();
    }

    if (!portname.isEmpty())
    {
        mCom->setPortName(portname);
//        qDebug() << "try open" << portname;
        if (mCom->open(QIODevice::ReadWrite))
            emit connected();
    }
}
//---------------------------------------------------------
