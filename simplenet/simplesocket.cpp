#include "simplesocket.h"

SimpleSocket::SimpleSocket(QObject *parent) :
    QTcpSocket(parent)
{
    setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(this, SIGNAL(readyRead()), this, SLOT(receiveData()));

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), SLOT(attemptToConnect()));
    mTimer->setInterval(5000);

    connect(this, SIGNAL(connected()), mTimer, SLOT(stop()));
    connect(this, SIGNAL(disconnected()), mTimer, SLOT(start()));

    mNetConn = NetConnector::instance(this);
}
//---------------------------------------------------------------------------

void SimpleSocket::autoConnect(QString address, quint16 port)
{
    mAutoAddress = address;
    mAutoPort = port;
    mTimer->start();
    attemptToConnect();
}

void SimpleSocket::autoConnect(QString serviceName)
{
    mAutoService = serviceName;
    connect(mNetConn, SIGNAL(serverFound(QString,QString,int)), SLOT(onServerFound(QString,QString,int)));
    mNetConn->findServer(mAutoService);
}

void SimpleSocket::onServerFound(QString name, QString address, int port)
{
    if (name == mAutoService)
        autoConnect(address, port);
}

void SimpleSocket::attemptToConnect()
{
    SocketState st = state();
    if (st == UnconnectedState)
    {
        connectToHost(mAutoAddress, mAutoPort);
        mTimer->setInterval(1000);
    }
    else if (st == ConnectingState)
    {
        abort();
        mTimer->setInterval(5000);
    }
}
//---------------------------------------------------------------------------

void SimpleSocket::receiveData()
{
    mBuffer += readAll();
    QString cmd;
    do
    {
        cmd = getTextBefore("#end;", mBuffer);
        if (!cmd.isEmpty())
            parseCommand(cmd);
    }
    while (!cmd.isEmpty());
}
//---------------------------------------------------------------------------

void SimpleSocket::parseCommand(QString txt)
{
    QString cmd = getTextBefore(":", txt);
    QString key, value;
    StringMap arr;
    while (!txt.isEmpty())
    {
        key = getTextBefore("=", txt);
        key = key.trimmed();
        value = getTextBefore(";", txt);
        value = value.trimmed();
        if (key.isEmpty())
            break;
        arr[key] = value;
    }
    emit receive(cmd, arr);
}

void SimpleSocket::send(QString cmd, const StringMap &params)
{
    QString res = cmd + ":";
    foreach (QString key, params.keys())
        res += key + "=" + params[key] + ";";
    res += "#end;";
    sendPlain(res);
}

void SimpleSocket::sendPlain(QString message)
{
    write(message.toUtf8());
    flush();
}
//---------------------------------------------------------------------------

QString SimpleSocket::getTextBefore(QString pat, QString &txt)
{
    int p1 = txt.indexOf(pat);
    if (p1 == -1)
        return "";
    int p2 = txt.length() - p1 - pat.length();
    QString res = txt.left(p1);
    txt = txt.right(p2);
    return res;
}
//---------------------------------------------------------------------------
