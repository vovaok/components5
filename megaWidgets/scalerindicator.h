#ifndef SCALERINDICATOR_H
#define SCALERINDICATOR_H

#include <QWidget>
#include <QtGui>

class ScalerIndicator : public QWidget
{
    Q_OBJECT

private:
    float mValue, mValue2;
    bool m2Enabled;
    float mMin, mMax;
    float mGraduation;
    int mDivs;
    float mScaleWidth; // width of scale in degrees
    float mIndicatorAngle;
    QString mUnit;
    float mOrient;

    QImage back, front;
    int mPrecision; // number format precision
    float mR; // scaler radius
//    QRect mNr; // number text rectangle

    QVector<QPointF> mArrow;
    QVector<QPointF> mArrow2;

    void calcPrec();
    void prepareBack();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
//    QSize sizeHint();

public:
    explicit ScalerIndicator(QWidget *parent = 0);

    float value() const {return mValue;}
    void setValue(float val) {mValue = val; repaint();}
    float value2() const {return mValue2;}
    void setValue2(float val) {mValue2 = val; m2Enabled = true; repaint();}
    float minimumValue() const {return mMin;}
    void setMinimumValue(float val) {mMin = val; mPrecision = -1; calcPrec(); repaint();}
    float maximumValue() const {return mMax;}
    void setMaximumValue(float val) {mMax = val; mPrecision = -1; calcPrec(); repaint();}
    void setRange(float minimum, float maximum) {mMin = minimum; mMax = maximum; mPrecision = -1; calcPrec(); repaint();}
    float graduation() const {return mGraduation;}
    int divisions() const {return mDivs;}
    void setGraduation(float step, int divisions=1) {mGraduation = step; mDivs = divisions>0? divisions: 1; mPrecision = -1; calcPrec(); repaint();}
    float scaleWidth() const {return mScaleWidth;}
    void setScaleWidth(float angle_deg) {mScaleWidth = angle_deg; repaint();}
    float indicatorAngle() const {return mIndicatorAngle;}
    void setIndicatorAngle(float angle_deg) {mIndicatorAngle = angle_deg; repaint();}
    QString unit() const {return mUnit;}
    void setUnit(QString unit) {mUnit = unit; repaint();}
    void setPrecision(int digits) {mPrecision = digits; /*calcPrec();*/}
    void setOrient(float angle_deg) {mOrient = angle_deg; repaint();}

    void renderBack(QPainter &p);
    
signals:
    
public slots:
    
};

#endif // SCALERINDICATOR_H
