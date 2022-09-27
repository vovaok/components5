#include "seriallink.h"

SerialLink::SerialLink(QIODevice *device) :
    SerialFrame(device)
{
    ResponseTimeout = 100;
    timeoutTimer = new QTimer(this);
    connect(this, SIGNAL(dataReceived(QByteArray&)), this, SLOT(onData(QByteArray&)));
    connect(this, SIGNAL(connected()), this, SLOT(connectionHandler()));
    connect(this, SIGNAL(disconnected()), this, SLOT(connectionHandler()));
    connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    clearData();
}
//----------------------------------------------------------

void SerialLink::connectionHandler()
{
    clearData();
    if (device()->isOpen())
        requestUartLinkVersion();
}
//----------------------------------------------------------

void SerialLink::clearData()
{
    timeoutTimer->stop();
    packetSent = false;
    Ready = false;

    Params.clear();
    ParamMap.clear();
    Funcs.clear();
    FuncMap.clear();
    ParamCount = 0;
    FuncCount = 0;

    FEchoTestPassed = false;
    FProductID = 0x00000000;
    FProductName = "";
    FProductVersion = 0x0000;
    FCompilationDate = "";
    FCompilationTime = "";

    OutQueue.clear();
}

void SerialLink::onData(QByteArray &data)
{
    packetSent = false;
    timeoutTimer->stop();
    if (!OutQueue.isEmpty())
        OutQueue.removeFirst();

    SUartFrame *frm = reinterpret_cast<SUartFrame*>(data.data());
    int sz = data.size() - sizeof(SUartHeader);
    if (frm->hdr.response)
    {
        switch (frm->hdr.cmd)
        {
          // service commands -----------------------------------------------
          case cmdEcho:
            FEchoTestPassed = true;
            emit uartEvent(cleEchoReceived);
            break;

          case cmdInfo:
            if (sz)
            {
                unsigned char *newdata = frm->data + 1;
                switch (frm->data[0])
                {
                  case icmdProductID:
                    FProductID = *reinterpret_cast<uint32_t*>(newdata);
                    break;

                  case icmdProductName:
                    FProductName = "";
                    for (int i=0; i<sz; i++)
                      FProductName += reinterpret_cast<char*>(newdata)[i];
                    break;

                  case icmdProductVersion:
                    FProductVersion = *reinterpret_cast<unsigned short*>(newdata);
                    break;

                  case icmdCompilationDate:
                    FCompilationDate = "";
                    for (int i=0; i<sz; i++)
                      FCompilationDate += reinterpret_cast<char*>(newdata)[i];
                    break;

                  case icmdCompilationTime:
                    FCompilationTime = "";
                    for (int i=0; i<sz; i++)
                      FCompilationTime += reinterpret_cast<char*>(newdata)[i];
                    break;

                  case icmdResources:
                    ParamCount = newdata[0];
                    FuncCount = newdata[1];
                    emit uartEvent(cleProductInfoAvailable);
                    if (ParamCount)
                        requestParamData();
                    if (FuncCount)
                        requestFuncData();
                    break;
                }
            }
            break;

          case cmdUartLinkVersion:
            UartLinkVersion = reinterpret_cast<unsigned short*>(frm->data)[0];
            if (frm->hdr.error) // version < 1.1
                requestProductInfoOld();
            else if (UartLinkVersion >= 0x0101) // version >= 1.1
                requestProductInfo();
            else
                requestProductInfoOld();
            break;

          // parameter commands ---------------------------------------------
          case cmdGetParamInfo:
            if (frm->hdr.error)
            {
//                cstate = "param index out of range";
//                SUartParamCmd *cmd = reinterpret_cast<SUartParamCmd*>(frm->data);
//                FParamCount = cmd->idx;
            }
            else
            {
                SUartParamCmd *cmd = reinterpret_cast<SUartParamCmd*>(frm->data);
//                cstate = "param info "+String(cmd->idx)+" accepted";
                SUartParam *par = reinterpret_cast<SUartParam*>(cmd->value);
                Params[cmd->idx] = *par;
                ParamMap[QString(QByteArray(par->name, 8))] = cmd->idx;
                if (cmd->idx == ParamCount-1)
                {
                    emit uartEvent(cleParamInfoAvailable);
                    if (!FuncCount)
                    {
                        Ready = true;
                        emit ready();
                    }
                }
            }
            break;

          case cmdGetParam:
            if (frm->hdr.error)
            {
                SUartParamCmd *cmd = reinterpret_cast<SUartParamCmd*>(frm->data);
                QString param;
                param.sprintf("(%d)", cmd->idx);
                if (cmd->idx < paramCount())
                    param = "'" + QString(QByteArray(Params[cmd->idx].name, 8)) + "'";
                QString err = "An error has occured when retrieving the parameter ";
                err += param + " value";
                emit error(clErrGetParam, err);
            }
            else
            {
                SUartParamCmd *cmd = reinterpret_cast<SUartParamCmd*>(frm->data);
                QString param;
                param.sprintf("%d", cmd->idx);
                if (cmd->idx < paramCount())
                    param = QString(QByteArray(Params[cmd->idx].name, 8));
                emit paramAccepted(param, cmd->value);
            }
            break;

          case cmdSetParam:
            if (frm->hdr.error)
            {
                SUartParamCmd *cmd = reinterpret_cast<SUartParamCmd*>(frm->data);
                QString param;
                param.sprintf("(%d)", cmd->idx);
                if (cmd->idx < paramCount())
                    param = "'" + QString(QByteArray(Params[cmd->idx].name, 8)) + "'";
                QString err = "An error has occured when setting the parameter ";
                err += param + " value";
                emit error(clErrSetParam, err);
            }
            else
            {
                emit uartEvent(cleSetParam);
            }
            break;

          // remote function call commands ----------------------------------
          case cmdGetFuncInfo:
            if (frm->hdr.error)
            {
//                cstate = "func index out of range";
//                SUartFuncCmd *cmd = reinterpret_cast<SUartFuncCmd*>(frm->data);
//                FFuncCount = cmd->idx;
            }
            else
            {
                SUartFuncCmd *cmd = reinterpret_cast<SUartFuncCmd*>(frm->data);
                SUartFunc *fn = reinterpret_cast<SUartFunc*>(cmd->params);
                Funcs[cmd->idx] = *fn;
                FuncMap[QString(QByteArray(fn->name, 8))] = cmd->idx;
                if (cmd->idx == FuncCount-1)
                {
                    emit uartEvent(cleFuncInfoAvailable);
                    Ready = true;
                    emit ready();
                }
            }
            break;

          case cmdCallFunc:
            if (frm->hdr.error)
            {
                SUartFuncCmd *cmd = reinterpret_cast<SUartFuncCmd*>(frm->data);
                QString func;
                func.sprintf("(%d)", cmd->idx);
                if (cmd->idx < funcCount())
                    func = "'" + QString(QByteArray(Funcs[cmd->idx].name, 8)) + "'";
                QString err = "An error has occured when calling the function ";
                err += func;
                emit error(clErrSetParam, err);
            }
            else
            {
                SUartFuncCmd *cmd = reinterpret_cast<SUartFuncCmd*>(frm->data);
                QString func;
                func.sprintf("%d", cmd->idx);
                if (cmd->idx < funcCount())
                    func = QString(QByteArray(Funcs[cmd->idx].name, 8));
                emit funcReturn(func, cmd->params);
            }
            break;

          default:
            emit error(clErrUnknownCmd, "Unknown command sent");
        }
    }
    if (!packetSent)
        popQueue();
}

void SerialLink::sendPacket(EUartCommand command, void *data, int size)
{
    SUartPacket pkt;
    pkt.frm.hdr.cmd = command;
    pkt.frm.hdr.response = false;
    pkt.frm.hdr.error = false;
    pkt.frm.hdr.fragmented = false;
    pkt.frm.hdr.eof = true;
    pkt.frm.hdr.addr = 0;       // need implementation
    pkt.frm.hdr.frag_no = 0;    // need implementation
    for (int i=0; i<size; i++)
        pkt.frm.data[i] = reinterpret_cast<unsigned char*>(data)[i];
    pkt.size = sizeof(SUartHeader) + size;
    QByteArray ba(reinterpret_cast<const char*>(&pkt.frm), pkt.size);
//    sendData(ba);
    if (OutQueue.count() > 50)
    {
        emit error(clErrQueueOverflow, "Output queue overflow");
    }
    else
    {
        OutQueue << ba;
        if (!packetSent)
            popQueue();
    }
}

void SerialLink::popQueue()
{
    if (!OutQueue.isEmpty())
    {
        QByteArray ba = OutQueue.first();
//        QString s;
//        for (int i=0; i<ba.size(); i++)
//            s += QString().sprintf("%02x ", (unsigned char)ba[i]);
//        emit statusChanged(s);
        sendData(ba);
        packetSent = true;
        timeoutTimer->start(ResponseTimeout);
    }
}

void SerialLink::timeout()
{
    timeoutTimer->stop();
    packetSent = false;
    emit error(clErrTimeout, "There is no response received.. it's so bad((");
    popQueue();
}
//----------------------------------------------------------

// service methods
void SerialLink::echoTest()
{
    FEchoTestPassed = false;
    sendPacket(cmdEcho);
}

void SerialLink::requestUartLinkVersion()
{
    emit statusChanged("request UartLink version");
    sendPacket(cmdUartLinkVersion);
}

void SerialLink::requestInfo(EUartInfoCmd icmd)
{
    emit statusChanged("    requesting info #"+QString::number(icmd));
    EUartInfoCmd cmd = icmd;
    sendPacket(cmdInfo, &cmd, 1);
}
//----------------------------------------------------------

void SerialLink::requestProductInfo()
{
    emit statusChanged("requesting board info..");
    requestInfo(icmdProductID);
    requestInfo(icmdProductName);
    requestInfo(icmdProductVersion);
    requestInfo(icmdCompilationDate);
    requestInfo(icmdCompilationTime);
    requestInfo(icmdResources);
}

void SerialLink::requestProductInfoOld()
{
    emit error(clErrBadVersion, "Board uses old version of UartLink");
}

void SerialLink::requestParamData()
{
    emit statusChanged("requesting parameter data...");
    Params.resize(ParamCount);
    for (int i=0; i<ParamCount; i++)
        requestParameterInfo(i);
}

void SerialLink::requestFuncData()
{
    Funcs.resize(FuncCount);
    emit statusChanged("requesting function data...");
    for (int i=0; i<FuncCount; i++)
        requestFunctionInfo(i);
}
//---------------------------------------------------------------------------

void SerialLink::requestParameterInfo(unsigned char id)
{
    SUartParamCmd cmd;
    cmd.idx = id;
    sendPacket(cmdGetParamInfo, &cmd, sizeof(unsigned char));
}

void SerialLink::requestParameter(unsigned char id)
{
    SUartParamCmd cmd;
    cmd.idx = id;
    sendPacket(cmdGetParam, &cmd, sizeof(unsigned char));
}

void SerialLink::setParameter(unsigned char id, void *data, int size)
{
    SUartParamCmd cmd;
    cmd.idx = id;
    for (int i=0; i<size; i++)
            cmd.value[i] = reinterpret_cast<unsigned char*>(data)[i];
    sendPacket(cmdSetParam, &cmd, sizeof(unsigned char) + size);
}
//---------------------------------------------------------------------------

void SerialLink::requestFunctionInfo(unsigned char id)
{
    SUartFuncCmd cmd;
    cmd.idx = id;
    sendPacket(cmdGetFuncInfo, &cmd, sizeof(unsigned char));
}

void SerialLink::callFunction(unsigned char id, void *params, int size)
{
    SUartFuncCmd cmd;
    cmd.idx = id;
    for (int i=0; i<size; i++)
        cmd.params[i] = reinterpret_cast<unsigned char*>(params)[i];
    sendPacket(cmdCallFunc, &cmd, sizeof(unsigned char) + size);
}
//---------------------------------------------------------------------------

void SerialLink::requestParameter(QString name)
{
    if (!isReady())
        return;
    int idx = ParamMap.value(name, -1);
    if (idx >= 0) // parameter exists
        requestParameter(idx);
    else
        emit error(clErrBadParameter, "Parameter with name '"+name+"' is not existing");
}

void SerialLink::setParameter(QString name, void *params)
{
    if (!isReady())
        return;
    int idx = ParamMap.value(name, -1);
    if (idx >= 0) // parameter exists
        setParameter(idx, params, Params[idx].size);
    else
        emit error(clErrBadParameter, "Parameter with name '"+name+"' is not existing");
}
//---------------------------------------------------------------------------

void SerialLink::callFunction(QString name, void *params)//, int Size)
{
    if (!isReady())
        return;
    int idx = FuncMap.value(name, -1);
    if (idx >= 0) // function exists
        callFunction(idx, params, Funcs[idx].sizes.params_size);
    else
        emit error(clErrBadFunction, "Function "+name+"() is not existing");
}

void SerialLink::callFunction(QString name, QByteArray &ba)
{
    if (!isReady())
        return;
    int idx = FuncMap.value(name, -1);
    if (idx >= 0) // function exists
        callFunction(idx, ba.data(), ba.size());
    else
        emit error(clErrBadFunction, "Function "+name+"() is not existing");
}
//---------------------------------------------------------------------------

void SerialLink::paramInfo(unsigned char id, SUartParam &info)
{
    if (id >= paramCount())
    {
        QString err;
        err.sprintf("Parameter index [%d] out of range", id);
        emit error(clErrBadParameter, err);
//        info = 0L;
        return;
    }
    info = Params[id];
}

void SerialLink::paramInfo(QString name, SUartParam &info)
{
    int idx = ParamMap.value(name, -1);
    if (idx >= 0) // parameter exists
        paramInfo(idx, info);
    else
       emit error(clErrBadParameter, "Parameter with name '"+name+"' is not existing");
}

void SerialLink::funcInfo(unsigned char id, SUartFunc &info)
{
    if (id >= funcCount())
    {
        QString err;
        err.sprintf("Function index [%d] out of range", id);
        emit error(clErrBadFunction, err);
//        info = 0L;
        return;
    }
    info = Funcs[id];
}

void SerialLink::funcInfo(QString name, SUartFunc &info)
{
    int idx = FuncMap.value(name, -1);
    if (idx >= 0) // function exists
        funcInfo(idx, info);
    else
        emit error(clErrBadFunction, "Function "+name+"() is not existing");
}
//---------------------------------------------------------------------------
