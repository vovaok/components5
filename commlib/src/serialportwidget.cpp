#include "serialportwidget.h"

SerialPortWidget::SerialPortWidget(QWidget *parent) :
    QWidget(parent),
    mAutoIdx(0)
{
    mCom = new QSerialPort();
    mCom->setBaudRate(62500);
    mCom->setParity(QSerialPort::EvenParity);
    mCom->setStopBits(QSerialPort::OneStop);
    mCom->setFlowControl(QSerialPort::NoFlowControl);
    connect(mCom, SIGNAL(readyRead()), SLOT(onDataReady()));

    mPorts = new QComboBox();
    mPorts->setMinimumWidth(100);
    pollPorts();
    connect(mPorts, SIGNAL(activated(QString)), SLOT(onPortChanged(QString)));

    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(mPorts);
    setLayout(lay);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(onTimer()));
    timer->start(100);
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

void SerialPortWidget::autoConnect(QString description)
{
    mAutoDesc = description;
    pollPorts();
}

void SerialPortWidget::onTimer()
{
    if (!mCom->isOpen() && mAutoIdx > 0)
    {
        mPorts->setCurrentIndex(mAutoIdx);
        onPortChanged(mPorts->itemText(mAutoIdx));
    }
}

void SerialPortWidget::onDataReady()
{
    QByteArray ba = mCom->readAll();
    read(ba);
}

void SerialPortWidget::pollPorts()
{
    mPorts->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    mPorts->addItem("- Выберите порт -");
    mAutoIdx = 0;
    foreach (QSerialPortInfo port, ports)
    {
        QString s;
        if (port.isBusy())
            s += "busy ";
        if (port.isNull())
            s += "null ";
        if (!port.isValid())
            s += "invalid ";
        QString description = port.description();
        qDebug() << port.portName() << ":" << s << "//" << description;

        mPorts->addItem(port.portName());
        if (!mAutoDesc.isEmpty())
        {
            if (description.contains(mAutoDesc))
                mAutoIdx = mPorts->count() - 1;
        }
    }
}

void SerialPortWidget::onPortChanged(QString portname)
{
    if (portname == mPorts->itemText(0))
    {
        mCom->close();
        disconnectEvent();
        emit disconnected();
    }
    else
    {
        mCom->setPortName(portname);
        if (mCom->open(QIODevice::ReadWrite))
            emit connected();
        connectEvent();
    }
}

