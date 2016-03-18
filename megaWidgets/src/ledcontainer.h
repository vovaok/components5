#ifndef LEDCONTAINER_H
#define LEDCONTAINER_H

#include <QGroupBox>
#include <QtGui>
#include "led.h"

class LedContainer : public QGroupBox
{
    Q_OBJECT

private:
    QVector<Led*> mLeds;
    QVector<QLabel*> mLabels;
    int mLedSize;
    bool mHorizontal;

protected:

public:
    explicit LedContainer(QWidget *parent = 0);

    void add(QString label=QString(), QColor color=Qt::green);
    void add(QStringList labels);
    void setLed(int number, bool state);

    int ledSize() const {return mLedSize;}
    void setLedSize(int size);

signals:

private slots:
    void onLedClick();

public slots:
    void setData(const void *data);
    void getData(void *data);

    void setLedsClickable(bool enabled);
    void setHorizontal(bool horizontal);
};

#endif // LEDCONTAINER_H
