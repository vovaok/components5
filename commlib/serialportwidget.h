#ifndef SERIALPORTWIDGET_H
#define SERIALPORTWIDGET_H

#include <QtWidgets>
#include <qserialport.h>
#include <qserialportinfo.h>
#include "deviceenumerator.h"
#include "uartinterface.h"
#include "DonglePort.h"

class SerialPortWidget : public QWidget, public UartInterface
{
    Q_OBJECT
public:
    explicit SerialPortWidget(DonglePort *port, QWidget *parent = nullptr);
    ~SerialPortWidget();

private:
    DonglePort *port = nullptr;
    QComboBox * portsCombo = nullptr;

public slots:
    void onPortChanged(QString portname);
    void onDeviceConnected(QString name);
    void onDeviceDisconnected(QString name);
};

#endif // SERIALPORTWIDGET_H
