#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <QWidget>
#include <QtGui>

class Potentiometer : public QWidget
{
    Q_OBJECT

private:
    float mValue;
    float mAngle;
    QString mText;

    int mR;
    QPointF mC;
    QImage mBack;
    void prepare();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

public:
    explicit Potentiometer(QWidget *parent = 0);

    float value() const {return mValue;}
    void setValue(float val);
    float angle() const {return mAngle;}
    void setAngle(float angle_deg) {mAngle = angle_deg; repaint();}
    QString text() const {return mText;}
    void setText(QString text) {mText = text; prepare(); repaint();}
    
signals:
    
public slots:
    
};

#endif // POTENTIOMETER_H
