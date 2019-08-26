#include "serialcan.h"

SerialCan::SerialCan(QIODevice *parent, ProtocolVersion proto) :
    SerialFrame(parent)
{
    pProto = proto;
    connect(this, SIGNAL(dataReceived(QByteArray&)), this, SLOT(onData(QByteArray&)));
}

SerialCan::SerialCan(QIODevice *parent) :
    SerialFrame(parent)
{
    pProto = protoCommon;
    connect(this, SIGNAL(dataReceived(QByteArray&)), this, SLOT(onData(QByteArray&)));
}

void SerialCan::onData(QByteArray &data)
{
    if (data.size() < 4) // service commands
    {
        if (data.isEmpty())
        {
            emit onMessageAccepted();
            return;
        }
        CanGatewaySvcMsg *svc = reinterpret_cast<CanGatewaySvcMsg*>(data.data());
        if (svc->response == 1)
        {
            if (svc->cmd == canChangeProto)
            {
                setProtocolVersion((ProtocolVersion)svc->param1);
            }
        }
        return;
    }

    if (pProto == protoCommon)
    {
        QByteArray msgData;
        CanMessage *msg = reinterpret_cast<CanMessage*>(data.data());
        msgData.append(reinterpret_cast<const char*>(&msg->lparam), msg->length);
        emit onMessage(msg->id, msgData);
    }
    else if (pProto == protoExtended)
    {
        QByteArray msgData = data;
        CanMessageHeader *hdr = reinterpret_cast<CanMessageHeader*>(data.data());
        msgData.remove(0, sizeof(CanMessageHeader));
        emit onMessage(hdr->id, msgData);
    }
}

void SerialCan::sendMessage(uint32_t id, QByteArray &data)
{
    if (pProto == protoCommon)
    {
        CanMessage msg;
        msg.id = id;
        msg.length = data.size();
        QByteArray dadata = data;
        dadata.resize(8);
        uint32_t *params = reinterpret_cast<uint32_t*>(dadata.data());
        msg.lparam = params[0];
        msg.hparam = params[1];
        QByteArray ba;
        ba.append(reinterpret_cast<const char*>(&msg), sizeof(CanMessage));
        sendData(ba);
    }
    else
    {
        QByteArray msgData = data;
        CanMessageHeader hdr;
        hdr.id = id;
        msgData.prepend(reinterpret_cast<const char*>(&hdr), sizeof(CanMessageHeader));
        sendData(msgData);
    }
    emit onMessageSent(id, data);
}
//---------------------------------------------------------------------------

void SerialCan::changeProtocolVersion(ProtocolVersion protocol)
{
    CanGatewaySvcMsg svc;
    svc.cmd = canChangeProto;
    svc.response = 0;
    svc.param1 = protocol;
    QByteArray msgData;
    msgData.append(reinterpret_cast<const char*>(&svc), sizeof(CanGatewaySvcMsg));
    sendData(msgData);
}
//---------------------------------------------------------------------------

void SerialCan::clearBoardBuffer()
{
    CanGatewaySvcMsg svc;
    svc.cmd = canClearBuffer;
    svc.response = 0;
    QByteArray msgData;
    msgData.append(reinterpret_cast<const char*>(&svc), sizeof(CanGatewaySvcMsg));
    sendData(msgData);
}
//---------------------------------------------------------------------------
