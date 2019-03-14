#include "serialportwidget.h"

SerialPortWidget::SerialPortWidget(DonglePort *port, QWidget *parent) : QWidget(parent), port(port)
{
    portsCombo = new QComboBox();
    portsCombo->setMinimumWidth(100);
    portsCombo->addItem("- Выберите порт -");
    connect(portsCombo, SIGNAL(activated(QString)), SLOT(onPortChanged(QString)));

    connect(DeviceEnumerator::instance(), SIGNAL(deviceConnected(QString)), this, SLOT(onDeviceConnected(QString)));
    connect(DeviceEnumerator::instance(), SIGNAL(deviceRemoved(QString)), this, SLOT(onDeviceDisconnected(QString)));

    auto layout = new QHBoxLayout;
    layout->addWidget(portsCombo);
    setLayout(layout);
}

SerialPortWidget::~SerialPortWidget()
{
    disconnect(DeviceEnumerator::instance(), SIGNAL(deviceConnected(QString)), this, SLOT(onDeviceConnected(QString)));
    disconnect(DeviceEnumerator::instance(), SIGNAL(deviceRemoved(QString)), this, SLOT(onDeviceDisconnected(QString)));
}

void SerialPortWidget::onPortChanged(QString portname)
{
    port->changePort(portname);
}

void SerialPortWidget::onDeviceConnected(QString name)
{
    const QSerialPortInfo &info = DeviceEnumerator::instance()->getInfo(name);
    portsCombo->addItem(name);

    if (!info.isBusy() && !port->isOpen())
    {
        portsCombo->setCurrentText(name);
        onPortChanged(name);
    }
}

void SerialPortWidget::onDeviceDisconnected(QString name)
{
    portsCombo->removeItem(portsCombo->findText(name));

    if (portsCombo->currentText() == name)
    {
        portsCombo->setCurrentIndex(0);
        onPortChanged("");
    }
}
