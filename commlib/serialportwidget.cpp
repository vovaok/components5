#include "serialportwidget.h"

SerialPortWidget::SerialPortWidget(DonglePort *port, QWidget *parent) : QWidget(parent), port(port)
{
    portsCombo = new QComboBox();
    portsCombo->setMinimumWidth(100);
    portsCombo->addItem("- Выберите порт -");
    connect(portsCombo, SIGNAL(activated(int)), SLOT(onPortChanged(int)));

    connect(DeviceEnumerator::instance(), SIGNAL(deviceConnected(QString)), this, SLOT(onDeviceConnected(QString)));
    connect(DeviceEnumerator::instance(), SIGNAL(deviceRemoved(QString)), this, SLOT(onDeviceDisconnected(QString)));

    connect(port, SIGNAL(connected()), SLOT(onPortConnected()));
    connect(port, SIGNAL(disconnected()), SLOT(onPortDisconnected()));

    auto layout = new QHBoxLayout;
    layout->addWidget(portsCombo);
    setLayout(layout);
}

SerialPortWidget::~SerialPortWidget()
{
    disconnect(DeviceEnumerator::instance(), SIGNAL(deviceConnected(QString)), this, SLOT(onDeviceConnected(QString)));
    disconnect(DeviceEnumerator::instance(), SIGNAL(deviceRemoved(QString)), this, SLOT(onDeviceDisconnected(QString)));
}

void SerialPortWidget::addPort(QString name)
{
    if (portsCombo->findData(port->portName()) != -1)
        return;
    const QSerialPortInfo &info = DeviceEnumerator::instance()->getInfo(name);
    QString text = name;
    if (!info.serialNumber().isEmpty())
        text += " [" + info.serialNumber() + "]";
    portsCombo->addItem(text, name);
}

void SerialPortWidget::onPortChanged(int index)
{
    QString portname = portsCombo->itemData(index).toString();
    port->changePort(index? portname: "");
}

void SerialPortWidget::onPortConnected()
{
    addPort(port->portName());
    int index = portsCombo->findData(port->portName());
    portsCombo->setCurrentIndex(index);
}

void SerialPortWidget::onPortDisconnected()
{
    portsCombo->setCurrentIndex(0);
}

void SerialPortWidget::onDeviceConnected(QString name)
{
    addPort(name);
}

void SerialPortWidget::onDeviceDisconnected(QString name)
{
    int index = portsCombo->findData(name);
    portsCombo->removeItem(index);
}
