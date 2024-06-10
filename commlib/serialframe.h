#ifndef SERIALFRAME_H
#define SERIALFRAME_H

#include <QIODevice>

class SerialFrame : public QObject
{
    Q_OBJECT

private:
    QIODevice *mDev;

    QByteArray buffer;
    static const char uartSOF = '{';
    static const char uartESC = '\\';
    static const char uartEOF = '}';

    char cs;
    bool esc, cmd_acc;
    int mFramesSent;
    int mFramesReceived;

protected:
    QIODevice *device() {return mDev;}

public:
    explicit SerialFrame(QIODevice *device);

    void setActive(bool active);
    bool isActive() const {return mDev->isOpen();}

    int framesSent() {return mFramesSent;}
    int framesReceived() {return mFramesReceived;}

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &data);

private slots:
    void onDataReady();

public slots:
    void sendData(const QByteArray &data);
};

#endif // SERIALFRAME_H
