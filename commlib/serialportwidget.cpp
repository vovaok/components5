#include "serialportwidget.h"
SerialPortWidget::SerialPortWidget(QSerialPort *device, QWidget *parent) :
    QWidget(parent)
{
    if (device)
        mCom = device;
//    else
//        mCom = new QSerialPort();

//    mCom->moveToThread(&mThread);
//    mThread.start();

    if (mCom)
    {
        mCom->setBaudRate(62500);
        mCom->setParity(QSerialPort::EvenParity);
        mCom->setStopBits(QSerialPort::OneStop);
        mCom->setFlowControl(QSerialPort::NoFlowControl);
        connect(mCom, &QSerialPort::readyRead, this, &SerialPortWidget::onDataReady);
    }

    mPorts = new QComboBox();
    mPorts->setMinimumWidth(100);
    mPorts->addItem("- Выберите порт -");
    connect(mPorts, &QComboBox::textActivated, this, &SerialPortWidget::onPortChanged);
//    connect(mPorts, &QComboBox::textActivated, this, &SerialPortWidget::portChanged);

    mEnum = DeviceEnumerator::instance();
    connect(mEnum, &DeviceEnumerator::deviceConnected, this, &SerialPortWidget::onDeviceConnected);
    connect(mEnum, &DeviceEnumerator::deviceRemoved, this, &SerialPortWidget::onDeviceDisconnected);

    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(mPorts);
    setLayout(lay);
}

SerialPortWidget::~SerialPortWidget()
{
//    mThread.quit();
//    mThread.wait(1000);
}

void SerialPortWidget::setParity(QString parity)
{
    if (!mCom)
        return;

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
    if (!mCom)
        return;

    QSerialPort::StopBits bits = QSerialPort::UnknownStopBits;
    if (stopbits == 1.0)
        bits = QSerialPort::OneStop;
    else if (stopbits == 1.5)
        bits = QSerialPort::OneAndHalfStop;
    else if (stopbits == 2.0)
        bits = QSerialPort::TwoStop;
    mCom->setStopBits(bits);
}

void SerialPortWidget::disableAutoRead()
{
    if (mCom)
        disconnect(mCom, &QSerialPort::readyRead, this, &SerialPortWidget::onDataReady);
}

void SerialPortWidget::autoConnect(QString description)
{
    mAutoDesc = description;
}

void SerialPortWidget::onDataReady()
{
    if (mCom)
    {
        QByteArray ba = mCom->readAll();
        read(ba);
        emit dataReceived(ba);
    }
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

    if (!mAutoDesc.isEmpty())
    {
        if ((info.description().contains(mAutoDesc) || info.serialNumber().contains(mAutoDesc)) && !info.isBusy())
        {
            if (mCom && mCom->isOpen())
            {
                // do nothing
            }
            else
            {
                mPorts->setCurrentText(port);
                onPortChanged(port);
            }
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
    if (mCom)
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
            if (mCom->open(QIODevice::ReadWrite))
            {
                emit connected();
            }
        }
    }

    emit portChanged(portname);
}
//---------------------------------------------------------
