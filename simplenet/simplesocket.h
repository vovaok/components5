#ifndef SIMSOCK_H
#define SIMSOCK_H

#include <QTcpSocket>
#include <QMap>
#include <QTimer>
#include "netconnector.h"
#include "commontypes.h"

class SimpleSocket : public QTcpSocket
{
    Q_OBJECT

private:
    QString mAutoService;
    QString mAutoAddress;
    quint16 mAutoPort;
    QString mBuffer;
    QTimer *mTimer;
    NetConnector *mNetConn;

    void parseCommand(QString txt);

public:
    explicit SimpleSocket(QObject *parent = 0);

    static QString getTextBefore(QString pat, QString &txt);
    void setAutoAddress(QString address) {mAutoAddress = address;}
    bool isConnected() const {return state() == ConnectedState;}
    
signals:
    void receive(QString cmd, StringMap params);
    
private slots:
    void receiveData();
    void attemptToConnect();
    void onServerFound(QString name, QString address, int port); // for NetConnector

public slots:
    void send(QString cmd, const StringMap &params = StringMap());
    void sendPlain(QString message);
    void autoConnect(QString address, quint16 port);
    void autoConnect(QString serviceName);
};

#endif // SIMSOCK_H
