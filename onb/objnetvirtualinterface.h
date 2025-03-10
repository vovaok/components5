#ifndef OBJNETVIRTUALINTERFACE_H
#define OBJNETVIRTUALINTERFACE_H

#include "objnetInterface.h"
#include "objnetvirtualserver.h"
#include <QTcpSocket>
#include <QHostAddress>

namespace Objnet
{

class ObjnetVirtualInterface : public QObject, public ObjnetInterface
{
    Q_OBJECT

private:
    QTcpSocket *mSocket;
    QString mNetname;
    QString mServerIp;
    typedef struct {uint32_t id, mask;} Filter;
    QVector<Filter> mFilters;
    QQueue<CommonMessage> mRxQueue;
    bool mActive;

    OviCodec mCodec;

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketRead();
    void msgReceived(const QByteArray &ba);

public:
    ObjnetVirtualInterface(QString netname, QString serverIp="127.0.0.1");

    virtual bool send(const CommonMessage &msg) override;
    void flush() override;

    int addFilter(uint32_t id, uint32_t mask=0xFFFFFFFF) override;
    void removeFilter(int number) override;

    bool isActive() const {return mActive;}

    QString netname() const {return mNetname;}

public slots:
    void setActive(bool enabled);
};

}

#endif // OBJNETVIRTUALINTERFACE_H
