#include "simpleserver.h"

SimpleServer::SimpleServer(QString serviceName, QObject *parent) :
    QObject(parent),
    mPort(0),
    mServiceName(serviceName)
{
    mServer = new QTcpServer(this);
    connect(mServer, SIGNAL(newConnection()), this, SLOT(onClientConnect()));
}

SimpleServer::~SimpleServer()
{
    setActive(false);
//    delete srv; v teorii ne nado
}

void SimpleServer::onClientConnect()
{
    QTcpSocket *socket = mServer->nextPendingConnection();
    mBuffers[socket] = "";
    connect(socket, SIGNAL(disconnected()), SLOT(onClientDisconnect()));
    connect(socket, SIGNAL(readyRead()), SLOT(onClientRead()));
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    emit clientConnect(socket);
}

void SimpleServer::onClientDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;
    mBuffers.remove(socket);
    emit clientDisconnect(socket);
    socket->deleteLater();
}

void SimpleServer::onClientRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    QString data = socket->readAll();
    mBuffers[socket] += data;

    QString cmd;
    do
    {
        cmd = getTextBefore("#end;", mBuffers[socket]);
        if (!cmd.isEmpty())
            parseCommand(socket, cmd);
    }
    while (!cmd.isEmpty());
}

void SimpleServer::parseCommand(QTcpSocket *socket, QString txt)
{
    QString cmd = getTextBefore(":", txt);
    QString key, value;
    StringMap arr;
    while (!txt.isEmpty())
    {
        key = getTextBefore("=", txt);
        key = key.trimmed();
        value = getTextBefore(";", txt);
        value = value.trimmed();//QString::fromUtf8(value.trimmed().toLatin1());
        if (key.isEmpty())
            break;
        arr[key] = value;
    }
    emit clientReceiveCommand(socket, cmd, arr);
}

void SimpleServer::sendCommandTo(QTcpSocket *socket, QString cmd, StringMap array)
{
    QString res = cmd + ":";
    foreach (QString key, array.keys())
        res += key + "=" + array[key] + ";";
    res += "#end;";
    socket->write(res.toUtf8());
    socket->flush();
}

void SimpleServer::sendCommandToAll(QString cmd, StringMap array)
{
    if (!mBuffers.empty())
    foreach(QTcpSocket* sock, mBuffers.keys())
    {
        sendCommandTo(sock, cmd, array);
    }
}

void SimpleServer::setActive(bool active)
{
    if (active && !this->active())
    {
        mServer->listen(QHostAddress::Any, mPort);
        if (!mPort)
            mPort = mServer->serverPort();
        if (!mServiceName.isEmpty())
            NetConnector::instance(this)->declareServer(mServiceName, mPort);
    }
    else if (!active && this->active())
    {
        if (!mServiceName.isEmpty())
            NetConnector::instance(this)->removeServer(mServiceName);
        mServer->close();
        mBuffers.clear();
    }
}

void SimpleServer::setPort(quint16 port)
{
    mPort = port;
    if (active())
    {
        setActive(false);
        setActive(true);
        if (!mServiceName.isEmpty())
        {
            NetConnector::instance(this)->removeServer(mServiceName);
            NetConnector::instance(this)->declareServer(mServiceName, mPort);
        }
    }
}

QString SimpleServer::getTextBefore(QString pat, QString &txt)
{
    int p1 = txt.indexOf(pat);
    if (p1 == -1)
        return "";
    int p2 = txt.length() - p1 - pat.length();
    QString res = txt.left(p1);
    txt = txt.right(p2);
    return res;
}
