#include "sbutton.h"

SButton::SButton(QWidget *parent) :
    QAbstractButton(parent),
    mState(false),
    mColor(QColor(192, 64, 64))
{
    setMinimumSize(8, 8);
}
//---------------------------------------------------------------------------

void SButton::prepare()
{
    int w = width();
    int h = height();

    mBack = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    mBack.fill(Qt::transparent);

    QPainter p(&mBack);
    p.setRenderHint(QPainter::Antialiasing);

    QFontMetrics fm(p.font());
    int th = fm.height();
    int rh = h - th;
    mR = (w<rh? w: rh) / 2;

    p.drawText(rect(), mText, Qt::AlignHCenter | Qt::AlignTop);

    mC = QPointF(w/2, (h+th)/2);
    p.translate(mC);
    p.scale(mR, -mR);

    QConicalGradient grad(0.5, 0.5, 240);
    grad.setCoordinateMode(QGradient::ObjectBoundingMode);
    grad.setColorAt(0.0, QColor(255, 255, 255, 255));
    grad.setColorAt(0.05, QColor(255, 255, 255, 255));
    grad.setColorAt(0.1, QColor(200, 200, 200, 255));
    grad.setColorAt(0.5, QColor(100, 100, 100, 255));
    grad.setColorAt(0.9, QColor(200, 200, 200, 255));
    grad.setColorAt(0.95, QColor(255, 255, 255, 255));
    grad.setColorAt(1.0, QColor(255, 255, 255, 255));

    QPen pen(QPen(QBrush(grad), 0.11));
    p.setPen(pen);
    p.setBrush(Qt::gray);
    p.drawEllipse(QPointF(0, 0), 0.9, 0.9);

    p.setPen(QPen(QColor(64, 64, 64), 0.05));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(QPointF(0, 0), 0.7, 0.7);

    p.end();
}
//---------------------------------------------------------------------------

void SButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawImage(0, 0, mBack);

    p.translate(mC);
    p.scale(mR, -mR);

    if (mState)
    {
        QConicalGradient grad(0.5, 0.5, 240);
        grad.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad.setColorAt(0.0, mColor.lighter(250));
        grad.setColorAt(0.05, mColor.lighter(200));
        grad.setColorAt(0.1, mColor);
        grad.setColorAt(0.5, mColor.darker());
        grad.setColorAt(0.9, mColor);
        grad.setColorAt(0.95, mColor.lighter(200));
        grad.setColorAt(1.0, mColor.lighter(250));

        QRadialGradient grad2(0.5, 0.5, 0.6, 0.65, 0.35);
        grad2.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad2.setColorAt(0, mColor.lighter());
        grad2.setColorAt(0.2, mColor);
        grad2.setColorAt(0.8, mColor.darker());
        grad2.setColorAt(1, mColor.darker(300));

        QPen pen(QPen(QBrush(grad), 0.08));
        p.setPen(pen);
        p.setBrush(grad2);
        p.drawEllipse(QPointF(0, 0), 0.65, 0.65);
    }
    else
    {
        QConicalGradient grad(0.5, 0.5, 240);
        grad.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad.setColorAt(0.0, QColor(255, 255, 255, 255));
        grad.setColorAt(0.05, QColor(255, 255, 255, 255));
        grad.setColorAt(0.1, mColor);
        grad.setColorAt(0.5, mColor.darker());
        grad.setColorAt(0.9, mColor);
        grad.setColorAt(0.95, QColor(255, 255, 255, 255));
        grad.setColorAt(1.0, QColor(255, 255, 255, 255));

        QRadialGradient grad2(0.5, 0.5, 0.6, 0.35, 0.65);
        grad2.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad2.setColorAt(0, mColor.lighter(250));
        grad2.setColorAt(0.2, mColor.lighter());
        grad2.setColorAt(0.8, mColor.lighter(125));
        grad2.setColorAt(1, mColor.darker());

        QPen pen(QPen(QBrush(grad), 0.08));
        p.setPen(pen);
        p.setBrush(grad2);
        p.drawEllipse(QPointF(0, 0), 0.65, 0.65);
    }

    p.end();
}
//---------------------------------------------------------------------------

void SButton::resizeEvent(QResizeEvent *)
{
    prepare();
}
//---------------------------------------------------------------------------

void SButton::setState(bool on)
{
    if (mState != on)
    {
        mState = on;
        repaint();
    }
}
//---------------------------------------------------------------------------
