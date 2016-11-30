#include "led.h"

Led::Led(QWidget *parent) :
    QWidget(parent),
    mColor(Qt::red),
    mState(false),
    mFlags((LedFlags)0x00)
{
    setMinimumSize(8, 8);
}

Led::Led(QColor col, LedFlags flags, bool on, QWidget *parent) :
    QWidget(parent),
    mColor(col),
    mState(on),
    mFlags(flags)
{
    setMinimumSize(8, 8);
//    prepareImg();
    if (mFlags.testFlag(Clickable))
        setCursor(Qt::PointingHandCursor);
}

void Led::setFlags(LedFlags flags)
{
    mFlags = flags;
    if (mFlags.testFlag(Clickable))
        setCursor(Qt::PointingHandCursor);
    else
        setCursor(Qt::ArrowCursor);
}
//---------------------------------------------------------------------------

void Led::setState(bool on)
{
    if (mState != on)
    {
        mState = on;
        repaint();
    }
}
//---------------------------------------------------------------------------

void Led::paintEvent(QPaintEvent *e)
{
    (void)e;
    if (mFlags.testFlag(InvisibleWhenOff) && !mState)
        return;

    QPainter p(this);
    if (mState)
        p.drawImage(0, 0, mOnImg);
    else
        p.drawImage(0, 0, mOffImg);
    p.end();
}
//---------------------------------------------------------------------------

void Led::mousePressEvent(QMouseEvent *)
{
    if (mFlags.testFlag(Clickable))
        emit clicked();
}
//---------------------------------------------------------------------------

void Led::resizeEvent(QResizeEvent *)
{
    prepareImg();
}
//---------------------------------------------------------------------------

void Led::prepareImg()
{
    qreal w = width();
    qreal h = height();

    mOffImg = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    mOffImg.fill(Qt::transparent);
    mOnImg = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    mOnImg.fill(Qt::transparent);

    QColor colOff = mColor.darker(500);
    QColor colOn = mColor;
    QColor trColor = mColor;
    trColor.setAlphaF(0.5);

    QRadialGradient gradOff(0.5, 0.5, 0.5, 0.25, 0.25);
    gradOff.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradOff.setColorAt(0.0, Qt::white);
    gradOff.setColorAt(0.3, colOff.lighter());
    gradOff.setColorAt(0.8, colOff);
    gradOff.setColorAt(1.0, colOff.darker());

    QRadialGradient gradOn(0.5, 0.5, 0.5, 0.25, 0.25);
    gradOn.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradOn.setColorAt(0.0, Qt::white);
    gradOn.setColorAt(0.3, colOn.lighter());
    gradOn.setColorAt(0.8, colOn);
    gradOn.setColorAt(1.0, colOn.darker());

    QRadialGradient grad2(0.5, 0.5, 0.5);
    grad2.setCoordinateMode(QGradient::ObjectBoundingMode);
    grad2.setColorAt(0.0, trColor);
    grad2.setColorAt(0.5, trColor);
    grad2.setColorAt(1.0, Qt::transparent);

    QPainter p(&mOffImg);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(gradOff);
    p.drawEllipse(w/4, h/4, w/2, h/2);
    p.end();

    p.begin(&mOnImg);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(grad2);
    p.drawEllipse(0, 0, w-1, h-1);
    p.setBrush(gradOn);
    p.drawEllipse(w/4, h/4, w/2, h/2);
    p.end();
}
//---------------------------------------------------------------------------
