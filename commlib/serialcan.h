#ifndef SERIALCAN_H
#define SERIALCAN_H

#include "serialframe.h"

#pragma pack(push,1)
typedef struct
{
    uint32_t id;
    uint32_t lparam;
    uint32_t hparam;
    char length;
} CanMessage;

typedef struct
{
    uint32_t id;
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
    void sendMessage(uint32_t id, QByteArray &data);

    ProtocolVersion protocolVersion() const {return pProto;}
    void changeProtocolVersion(ProtocolVersion protocol);
    void clearBoardBuffer();

signals:
    void onMessage(uint32_t id, QByteArray &data);
    void onMessageSent(uint32_t id, QByteArray &data);
    void onMessageAccepted();

private slots:
    void onData(QByteArray &data);

public slots:

};

#endif // SerialCan_H
