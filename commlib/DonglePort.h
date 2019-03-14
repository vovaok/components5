#ifndef DONGLEPORT_H
#define DONGLEPORT_H

#include <QObject>
#include <QSerialPort>

class DonglePort : public QSerialPort
{
    Q_OBJECT
public:
    explicit DonglePort(QObject *parent = nullptr);
    ~DonglePort();

    void changePort(QString portName);
    void autoConnectTo(QString value);
    void disableAutoRead();

signals:
    void dataReceived(const QByteArray &byteArray);
    void connected();
    void disconnected();

private slots:
    void onDataReady();
    void onDeviceConnected(QString port);
    void onDeviceDisconnected(QString port);


private:
    QString serial;
};

#endif // DONGLEPORT_H
