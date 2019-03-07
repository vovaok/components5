#ifndef SIMPLESERVER_H
#define SIMPLESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "netconnector.h"
#include "commontypes.h"

class SimpleServer : public QObject
{
    Q_OBJECT

private:
    typedef QMap<QTcpSocket*, QString> SocketStringMap;

private:
    QTcpServer *mServer;
    quint16 mPort;
//    bool pconnected;
    QString mServiceName;
    SocketStringMap mBuffers;

    void parseCommand(QTcpSocket *socket, QString txt);

private slots:
    void onClientConnect();
    void onClientDisconnect();
    void onClientRead();

public:
    explicit SimpleServer(QString serviceName, QObject *parent = 0);
    ~SimpleServer();

    QTcpServer* server() {return mServer;}

    static QString getTextBefore(QString pat, QString &txt);

    quint16 port() {return mPort;}
    void setPort(quint16 port);

    bool active() {return mServer->isListening();}
    void setActive(bool active);
    bool connected() {return mBuffers.count();}
    int clientCount() {return mBuffers.count();}

    void sendCommandTo(QTcpSocket *socket, QString cmd, StringMap array);
    void sendCommandToAll(QString cmd, StringMap array);

signals:
    void clientConnect(QTcpSocket *socket);
    void clientDisconnect(QTcpSocket *socket);
    void clientReceiveCommand(QTcpSocket *socket, QString cmd, StringMap array);

public slots:

};
#endif // SIMPLESERVER_H
