#ifndef UARTINTERFACE_H
#define UARTINTERFACE_H

#include <QtCore>

class UartInterface2 : public QObject
{
    Q_OBJECT

public:
    explicit UartInterface2() :
        QObject(0L)
    {
    }

    virtual void read(QByteArray &ba) {Q_UNUSED(ba);}
    virtual void connectEvent() {}
    virtual void disconnectEvent() {}
};

class UartInterface
{
private:
    UartInterface2 *mIf;

protected:
    void read(QByteArray &ba) {if (mIf) mIf->read(ba);}
    void connectEvent() {if (mIf) mIf->connectEvent();}
    void disconnectEvent() {if (mIf) mIf->disconnectEvent();}

public:
    explicit UartInterface() : mIf(nullptr) {}
    void linkWithInterface(UartInterface2 *if2) {mIf = if2;}

    virtual void write(const QByteArray &ba) {Q_UNUSED(ba);}
};



#endif // UARTINTERFACE_H
