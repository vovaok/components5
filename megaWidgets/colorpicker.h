#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>
#include <QPainter>
#include <QVector2D>
#include <QMouseEvent>
#include <math.h>
#include <QDebug>

class ColorPicker : public QWidget
{
    Q_OBJECT

private:
    class Pipka : public QWidget
    {
    private:
        QColor mColor;
    protected:
        void paintEvent(QPaintEvent *)
        {
            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing);

            QRadialGradient gradj(0.5, 0.5, 0.5, 0.25, 0.25);
            gradj.setCoordinateMode(QGradient::ObjectBoundingMode);
            gradj.setColorAt(0.0, Qt::white);
            gradj.setColorAt(0.3, mColor.lighter());
            gradj.setColorAt(0.8, mColor);
            gradj.setColorAt(1.0, mColor.darker());

            p.setBrush(gradj);
            p.setPen(Qt::NoPen);

            p.drawEllipse(rect());
            p.end();
        }

    public:
        Pipka(QWidget *parent=0L) :
            QWidget(parent)
        {
            resize(32, 32);
        }

        void setColor(QColor color)
        {
            if (color != mColor)
            {
                mColor = color;
                repaint();
            }
        }
    };

private:
    QImage mBack;
    QColor mColor;
    int mBrightness;
    int mSize;
    int mBulbSize;
    int mColorRadius;
    int mx, my;
    bool mCaptured;
    bool mCursorCapture;
    Pipka *pipka;

    void prepareBack();

    void pressEv(QEvent *e);
    void moveEv(QEvent *e);
    void releaseEv(QEvent *e);

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    bool event(QEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

public:
    explicit ColorPicker(QWidget *parent = 0);

    QColor color() const {return mColor;}
    int red() const {return mColor.red();}
    int green() const {return mColor.green();}
    int blue() const {return mColor.blue();}
    bool isActive() const {return mCaptured;}
    
signals:
    void activated();
    void picked();
    
public slots:
    void setBrightness(int value);
};

#endif // COLORPICKER_H
