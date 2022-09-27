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
    int mBytesInColumn;

    void recalcHeight();

protected:

public:
    explicit LedContainer(QWidget *parent = 0);
    LedContainer(const QString &title, QWidget *parent = 0);

    void add(QString label);
    void add(QString label, QColor color);
    void add(QStringList labels);
    void setLed(int number, bool state);
    bool ledState(int number);

    int ledSize() const {return mLedSize;}
    void setLedSize(int size);

    void setBytesInColumn(int value) {mBytesInColumn = value;}

    LedContainer &operator << (QString label) {add(label); return *this;}

signals:
    void ledClicked(int idx);

private slots:
    void onLedClick();

public slots:
    void setData(const void *data);
    void getData(void *data);

    void setLedsClickable(bool enabled);
    void setHorizontal(bool horizontal);
};

#endif // LEDCONTAINER_H
