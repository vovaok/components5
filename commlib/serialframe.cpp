#include "serialframe.h"

SerialFrame::SerialFrame(QIODevice *device) :
    QObject(device),
    mDev(device)
{
    cmd_acc = false;
    mFramesSent = 0;
    connect(mDev, &QIODevice::readyRead, this, &SerialFrame::onDataReady);
}

void SerialFrame::setActive(bool active)
{
    if (!active)
    {
        mDev->close();
        emit disconnected();
    }
    else
    {
        if (mDev->open(QIODevice::ReadWrite))
            emit connected();
    }
}

void SerialFrame::onDataReady()
{
    QByteArray ba = mDev->readAll();
    int cnt = ba.size();
    for (int i=0; i<cnt; i++)
    {
        char byte = ba[i];
        switch (byte)
        {
          case uartESC:
            esc = true;
            break;

          case uartSOF:
            buffer.clear();
            cs = 0;
            esc = false;
            cmd_acc = true;
            break;

          case uartEOF:
            if (cmd_acc)
            {
                if (!cs)
                {
                    buffer.chop(1); // remove checksum
                    mFramesReceived++;
                    emit dataReceived(buffer);
                }
                cmd_acc = false;
            }
            break;

          default:
            if (!cmd_acc)
                break;
            if (esc)
                byte ^= 0x20;
            esc = false;
            buffer.append(byte);
            cs += byte;
        }
    }
}

void SerialFrame::sendData(const QByteArray &data)
{
    if (!mDev->isOpen())
        return;

    QByteArray out;
    char cs = 0;

    out.append(uartSOF);

    for (char i=0; i<data.size(); i++)
    {
        char b = data[i];
        cs -= b;
        if (b == uartESC || b == uartSOF || b == uartEOF)
        {
            out.append(uartESC);
            b ^= 0x20;
        }
        out.append(b);
    }

    if (cs == uartESC || cs == uartSOF || cs == uartEOF)
    {
        out.append(uartESC);
        cs ^= 0x20;
    }

    out.append(cs);

    out.append(uartEOF);

    mDev->write(out);
    mFramesSent++;
}
