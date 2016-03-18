#ifndef BUTTON_H
#define BUTTON_H

#include <QWidget>
#include <QPainter>
#include <QVector2D>
#include <QMouseEvent>
#include <math.h>
#include <QDebug>
#include <QFontMetrics>

class Button : public QWidget
{
    Q_OBJECT

private:
    QImage mBack;
    QImage mImg;
    QString mText;
    QColor mColor;
    int mSize;
    bool mPressed;
    bool mEnabled;

    void prepareBack();

    void pressEv(QEvent *e);
    void moveEv(QEvent *e);
    void releaseEv(QEvent *e);

protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

public:
    explicit Button(QString text=QString(), QWidget *parent = 0);

    void setText(QString text);
    void setImage(const QImage &image);
    void setColor(QColor color);

signals:
    void clicked();
    void pressed();
    void released();

public slots:
    void setEnabled(bool enabled);
};

#endif // BUTTON_H
