#ifndef ROTARYSWITCH_H
#define ROTARYSWITCH_H

#include <QWidget>
#include <QtGui>

class RotarySwitch : public QWidget
{
    Q_OBJECT

private:
    QVector<QString> mLabels;
    int mCurPos;
    int mStartAngle;
    float mAngleStep; // desired angle step in degrees

    float mAs; // computed angle step in radians

protected:
    void paintEvent(QPaintEvent *);

public:
    explicit RotarySwitch(QWidget *parent = 0);
    void add(QString position);

    int startAngle() const {return mStartAngle;}
    void setStartAngle(int angle_deg) {mStartAngle = angle_deg; repaint();}
    float angleStep() const {return mAngleStep;}
    void setAngleStep(float step_deg) {mAngleStep = step_deg; repaint();}

    int position() const {return mCurPos;}
    void setPosition(int pos);
    
signals:
    
public slots:
    
};

#endif // ROTARYSWITCH_H
