#ifndef SEGMENTINDICATOR_H
#define SEGMENTINDICATOR_H

#include <QWidget>
#include <QtGui>

class SegmentIndicator : public QWidget
{
    Q_OBJECT

private:
    QColor mBkColor;    // backgroung color
    QColor mLnColor;    // lighted segment color
    QColor mLtColor;    // off segment color
    int mSkew;          // skew of the segments (a la italic)
    int mBevel;         // bevel of the indicator
    int mThickness;     // thickness of the segment

    int mSk, mTh;       // skew and thickness after auto assigning
    int mW, mH;         // width and height of segments area

    QVector<QLine> mSegs;     // segment start and end points
    unsigned char mData;

    QImage mBack;
    void computeParams();
    void generateSegments();
    void prepareBack();

protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *);

public:
    explicit SegmentIndicator(QWidget *parent = 0);

    QColor backColor() const {return mBkColor;}
    void setBackColor(QColor color) {mBkColor = color; computeParams(); generateSegments(); prepareBack();}
    QColor segmentColor() const {return mLnColor;}
    void setSegmentColor(QColor color) {mLnColor = color; computeParams(); generateSegments(); prepareBack();}
    QColor segmentOffColor() const {return mLtColor;}
    void setSegmentOffColor(QColor color) {mLtColor = color; computeParams(); generateSegments(); prepareBack();}
    int skew() const {return mSkew;}
    void setSkew(int skew_px=-1) {mSkew = skew_px; computeParams(); generateSegments(); prepareBack();}
    int bevelSize() const {return mBevel;}
    void setBevelSize(int size_px) {mBevel = size_px; computeParams(); generateSegments(); prepareBack();}
    int segmentThickness() const {return mThickness;}
    void setSegmentThickness(int size_px=-1) {mThickness = size_px; computeParams(); generateSegments(); prepareBack();}
    
    void setSegment(int number, bool state);

signals:
    
public slots:
    void setData(unsigned char byte);
};

#endif // SEGMENTINDICATOR_H
