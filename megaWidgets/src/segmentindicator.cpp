#include "segmentindicator.h"

SegmentIndicator::SegmentIndicator(QWidget *parent) :
    QWidget(parent),
    mBkColor(Qt::gray),
    mLnColor(Qt::red),
    mLtColor(Qt::lightGray),
    mSkew(-1), // -1 = auto
    mBevel(2),
    mThickness(-1), // -1 = auto
    mData(0)
{
    setMinimumSize(30, 60);
    mLtColor = mBkColor.lighter(140);
    computeParams();
    generateSegments();
    prepareBack();
}
//---------------------------------------------------------------------------

void SegmentIndicator::computeParams()
{
    mSk = mSkew>=0? mSkew: width()/11;
    mW = width()*0.95-mSk*2-mBevel*2;
    mTh = mThickness>=0? mThickness: mW/6; // thickness
    mH = height()*0.6-mBevel*2-mTh;
}

void SegmentIndicator::generateSegments()
{
    mSegs.resize(8);
    mSegs[0] = QLine(1+mTh+mTh/4+mSk*2, 1+mTh/2,    mW-mTh-mTh/4-2+mSk*2, 1+mTh/2   ); // a
    mSegs[1] = QLine(mW-mTh/2-2+mSk*2,  1+mTh/2,    mW-mTh/2-2+mSk,       mH/2-mTh/4); // b
    mSegs[2] = QLine(mW-mTh/2-2+mSk,    mH/2+mTh/4, mW-mTh/2-2+0,         mH-mTh/2-2); // c
    mSegs[3] = QLine(mW-mTh-mTh/4-2+0,  mH-mTh/2-2, 1+mTh+mTh/4+0,        mH-mTh/2-2); // d
    mSegs[4] = QLine(1+mTh/2+0,         mH-mTh/2-2, 1+mTh/2+mSk,          mH/2+mTh/4); // e
    mSegs[5] = QLine(1+mTh/2+mSk,       mH/2-mTh/4, 1+mTh/2+mSk*2,        1+mTh/2   ); // f
    mSegs[6] = QLine(1+mTh+mTh/4+mSk,   mH/2,       mW-mTh-mTh/4-2+mSk,   mH/2      ); // g
    mSegs[7] = QLine(mW-2+mSk*2,        mH-mTh/2-2, mW-2+mSk*2-mSk,       mH+mTh-1  ); // h
    for (int i=0; i<mSegs.count(); i++)
    {
        mSegs[i].translate((width()-(mW+mSk*2))/2, (height()-mH)/2);
    }
}

void SegmentIndicator::prepareBack()
{
    int w = width();
    int h = height();
    mBack = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    QPainter p(&mBack);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), mBkColor);
    for (int i=0; i<1000; i++)
    {
        int x = rand() % width();
        int y = rand() % height();
        int fac = 100 + (rand() % 15);
        QColor c = mBkColor.darker(fac);
        p.setPen(QPen(c, 2));
        p.drawPoint(x, y);
    }
    for (int i=0; i<mBevel-1; i++)
    {
        QColor lc = mBkColor.lighter(100+(mBevel-i)*100/mBevel);
        QColor dc = mBkColor.darker(100+(mBevel-i)*100/mBevel);
        p.setPen(QPen(lc, 1));
        p.drawLine(i, i, w-i-1, i);
        p.drawLine(i, i, i, h-i-1);
        p.setPen(QPen(dc, 1));
        p.drawLine(w-i-1, h-i-1, w-i-1, i);
        p.drawLine(w-i-1, h-i-1, i, h-i-1);
    }
    for (int i=0; i<mSegs.count(); i++)
    {
        p.setPen(QPen(mLtColor, mTh, Qt::SolidLine, Qt::FlatCap));
        p.drawLine(mSegs[i]);
    }
    p.end();
}
//---------------------------------------------------------------------------

void SegmentIndicator::resizeEvent(QResizeEvent *e)
{
    (void)e;
    computeParams();
    generateSegments();
    prepareBack();
}

void SegmentIndicator::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    // restore background
    p.drawImage(0, 0, mBack);
    // draw switched segments
    p.setPen(QPen(mLnColor, mTh*1.5, Qt::SolidLine, Qt::RoundCap));
    for (int i=0; i<8; i++)
    {
        if (mData & (1<<i))
            p.drawLine(mSegs[i]);
    }
    p.end();
}
//---------------------------------------------------------------------------

void SegmentIndicator::setData(unsigned char byte)
{
    mData = byte;
    repaint();
}

void SegmentIndicator::setSegment(int number, bool state)
{
    if (state)
        mData |= (1<<number);
    else
        mData &=~(1<<number);
    repaint();
}
//---------------------------------------------------------------------------
