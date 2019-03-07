#include "netconnector.h"

NetConnector* NetConnector::mSelf = nullptr;

NetConnector::NetConnector(QObject *parent) : QUdpSocket(parent)
{
    int port = 42042;
    bool bres = false;
    do
    {
        bres = bind(port++);
    }
    while (!bres);
    qDebug() << bres << (port-1);
    connect(this, SIGNAL(readyRead()), SLOT(datagramReady()));

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), SLOT(onTimer()));
    mTimer->setInterval(3000);
}

NetConnector *NetConnector::instance(QObject *parent)
{
    if (!mSelf)
        mSelf = new NetConnector(parent);
    return mSelf;
}

void NetConnector::declareServer(QString name, int port)
{
    mDeclaredServers[name] = port;
    StringMap params;
    params["server"] = name;
    params["address"] = localAddress().toString();
    params["port"] = QString::number(mDeclaredServers[name]);
    send("found", params);
}

void NetConnector::declareServer(QString name, QTcpServer *server)
{
    mDeclaredServers[name] = server->serverPort();
    StringMap params;
    params["server"] = name;
    params["address"] = server->serverAddress().toString();
    params["port"] = QString::number(mDeclaredServers[name]);
    send("found", params);
}

void NetConnector::removeServer(QString name)
{
    mDeclaredServers.remove(name);
}

void NetConnector::findServer(QString name)
{
    foreach (QString srv, mDeclaredServers.keys())
    {
        if (srv == name)
        {
            emit serverFound(name, "127.0.0.1", mDeclaredServers[srv]);
            return;
        }
    }

    StringMap params;
    params["server"] = name;
    send("search", params);
}

void NetConnector::onTimer()
{

}

void NetConnector::datagramReady()
{
    while (hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress address;
        quint16 port;
        datagram.resize(pendingDatagramSize());
        readDatagram(datagram.data(), datagram.size(), &address, &port);
        QString pkt = QString::fromUtf8(datagram);

        StringMap params;
        QString cmd = parse(pkt, params);

        if (cmd == "search")
        {
            QString name = params["server"];
            if (mDeclaredServers.contains(name))
            {
                params["address"] = localAddress().toString();
                params["port"] = QString::number(mDeclaredServers[name]);
                send("found", params, address, port);
            }
        }
        else if (cmd == "found")
        {
            emit serverFound(params["server"], address.toString(), params["port"].toInt());
        }
    }
}
//---------------------------------------------------------------------------

QString NetConnector::getTextBefore(QString pat, QString &txt)
{
    int p1 = txt.indexOf(pat);
    if (p1 == -1)
        return "";
    int p2 = txt.length() - p1 - pat.length();
    QString res = txt.left(p1);
    txt = txt.right(p2);
    return res;
}

QString NetConnector::parse(QString pkt, StringMap &data)
{
    QString cmd = getTextBefore(":", pkt);
    QString key, value;
    while (!pkt.isEmpty())
    {
        key = getTextBefore("=", pkt);
        key = key.trimmed();
        value = getTextBefore(";", pkt);
        value = value.trimmed();
        if (key.isEmpty())
            break;
        data[key] = value;
    }
    return cmd;
}

void NetConnector::send(QString cmd, const StringMap &params, QHostAddress receiver, int port)
{
    QString res = cmd + ":";
    foreach (QString key, params.keys())
        res += key + "=" + params[key] + ";";
//    res += "#end;";

    writeDatagram(res.toUtf8(), receiver, port);
}
//---------------------------------------------------------------------------
