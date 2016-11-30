#include "scalerindicator.h"

ScalerIndicator::ScalerIndicator(QWidget *parent) :
    QWidget(parent),
    mValue(0), mValue2(0),
    m2Enabled(false),
    mMin(0), mMax(1),
    mGraduation(0.1), mDivs(5),
    mScaleWidth(180),
    mIndicatorAngle(180),
    mPrecision(-1),
    mR(16)
{
    setMinimumSize(48, 48);
    mArrow << QPointF(0, -0.03);
    mArrow << QPointF(0.5, -0.02);
    mArrow << QPointF(0.7, -0.02);
    mArrow << QPointF(0.75, 0);
    mArrow << QPointF(0.95, 0);
    mArrow << QPointF(0.75, 0);
    mArrow << QPointF(0.7, 0.02);
    mArrow << QPointF(0.5, 0.02);
    mArrow << QPointF(0, 0.03);

    mArrow2 << QPointF(0.98, -0.05);
    mArrow2 << QPointF(0.95, -0.05);
    mArrow2 << QPointF(0.9, 0);
    mArrow2 << QPointF(0.95, 0.05);
    mArrow2 << QPointF(0.98, 0.05);
}
//---------------------------------------------------------------------------

void ScalerIndicator::prepareBack()
{
    int w = width();
    int h = height();
    int wr = (mIndicatorAngle<180? w/(2*sin(mIndicatorAngle*M_PI/180/2)): w/2) - 1;
    int hr = h/(1-cos(mIndicatorAngle*M_PI/180/2)) - 1;
    mR = hr<wr? hr: wr;
    QRect brect(w/2-mR, 1, 2*mR, 2*mR);

    back = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    back.fill(Qt::transparent);
    QPainter p(&back);
    p.setRenderHint(QPainter::Antialiasing);

    p.translate(w/2, mR+1);
    p.scale(-mR, -mR);

    // draw scale
    p.setPen(QPen(Qt::black, 0.005));
    float r1=0.8, r2=0.9;
    float div = mGraduation / mDivs;
    for (float v=mMin; v<=mMax; v+=mGraduation)
    {
        r1 = 0.8;
        if (v == 0 || v == mMin || v == mMax)
            r1 = 0.75;
        for (int i=0; i<mDivs; i++)
        {
            float a = (((v+div*i) - mMin) / (mMax-mMin) * mScaleWidth + (180-mScaleWidth)/2) * M_PI/180;
            QPointF p1(r1*cos(a), r1*sin(a));
            QPointF p2(r2*cos(a), r2*sin(a));
            p.drawLine(p1, p2);
            r1 = 0.85;
            if (v == mMax)
                break;
        }
    }
    p.end();

    front = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    front.fill(Qt::transparent);
    p.begin(&front);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::darkGray);
    QConicalGradient grad(w/2, mR, 120);
    grad.setCoordinateMode(QGradient::LogicalMode);
    grad.setColorAt(0.0, QColor(255, 255, 255, 150));
    grad.setColorAt(0.1, QColor(200, 200, 200, 50));
    grad.setColorAt(0.3, QColor(100, 100, 100, 100));
    grad.setColorAt(0.4, QColor(200, 200, 200, 50));
    grad.setColorAt(0.5, QColor(255, 255, 255, 150));
    grad.setColorAt(0.6, QColor(200, 200, 200, 50));
    grad.setColorAt(0.8, QColor(100, 100, 100, 100));
    grad.setColorAt(0.9, QColor(200, 200, 200, 50));
    grad.setColorAt(1.0, QColor(255, 255, 255, 150));
    p.setBrush(grad);
    p.drawChord(brect, (90-mIndicatorAngle/2)*16, mIndicatorAngle*16); // Pie
    p.end();
}
//---------------------------------------------------------------------------

void ScalerIndicator::paintEvent(QPaintEvent *)
{
    int w2 = width() / 2;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawImage(0, 0, back);

    // draw value
    QString text = QString::number(mValue, 'f', mPrecision) + " " + mUnit;
    QFont font = p.font();
    font.setPixelSize(mR*0.15);
    p.setFont(font);
    QRect trect(w2-mR, 1-mR/3, 2*mR, 2*mR);
    p.drawText(trect, Qt::AlignCenter, text);

    p.save();
    p.translate(w2, mR);
    p.scale(-mR, -mR);

    // draw arrow 2
    if (m2Enabled)
    {
        p.save();
        float v = (mValue2 - mMin) / (mMax - mMin);
        v = v<0? 0: v>1? 1: v;
        float a = v * mScaleWidth + (180-mScaleWidth)/2;
        p.setPen(QPen(Qt::blue, 0.005));
        p.setBrush(Qt::blue);
        p.rotate(a);
        p.drawPolygon(mArrow2);
        p.restore();
    }

    // draw arrow
    float v = (mValue - mMin) / (mMax - mMin);
    v = v<0? 0: v>1? 1: v;
    float a = v * mScaleWidth + (180-mScaleWidth)/2;
    p.setPen(QPen(Qt::red, 0.005));
    p.setBrush(QColor(255, 0, 0, 192));
    p.rotate(a);
    p.drawEllipse(QPointF(0, 0), 0.03, 0.03);
    p.drawPolygon(mArrow);

    p.restore();
    p.drawImage(0, 0, front);
    p.end();
}
//---------------------------------------------------------------------------

void ScalerIndicator::resizeEvent(QResizeEvent *)
{
    prepareBack();
}

//QSize ScalerIndicator::sizeHint()
//{
//    int w = width();
//    int h = height();
//    int wr = (mIndicatorAngle<180? w/(2*sin(mIndicatorAngle*M_PI/180/2)): w/2) - 1;
//    int hr = h/(1-cos(mIndicatorAngle*M_PI/180/2)) - 1;
//    mR = hr<wr? hr: wr;
//    QRect brect(w/2-mR, 1, 2*mR, h);
//    return brect.size();
//}
//---------------------------------------------------------------------------

void ScalerIndicator::calcPrec()
{
    if (mPrecision >= 0)
        return;
    float num = fabs((mMax - mMin) / (mGraduation * mDivs));
    int order = floor(-log10(num)+1);
    mPrecision = order>0? order: 0;
}
//---------------------------------------------------------------------------
