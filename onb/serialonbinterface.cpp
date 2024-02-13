#include "serialonbinterface.h"

using namespace Objnet;

SerialThread::SerialThread(QObject *parent) :
    QThread(parent)
{
}

void SerialThread::setPort(QString name)
{
    m_portName = name;
//    emit internalPortChange(name);
}

bool SerialThread::sendData(const QByteArray &ba)
{
    if (!m_dev->isOpen())
        return false;

    if (m_buffer.isEmpty() && !m_busy)
    {
        m_mutex.lock();
        m_buffer = ba;
//        qDebug() << "buffer=" << ba;
        m_mutex.unlock();
        return true;
    }
    return false;

//    m_timer.start();
//    m_serial->sendData(ba);
//    emit internalSendData(ba);
}

bool SerialThread::isActive() const
{
    return m_dev && m_dev->isOpen();
}

void SerialThread::run()
{
    m_dev = new QSerialPort();
    m_dev->setReadBufferSize(256);
    m_dev->setBaudRate(1000000);
    m_dev->setParity(QSerialPort::EvenParity);
    m_dev->setStopBits(QSerialPort::OneStop);

//    m_serial = new SerialFrame(m_dev);
//    connect(m_serial, &SerialFrame::dataReceived, this, &SerialThread::dataReceived);
//    connect(m_serial, &SerialFrame::dataReceived, this, &SerialThread::onDataReceived);
//    connect(this, &SerialThread::internalSendData, m_serial, &SerialFrame::sendData);
//    connect(this, &SerialThread::internalPortChange, this, &SerialThread::changePort);

//    connect(this, &SerialThread::internalSendData, [=](const QByteArray &ba)
//    {
//        m_timer.start();
//        m_serial->sendData(ba);
//    });

//    exec();
    while (!isInterruptionRequested())
    {
        if (m_portName != m_dev->portName())
            changePort(m_portName);

        if (m_dev->isOpen() && !m_buffer.isEmpty())
        {
            m_mutex.lock();
//            qDebug() << ">>" << m_buffer.toHex(' ');
            if (m_buffer[3] & 0x10)
            {
                m_busy = true;
//                qDebug() << "busy=true";
            }
            m_dev->write(encode(m_buffer));
            m_timer.start();
            m_dev->waitForBytesWritten(1);
            m_buffer.clear();
            m_mutex.unlock();
        }

//        // wait 500 us
//        while (m_timer.nsecsElapsed() < 2000000)
//        {

//        }

        if (m_dev->isOpen())// && m_busy)
        {
//            QByteArray ba;

//            bool newdata = m_dev->waitForReadyRead(9);
//            qDebug() << "newdata =" << newdata;
//            if (newdata)
//                m_timer.start();

//            while (m_timer.nsecsElapsed() < 10000000)
//            {
//                char b;
//                int r = m_dev->read(&b, 1);
//                if (r < 1)
//                    continue;
//                ba.append(b);
//                if (b != '}')
//                    continue;
                QByteArray ba = m_dev->peek(256);
//                if (!ba.isEmpty())
//                    qDebug() << ba;
                int size = ba.indexOf('}') + 1;
                ba = decode(m_dev->read(size));
//                ba = decode(ba);
                if (!ba.isEmpty())
                {
//                    qDebug() << "[SerialThread] response time" << m_timer.nsecsElapsed() / 1000 << "µs";
//                    qDebug() << ba.toHex(' ');
//                        m_timer.invalidate();

                    emit dataReceived(ba);
//                    ba.clear();
//                    if (!m_dev->bytesAvailable())
//                        break;
    //                onDataReceived(ba);
                }
//            }
        }

//        if (m_busy == true)
//        {
//            qDebug() << "busy=false";
//            qDebug() << "";
//        }
        m_busy = false;



        usleep(100); // na samom dele 2 ms

    }

    m_dev->close();
    m_dev->deleteLater();
}

QByteArray SerialThread::encode(const QByteArray &ba)
{
    QByteArray out;
    char cs = 0;
    out.append('{');
    int sz = ba.size();
    for (char i=0; i<=sz; i++)
    {
        char b = i<sz? ba[i]: cs;
        cs -= b;
        switch (b)
        {
        case '{':
        case '}':
        case '\\':
            out.append('\\');
            b ^= 0x20;
        }
        out.append(b);
    }
    out.append('}');
    return out;
}

QByteArray SerialThread::decode(const QByteArray &ba)
{
    QByteArray mBuffer;
    bool esc = false;
    uint8_t cs = 0;
    bool cmd_acc = false;
//    bool noSOF = false;
    for (int i=0; i<ba.size(); i++)
    {
        char byte = ba[i];
        switch (byte)
        {
        case '\\':
            esc = true;
            break;

        case '{':
            mBuffer.clear();
            cs = 0;
//            noSOF = false;
            esc = false;
            cmd_acc = true;
            break;

        case '}':
            if (cmd_acc)
            {
                if (!cs)
                {
                    if (mBuffer.size())
                    {
                        mBuffer.chop(1); // remove checksum
                        return mBuffer;
                    }
                }
//                else if (errorEvent)
//                {
//                    errorEvent(mCurTxMac, ErrorChecksum);
//                }
                cmd_acc = false;
            }
            break;

        default:
//            if (!cmd_acc)
//            {
//                if (!noSOF && errorEvent)
//                {
//                    noSOF = 1;
//                    errorEvent(mCurTxMac, ErrorNoSOF);
//                }
//                break;
//            }
            if (esc)
                byte ^= 0x20;
            esc = 0;
            mBuffer.append(byte);
            cs += byte;
        }
    }
    return QByteArray();
}

//void SerialThread::onDataReceived(const QByteArray &ba)
//{
//    qDebug() << "[SerialThread] response time" << m_timer.nsecsElapsed() / 1000 << "µs";
//    emit dataReceived(ba);
//}

void SerialThread::changePort(QString name)
{
    if (m_dev)
    {
        m_dev->close();
        m_dev->setPortName(name);
        if (!name.isEmpty())
            m_dev->open(QIODevice::ReadWrite);
    }
}
//-----------------------------------------------------

SerialOnbInterface::SerialOnbInterface(/*QIODevice *device,*/ bool half_duplex)
{
    mMaxFrameSize = 64;
    if (half_duplex)
        mBusType = BusSwonb;
    else
        mBusType = BusWifi;

//    m_serial = new SerialFrame(device);
    m_serial = new SerialThread(this);
    connect(m_serial, &QThread::finished, m_serial, &QThread::deleteLater);

    connect(m_serial, &SerialThread::dataReceived, this, &SerialOnbInterface::onDataReceived);

    m_serial->start();
}

SerialOnbInterface::~SerialOnbInterface()
{
    m_serial->requestInterruption();
    m_serial->quit();
    m_serial->wait(1000);
    m_serial->terminate();
}

bool SerialOnbInterface::isBusPresent() const
{
    return m_serial->isActive();
}

bool SerialOnbInterface::send(const CommonMessage &msg)
{
//    if (mBusType == BusSwonb && m_timeoutTimer.isValid() && m_timeoutTimer.elapsed() < 10)
//    {
//        return false;
//    }

    QByteArray ba;
    uint32_t id = msg.rawId();
    ba.append(reinterpret_cast<const char*>(&id), 4);
    ba.append(msg.data());

//    qDebug() << "try send" << ba.toHex(' ');

    bool result = m_serial->sendData(ba);
    if (result)
        emit message("serial", msg); // for debug purposes

//    if (msg.isLocal())
//        m_timeoutTimer.start();
//    else
//        m_timeoutTimer.invalidate();
    return result;
}
//---------------------------------------------------------

int SerialOnbInterface::addFilter(uint32_t id, uint32_t mask)
{
    Q_UNUSED(id);
    Q_UNUSED(mask);
    return 0;
}

void SerialOnbInterface::removeFilter(int number)
{
    Q_UNUSED(number);
}
//---------------------------------------------------------

void SerialOnbInterface::onDataReceived(const QByteArray &ba)
{
//    qDebug() << "response in" << m_timeoutTimer.elapsed() << "ms";
//    m_timeoutTimer.invalidate();
    CommonMessage msg(ba);
    receive(std::move(msg));
    emit message("serial", msg); // for debug purposes
}
