#ifndef SERIALLINK_H
#define SERIALLINK_H

#include "serialframe.h"
#include <QVector>
#include <QMap>
#include <QTimer>

#define UART_DATA_BUF_SIZE   128

class SerialLink : public SerialFrame
{
    Q_OBJECT

private: // typedefs
#pragma pack(push,1)
    typedef enum
    {
        // service commands: 0x0...0x7
        cmdEcho         = 0x0,
        cmdInfo         = 0x1,
    //    cmdRetrieveCaps = 0x4,
        cmdUartLinkVersion = 0x7, // also for backward compatibility

        // parameter commands: 0x8...0xB
        cmdGetParamInfo = 0x8,
        cmdGetParam     = 0x9,
        cmdSetParam     = 0xA,
    //    cmdStoreParam   = 0xB,
    //    cmdSetParamDef  = 0xB,

        // remote function call commands: 0xC...0xF
        cmdGetFuncInfo  = 0xC,
        cmdCallFunc     = 0xD
    } EUartCommand;

    typedef enum
    {
        icmdProductID       = 0x01,
        icmdProductName     = 0x02,
        icmdProductVersion  = 0x03,
        icmdCompilationDate = 0x05,
        icmdCompilationTime = 0x06,
        icmdResources       = 0x07
    } EUartInfoCmd;

    typedef struct
    {
        unsigned char cmd: 4; //EUartCommand
        unsigned char addr: 4;

        unsigned char frag_no: 4; // current fragment number
        unsigned char response: 1;
        unsigned char error: 1;
        unsigned char fragmented: 1;
        unsigned char eof: 1; // end of fragments
    } SUartHeader;

    typedef struct
    {
        SUartHeader hdr;
        unsigned char data[UART_DATA_BUF_SIZE];
    } SUartFrame;

    typedef struct
    {
        SUartFrame frm;
        int size;
    } SUartPacket;

    // param code declarations
public:
    typedef struct
    {
        char size;
        union
        {
            char flags;
            struct
            {
                unsigned char _volatile: 1;
                unsigned char read: 1;
                unsigned char write: 1;
                unsigned char permanent: 1;
                unsigned char io: 1;
            } flags_bit;
        };
        unsigned short ptr;
        char name[8];
    } SUartParam;

private:
    typedef struct
    {
        unsigned char idx;
        unsigned char value[UART_DATA_BUF_SIZE-1];
    } SUartParamCmd;

    // remote function call code declarations:
public:
    typedef struct
    {
        unsigned char params_size: 4;
        unsigned char return_size: 4;
        unsigned char params_ptr:  1;
        unsigned char return_ptr:  1;
    } SUartFuncSizes;

    typedef struct
    {
        unsigned short ptr;
        SUartFuncSizes sizes;
        char name[8];
    } SUartFunc;

private:
    typedef struct
    {
        unsigned char idx;
        unsigned char params[UART_DATA_BUF_SIZE-1];
    } SUartFuncCmd;
#pragma pack(pop)

public: // typedefs

    typedef enum
    {
        cleNotify,
        cleEchoReceived,
        cleProductInfoAvailable,
        cleParamInfoAvailable,
        cleFuncInfoAvailable,
        cleSetParam
    } UartLinkEvent;

    typedef enum
    {
        clErrGetParam,
        clErrSetParam,
        clErrCallFunction,
        clErrBadParameter,
        clErrBadFunction,
    //	clErrBadFunctionCall,
        clErrUnknownCmd,
        clErrTimeout,
        clErrQueueOverflow,
        clErrBadVersion
    } UartLinkError;

private:
    QList<QByteArray> OutQueue;
    bool packetSent;
    QTimer *timeoutTimer;
    int ResponseTimeout;
    //QList<QByteArray> InQueue;

    QVector<SUartParam> Params;
    QVector<SUartFunc> Funcs;
    QMap<QString, int> ParamMap;
    QMap<QString, int> FuncMap;

    bool Ready;
    bool FEchoTestPassed;
    unsigned short UartLinkVersion;
    uint32_t FProductID;
    QString FProductName;
    unsigned short FProductVersion;
    QString FCompilationDate;
    QString FCompilationTime;
    int ParamCount;
    int FuncCount;

private: // methods
    void clearData();

    void requestUartLinkVersion();
    void requestInfo(EUartInfoCmd icmd);

    void requestParameterInfo(unsigned char id);
    void requestParameter(unsigned char id);
    void setParameter(unsigned char id, void *data, int size);
    void requestFunctionInfo(unsigned char id);
    void callFunction(unsigned char id, void *params, int size);

private slots:
    void connectionHandler();
    void onData(QByteArray &data);
    void sendPacket(EUartCommand command, void *data=0L, int size=0);
    void popQueue();
    void timeout();

protected:
    void requestProductInfo();
    void requestProductInfoOld();
    void requestParamData();
    void requestFuncData();

public:
    explicit SerialLink(QIODevice *device);

    bool isReady() {return Ready;}
    bool echoTestPassed() {return FEchoTestPassed;}
    uint32_t productID() {return FProductID;}
    QString productName() {return FProductName;}
    unsigned short productVersion() {return FProductVersion;}
    unsigned char productMajorVersion() {return FProductVersion>>8;}
    unsigned char productMinorVersion() {return FProductVersion&0xFF;}
    QString productVersionString() {return QString().sprintf("%d.%d", productMajorVersion(), productMinorVersion());}
    QString compilationDate() {return FCompilationDate;}
    QString compilationTime() {return FCompilationTime;}

    int paramCount() {return ParamCount;}
    int funcCount() {return FuncCount;}
    void paramInfo(unsigned char id, SUartParam &info);
    void paramInfo(QString name, SUartParam &info);
    void funcInfo(unsigned char id, SUartFunc &info);
    void funcInfo(QString name, SUartFunc &info);

    int responseTimeout() {return ResponseTimeout;}
    void setResponseTimeout(int ms) {if (ms) ResponseTimeout = ms;}

//    TTempEvent FOnTemp;
//    void setCState(String val){if(FOnTemp) FOnTemp(val);}
//    __property String cstate = {write=setCState};

signals:
    void uartEvent(SerialLink::UartLinkEvent);
    void paramAccepted(QString param, void *data);
    void funcReturn(QString func, void *data);
    void error(SerialLink::UartLinkError error, QString errorString);
    void ready();
    void statusChanged(QString status);

public slots:
    void echoTest();
    void requestParameter(QString name);
    void setParameter(QString name, void *params);
    void callFunction(QString name, void *params=0L);//, int Size=-1);
    void callFunction(QString name, QByteArray &ba);
};

#endif // SERIALLINK_H
