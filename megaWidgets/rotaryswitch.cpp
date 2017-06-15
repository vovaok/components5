#include "rotaryswitch.h"

RotarySwitch::RotarySwitch(QWidget *parent) :
    QWidget(parent),
    mCurPos(0),
    mStartAngle(0),
    mAngleStep(0)
{
    setFixedSize(160, 160);
}
//---------------------------------------------------------------------------

void RotarySwitch::paintEvent(QPaintEvent *)
{
    int cnt = mLabels.count();
    int R = (width()<height()? width(): height()) / 2;
    int sx = width() / 2;
    int sy = height() / 2;
    QPoint s(sx, sy);
    int r1 = R;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(s);
    if (cnt)
    {
        float step = mAngleStep * M_PI/180;
        float maxStep = 2*M_PI / cnt;
        if (mAngleStep > 0)
            mAs = step>maxStep? maxStep: step;
        else if (mAngleStep < 0)
            mAs = step<-maxStep? -maxStep: step;
        else
            mAs = maxStep;

        QVector<QRect> rects;
        QFontMetrics fm(p.font());
        for (int i=0; i<cnt; i++)
        {
            float a = mStartAngle * M_PI/180 + mAs * i;

            QRect rect = fm.boundingRect(mLabels[i]);
            r1 = R - rect.height()/2;
            QPoint p2(r1*cos(a), -r1*sin(a));
            rect.moveCenter(p2);
            rects << rect;
        }

        R = 3*r1 - 2*R;
        int r = R * 3/4;
        int pw = R / 30.0;

        p.setPen(QPen(Qt::black, pw));
        for (int i=0; i<cnt; i++)
        {
            float a = mStartAngle * M_PI/180 + mAs * i;
            QPoint p1(r*cos(a), -r*sin(a));
            QPoint p2(R*cos(a), -R*sin(a));
            p.drawEllipse(p1, pw, pw);
            p.drawLine(p1, p2);
            QRect rect = rects[i].adjusted(0, 0, 10, 0);
            p.drawText(rect, mLabels[i]);
        }

        p.rotate(-mStartAngle - mAs*180/M_PI * mCurPos);
        QRadialGradient grad(0.5, 0.5, 0.5, 0.25, 0.25);
        grad.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad.setColorAt(0, Qt::white);
        grad.setColorAt(1, Qt::black);
        p.setBrush(grad);
        p.drawEllipse(QPoint(0, 0), (int)(r*0.6), (int)(r*0.4));
        QVector<QPoint> points;
        points << QPoint(-r*0.45, -r*0.25);
        points << QPoint(-r*0.45, r*0.25);
        points << QPoint(r*0.95, 0);
        QPen pen(Qt::black, r*0.25);
        pen.setJoinStyle(Qt::RoundJoin);
        p.setPen(pen);
        p.setBrush(Qt::black);
        p.drawPolygon(points.data(), points.count());
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::white);
        p.drawEllipse(QPoint(r*0.65, 0), (int)(r*0.1), (int)(r*0.1));
    }
    p.end();
}
//---------------------------------------------------------------------------

void RotarySwitch::add(QString position)
{
    mLabels << position;
    repaint();
}
//---------------------------------------------------------------------------

void RotarySwitch::setPosition(int pos)
{
    if (mCurPos != pos)
    {
        mCurPos = pos>=0&&pos<mLabels.count()? pos: 0;
        repaint();
    }
}
//---------------------------------------------------------------------------
