#include "ledcontainer.h"

LedContainer::LedContainer(QWidget *parent) :
    QGroupBox(parent),
    mLedSize(24),
    mHorizontal(false),
    mBytesInColumn(0)
{
    setMinimumHeight(64);
    QGridLayout *lay = new QGridLayout;
    lay->setContentsMargins(0, 0, 4, 8);
    lay->setSpacing(0);
    setLayout(lay);
}

LedContainer::LedContainer(const QString &title, QWidget *parent) :
    QGroupBox(title, parent),
    mLedSize(24),
    mHorizontal(false),
    mBytesInColumn(0)
{
    setMinimumHeight(64);
    QGridLayout *lay = new QGridLayout;
    lay->setContentsMargins(0, 0, 4, 8);
    lay->setSpacing(0);
    setLayout(lay);
}
//---------------------------------------------------------------------------

void LedContainer::add(QString label)
{
    QRegExp rx("([^\\|]*)\\|(.*)");
    if (rx.indexIn(label) != -1)
    {
        add(rx.cap(1), rx.cap(2));
    }
    else
    {
        add(label, Qt::green);
    }
}

void LedContainer::add(QString label, QColor color)
{
    QGridLayout *lay = qobject_cast<QGridLayout*>(layout());

    if (label.isNull())
    {
        mLeds << 0L;
        mLabels << 0L;
        return;
    }

    Led *led = new Led(color);
    led->setFixedSize(mLedSize, mLedSize);
    connect(led, SIGNAL(clicked()), SLOT(onLedClick()));
    if (mHorizontal)
        label += "               ";
    QLabel *lbl = new QLabel(label);
    lbl->setMinimumHeight(mLedSize);

    int cnt = mLeds.count();
    if (mHorizontal)
    {
        int hcnt = cnt >> 3;
        int vcnt = cnt & 7;
        lay->addWidget(led, vcnt, hcnt*2, Qt::AlignVCenter);
        lay->addWidget(lbl, vcnt, hcnt*2+1, Qt::AlignVCenter);
    }
    else if (mBytesInColumn)
    {
        int hcnt = cnt / (8 * mBytesInColumn);
        int vcnt = cnt % (8 * mBytesInColumn);

        if (vcnt && !(vcnt & 7))
        {
            QLabel *l = new QLabel();
            l->setFixedHeight(8);
            lay->addWidget(l, vcnt+(vcnt>>3)-1, hcnt*2+1);
        }

        vcnt += vcnt>>3;
        lay->addWidget(led, vcnt, hcnt*2, Qt::AlignVCenter);
        lay->addWidget(lbl, vcnt, hcnt*2+1, Qt::AlignVCenter);
    }
    else
    {
        cnt += cnt>>3;
        lay->addWidget(led, cnt, 0, Qt::AlignVCenter);
        lay->addWidget(lbl, cnt, 1, Qt::AlignVCenter);
    }

    mLeds << led;
    mLabels << lbl;

    if (!(mLeds.count() & 7) && !mHorizontal && !mBytesInColumn)
    {
        QLabel *l = new QLabel();
        l->setFixedHeight(8);
        lay->addWidget(l, ++cnt, 1);
    }

    if (mHorizontal)
        setMinimumHeight((8+1)*mLedSize*2/3);
    else if (mBytesInColumn)
        setMinimumHeight((8+1)*mBytesInColumn*mLedSize*2/3);
    else
        recalcHeight();
}

void LedContainer::add(QStringList labels)
{
    foreach (QString label, labels)
    {
        add(label);
    }
}
//---------------------------------------------------------------------------

void LedContainer::setLed(int number, bool state)
{
    if (number >= 0 && number < mLeds.count())
    {
        if (mLeds[number])
            mLeds[number]->setState(state);
    }
}

bool LedContainer::ledState(int number)
{
    if (number >= 0 && number < mLeds.count())
    {
        if (mLeds[number])
            return mLeds[number]->state();
    }
    return false;
}

void LedContainer::setData(const void *data)
{
    for (int i=0; i<mLeds.count(); i++)
    {
        Led *led = mLeds[i];
        if (!led)
            continue;
//        led->resize(28, 28);
        int byte = i >> 3;
        int bit = i & 7;
        unsigned char b = reinterpret_cast<const char*>(data)[byte];
        bool state = b & (1<<bit);
        led->setState(state);
    }
}

void LedContainer::getData(void *data)
{
    for (int i=0; i<mLeds.count(); i++)
    {
        Led *led = mLeds[i];
        if (!led)
            continue;
        bool state = led->state();
        int byte = i >> 3;
        int bit = i & 7;
        unsigned char &b = reinterpret_cast<unsigned char*>(data)[byte];
        if (state)
            b |= (1<<bit);
        else
            b &=~(1<<bit);
    }
}
//---------------------------------------------------------------------------

void LedContainer::setLedSize(int size)
{
    mLedSize = size;
    int cnt = mLeds.count();
    for (int i=0; i<cnt; i++)
    {
        if (!mLeds[i])
            continue;
        mLeds[i]->setFixedSize(mLedSize, mLedSize);
        mLabels[i]->setFixedHeight(mLedSize);
    }
    recalcHeight();
}

void LedContainer::setLedsClickable(bool enabled)
{
    for (int i=0; i<mLeds.count(); i++)
    {
        if (mLeds[i])
            mLeds[i]->setFlags(enabled? Led::Clickable: Led::NoFlags);
    }
}

void LedContainer::setHorizontal(bool horizontal)
{
    mHorizontal = horizontal;
}

void LedContainer::onLedClick()
{
    Led *led = qobject_cast<Led*>(sender());
    led->setState(!led->state());
    int idx = mLeds.indexOf(led);
    emit ledClicked(idx);
}
//---------------------------------------------------------------------------

void LedContainer::recalcHeight()
{
    int cnt = mLeds.count();
    int realcnt = 0;
    for (int i=0; i<cnt; i++)
    {
        if (mLeds[i])
            realcnt++;
    }
    realcnt += realcnt>>3;
    setMinimumHeight(realcnt*mLedSize*2/3);
}
