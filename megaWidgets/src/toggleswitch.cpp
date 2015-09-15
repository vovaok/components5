#include "toggleswitch.h"

ToggleSwitch::ToggleSwitch(QWidget *parent) :
    QWidget(parent),
    mState(false),
    mOffText("Œ“ À"),
    mOnText("¬ À"),
    mR(0)
{
    setMinimumSize(16, 16);
}
//---------------------------------------------------------------------------

void ToggleSwitch::prepare()
{
    int w = width();
    int h = height();

    mBack = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    mBack.fill(Qt::transparent);

    QPainter p(&mBack);
    p.setRenderHint(QPainter::Antialiasing);

    QFontMetrics fm(p.font());
    int th = fm.height();
    int rh = h - 2*th;
    mR = (w<rh? w: rh) / 2;

    p.drawText(rect(), mOnText, Qt::AlignHCenter | Qt::AlignTop);
    p.drawText(rect(), mOffText, Qt::AlignHCenter | Qt::AlignBottom);

    p.translate(w/2, h/2);
    p.scale(mR, -mR);
    QVector<QPointF> pts;
    for (int i=0; i<6; i++)
    {
        float a = i*2*M_PI/6 + M_PI/2;
        pts << QPointF(cos(a), sin(a));
    }

    QConicalGradient grad(0.5, 0.5, 240);
    grad.setCoordinateMode(QGradient::ObjectBoundingMode);
    grad.setColorAt(0.0, QColor(255, 255, 255, 250));
    grad.setColorAt(0.1, QColor(200, 200, 200, 150));
    grad.setColorAt(0.5, QColor(100, 100, 100, 250));
    grad.setColorAt(0.9, QColor(200, 200, 200, 150));
    grad.setColorAt(1.0, QColor(255, 255, 255, 250));

    QPen pen(QPen(QBrush(grad), 0.11));
    p.setPen(pen);
    p.setBrush(Qt::gray);
    p.drawConvexPolygon(pts);

    p.drawEllipse(QPointF(0, 0), 0.55, 0.55);

    grad.setAngle(60);
    p.setPen(QPen(QBrush(grad), 0.12));
    p.setBrush(Qt::black);
    p.drawRoundedRect(QRectF(-0.25, -0.4, 0.5, 0.8), 0.1, 0.1);

    p.end();
}

void ToggleSwitch::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawImage(0, 0, mBack);

    p.translate(width()/2, height()/2);
    p.scale(mR, -mR);

    if (mState)
    {
        QConicalGradient grad(0, -1, 265);
        grad.setCoordinateMode(QGradient::LogicalMode);
        grad.setColorAt(0, QColor(64, 64, 64));
        grad.setColorAt(0.025, Qt::black);//Qt::darkGray);
        grad.setColorAt(0.05, QColor(32, 32, 32));
        grad.setColorAt(0.95, QColor(32, 32, 32));
        grad.setColorAt(0.975, Qt::lightGray);//QColor(64, 64, 64));
        grad.setColorAt(1, QColor(64, 64, 64));

        QBrush brush(grad);
        p.setBrush(brush);
        p.setPen(QPen(brush, 0.1));
        QVector<QPointF> pts;
        pts << QPointF(-0.15, 0);
        pts << QPointF(-0.3, 0.6);
        pts << QPointF(0.3, 0.6);
        pts << QPointF(0.15, 0);
        p.drawConvexPolygon(pts);

        QRadialGradient grad2(0.25, 0.5, 1, 0.25, 0.75);
        grad2.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad2.setColorAt(0, Qt::white);
        grad2.setColorAt(1, Qt::black);
        QBrush brush2(grad2);
        p.setBrush(brush2);
        p.setPen(QPen(brush2, 0.1));
        p.drawEllipse(QPointF(0, 0.65), 0.3, 0.2);
    }
    else
    {
        QRadialGradient grad2(0.25, 0.5, 0.85, 0.25, 0.5);
        grad2.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad2.setColorAt(0, Qt::white);
        grad2.setColorAt(1, Qt::black);
        QBrush brush2(grad2);
        p.setBrush(brush2);
        p.setPen(QPen(brush2, 0.1));
        p.drawEllipse(QPointF(0, -0.65), 0.3, 0.2);

        QConicalGradient grad(0, 1, 95);
        grad.setCoordinateMode(QGradient::LogicalMode);
        grad.setColorAt(0, Qt::white);
        grad.setColorAt(0.025, Qt::darkGray);
        grad.setColorAt(0.05, Qt::black);
        grad.setColorAt(0.95, Qt::black);
        grad.setColorAt(0.975, Qt::darkGray);
        grad.setColorAt(1, Qt::white);

        QBrush brush(grad);
        p.setBrush(brush);
        p.setPen(QPen(brush, 0.1));
        QVector<QPointF> pts;
        pts << QPointF(-0.15, 0);
        pts << QPointF(-0.3, -0.6);
        pts << QPointF(0.3, -0.6);
        pts << QPointF(0.15, 0);
        p.drawConvexPolygon(pts);
    }

    p.end();
}
//---------------------------------------------------------------------------

void ToggleSwitch::resizeEvent(QResizeEvent *)
{
    prepare();
}
//---------------------------------------------------------------------------

void ToggleSwitch::setState(bool on)
{
    if (mState != on)
    {
        mState = on;
        repaint();
    }
}
//---------------------------------------------------------------------------
