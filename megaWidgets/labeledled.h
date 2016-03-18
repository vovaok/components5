#ifndef LABELEDLED_H
#define LABELEDLED_H

#include <QtGui>
#include "led.h"

class LabeledLed : public QHBoxLayout
{
    Q_OBJECT

private:
    Led *mLed;
    QLabel *mLabel;

public:
    explicit LabeledLed(QWidget *parent = 0);
    LabeledLed(QString label, QColor color=Qt::red, int size = 15, QWidget *parent = 0L);
    
    Led *led() {return mLed;}
    QLabel *label() {return mLabel;}

signals:
    
public slots:
    
};

#endif // LABELEDLED_H
