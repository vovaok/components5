#ifndef SERIALCAN_H
#define SERIALCAN_H

#include "serialframe.h"

#pragma pack(push,1)
typedef struct
{
    unsigned long id;
    unsigned long lparam;
    unsigned long hparam;
    char length;
} CanMessage;

typedef struct
{
    unsigned long id;
} CanMessageHeader;

typedef enum
{
    canGatewayState,
    canChangeProto,
    canPollDevices,
    canClearBuffer
} CanGatewayCmd;

typedef struct
{
    unsigned char cmd: 7;
    unsigned char response: 1;
    unsigned char param1;
    unsigned char param2;
} CanGatewaySvcMsg;
#pragma pack(pop)
//---------------------------------------------------------------------------

class SerialCan : public SerialFrame
{
    Q_OBJECT
public:
    typedef enum {protoCommon, protoExtended} ProtocolVersion;

private:
    ProtocolVersion pProto;
    void setProtocolVersion(ProtocolVersion protocol) {pProto = protocol;}

public:
    explicit SerialCan(QIODevice *parent);
    explicit SerialCan(QIODevice *parent, ProtocolVersion proto = protoCommon);
    void sendMessage(unsigned long id, QByteArray &data);

    ProtocolVersion protocolVersion() const {return pProto;}
    void changeProtocolVersion(ProtocolVersion protocol);
    void clearBoardBuffer();

signals:
    void onMessage(unsigned long id, QByteArray &data);
    void onMessageSent(unsigned long id, QByteArray &data);
    void onMessageAccepted();

private slots:
    void onData(QByteArray &data);

public slots:

};

#endif // SerialCan_H
