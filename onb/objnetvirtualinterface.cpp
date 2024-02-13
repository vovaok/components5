#include "objnetvirtualinterface.h"

using namespace Objnet;

ObjnetVirtualInterface::ObjnetVirtualInterface(QString netname, QString serverIp) :
    mNetname(netname),
    mServerIp(serverIp),
    mActive(false)
{
    mMaxFrameSize = 64;
    mSocket = new QTcpSocket(this);
    connect(mSocket, SIGNAL(connected()), SLOT(onSocketConnected()));
    connect(mSocket, SIGNAL(disconnected()), SLOT(onSocketDisconnected()));
    connect(mSocket, SIGNAL(readyRead()), SLOT(onSocketRead()));
    mSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    //mSocket->connectToHost(QHostAddress::LocalHost, 51966);
}
//---------------------------------------------------------

bool ObjnetVirtualInterface::send(const CommonMessage &msg)
{
    QByteArray ba;
    uint32_t id = msg.rawId();
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());
    if (mSocket->isOpen())
    {
        QByteArray ba2 = mCodec.encode(ba);
        mSocket->write(ba2);
    }
    return true;
}

void ObjnetVirtualInterface::flush()
{
    ObjnetInterface::flush();
    mSocket->flush();
}

int ObjnetVirtualInterface::addFilter(uint32_t id, uint32_t mask)
{
    Filter f;
    f.id = id & 0x1FFFFFFF;
    f.mask = mask & 0x1FFFFFFF;
    mFilters << f;
    return mFilters.size() - 1;
}

void ObjnetVirtualInterface::removeFilter(int number)
{
    if (number>=0 && number < mFilters.size())
        mFilters.remove(number);
}

void ObjnetVirtualInterface::setActive(bool enabled)
{
    if (mActive != enabled)
    {
        mActive = enabled;
        if (mActive)
        {
//            mSocket->connectToHost(QHostAddress::LocalHost, 51966);
            mSocket->connectToHost(QHostAddress(mServerIp), 51966);
        }
        else
            mSocket->disconnectFromHost();
    }
}
//---------------------------------------------------------

void ObjnetVirtualInterface::onSocketConnected()
{
    QByteArray ba;
    uint32_t id = 0xFF000000;
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(mNetname.toLatin1());
    ba = mCodec.encode(ba);
//    qDebug() << ba;
    mSocket->write(ba);
}

void ObjnetVirtualInterface::onSocketDisconnected()
{
    mRxQueue.clear();
}

void ObjnetVirtualInterface::onSocketRead()
{
    QByteArray in = mSocket->readAll();
    QByteArray ba;
    while (!(ba=mCodec.decode(in)).isEmpty())
        msgReceived(ba);
}

void ObjnetVirtualInterface::msgReceived(const QByteArray &ba)
{
    uint32_t id = *reinterpret_cast<const uint32_t*>(ba.data());

    bool accept = false;
    if (!mFilters.size())
        accept = true;
    foreach (Filter f, mFilters)
    {
        if ((f.id & f.mask) == (id & f.mask))
        {
            accept = true;
            break;
        }
    }
    //accept=true;

    if (accept)
    {
        CommonMessage msg;
        msg.setId(id);
        msg.setData(QByteArray(ba.data()+4, ba.size()-4));
        receive(std::move(msg));
    }
}
