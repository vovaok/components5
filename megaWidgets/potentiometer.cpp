#include "potentiometer.h"

Potentiometer::Potentiometer(QWidget *parent) :
    QWidget(parent),
    mValue(0),
    mAngle(300),
    mR(0)
{ 
    setMinimumSize(16, 16);
}
//---------------------------------------------------------------------------

void Potentiometer::prepare()
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
    QVector<QPointF> pts;
    for (int i=0; i<6; i++)
    {
        float a = i*2*M_PI/6 + M_PI/2;
        pts << QPointF(cos(a), sin(a));
    }

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
    p.drawConvexPolygon(pts);

    p.setPen(QPen(Qt::darkGray, 0.05));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(QPointF(0, 0), 0.625, 0.625);

    pen.setWidthF(0.25);
    p.setPen(pen);
    p.setBrush(QColor(64, 64, 64));
    p.drawEllipse(QPointF(0, 0), 0.5, 0.5);

    p.end();
}
//---------------------------------------------------------------------------

void Potentiometer::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawImage(0, 0, mBack);

    p.translate(mC);
    p.scale(mR, -mR);

//    p.setPen(QPen(QColor(0, 0, 0, 64), 0.1));
    QConicalGradient grad(0.5, 0.5, 240);
    grad.setCoordinateMode(QGradient::ObjectBoundingMode);
    grad.setColorAt(0.0, QColor(255, 255, 255, 255));
    grad.setColorAt(0.05, QColor(255, 255, 255, 255));
    grad.setColorAt(0.1, QColor(200, 200, 200, 150));
    grad.setColorAt(0.5, QColor(100, 100, 100, 250));
    grad.setColorAt(0.9, QColor(200, 200, 200, 150));
    grad.setColorAt(0.95, QColor(255, 255, 255, 255));
    grad.setColorAt(1.0, QColor(255, 255, 255, 255));

    QPen pen(QPen(QBrush(grad), 0.05));
    p.setPen(pen);
    p.setBrush(Qt::lightGray);
    QRectF chr(-0.3, -0.3, 0.6, 0.6);
    int a = (mValue*mAngle/2 + 90);
    p.drawChord(chr, (a+20)*16, 140*16);
    p.drawChord(chr, (a+200)*16, 140*16);

    p.end();
}
//---------------------------------------------------------------------------

void Potentiometer::resizeEvent(QResizeEvent *)
{
    prepare();
}
//---------------------------------------------------------------------------

void Potentiometer::setValue(float val)
{
    val = val<-1? -1: val>1? 1: val;
    if (mValue != val)
    {
        mValue = val;
        repaint();
    }
}
//---------------------------------------------------------------------------
