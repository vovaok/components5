#include "ledcontainer.h"

LedContainer::LedContainer(QWidget *parent) :
    QGroupBox(parent),
    mLedSize(24),
    mHorizontal(false)
{
    setMinimumHeight(64);
    QGridLayout *lay = new QGridLayout;
    lay->setContentsMargins(0, 0, 4, 8);
    lay->setSpacing(0);
    setLayout(lay);
}
//---------------------------------------------------------------------------

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
    else
    {
        cnt += cnt>>3;
        lay->addWidget(led, cnt, 0, Qt::AlignVCenter);
        lay->addWidget(lbl, cnt, 1, Qt::AlignVCenter);
    }

    mLeds << led;
    mLabels << lbl;

    if (!(mLeds.count() & 7) && !mHorizontal)
    {
        QLabel *l = new QLabel();
        l->setFixedHeight(8);
        lay->addWidget(l, ++cnt, 1);
    }

    if (mHorizontal)
        setMinimumHeight((8+1)*mLedSize*2/3);
    else
        setMinimumHeight((cnt+1)*mLedSize*2/3);
}

void LedContainer::add(QStringList labels)
{
    QRegExp rx("([^\\|]*)\\|(.*)");
//    rx.setMinimal(true);
    foreach (QString label, labels)
    {
        if (rx.indexIn(label) != -1)
        {
            add(rx.cap(1), rx.cap(2));
        }
        else
        {
            add(label);
        }
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
        mLeds[i]->setFixedSize(mLedSize, mLedSize);
        mLabels[i]->setFixedHeight(mLedSize);
    }
    cnt += cnt>>3;
    setMinimumHeight(cnt*mLedSize*2/3);
}

void LedContainer::setLedsClickable(bool enabled)
{
    for (int i=0; i<mLeds.count(); i++)
        mLeds[i]->setFlags(enabled? Led::Clickable: Led::NoFlags);
}

void LedContainer::setHorizontal(bool horizontal)
{
    mHorizontal = horizontal;
}

void LedContainer::onLedClick()
{
    Led *led = qobject_cast<Led*>(sender());
    led->setState(!led->state());
}
//---------------------------------------------------------------------------
