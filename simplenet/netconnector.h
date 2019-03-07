#ifndef NETCONNECTOR_H
#define NETCONNECTOR_H

#include <QUdpSocket>
#include <QTimer>
#include <QTcpServer>
#include "commontypes.h"

class NetConnector : public QUdpSocket
{
    Q_OBJECT

private:
    QTimer *mTimer;
    QHash<QString, int> mDeclaredServers;
    QString getTextBefore(QString pat, QString &txt);
    QString parse(QString pkt, StringMap &data);
    void send(QString cmd, const StringMap &params, QHostAddress receiver=QHostAddress::Broadcast, int port=42042);

    static NetConnector *mSelf;


public:
    explicit NetConnector(QObject *parent = 0);
    static NetConnector *instance(QObject *parent = 0);
    void declareServer(QString name, int port);
    void declareServer(QString name, QTcpServer *server);
    void removeServer(QString name);
    void findServer(QString name);


signals:
    void serverFound(QString name, QString address, int port);

private slots:
    void onTimer();
    void datagramReady();

public slots:

};

#endif // NETCONNECTOR_H
