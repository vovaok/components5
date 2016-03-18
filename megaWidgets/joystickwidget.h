#ifndef JOYSTICKWIDGET_H
#define JOYSTICKWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QVector2D>
#include <QMouseEvent>
#include <math.h>
#include <QDebug>

class JoystickWidget : public QWidget
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
    int mSize;
    int mStacks;
    float mScale;
//    float mPerspective;
    float r, R;
    QVector2D mPos;
    bool mRoundDomain;
    bool mCaptured;
//    bool mAutoRepaint;
    QVector<Pipka*> pipki;

    void calcParams();
    void prepareBack();
    void createPipki();
    void resizePipki();
    void updatePipki();

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
    explicit JoystickWidget(QWidget *parent = 0);

    QVector2D pos() const {return mPos;}
    float x() const {return mPos.x();}
    float y() const {return mPos.y();}
    bool isActive() const {return mCaptured;}
    
    void setColor(QColor color);
    void setRadius(float origin, float end);
//    void setPerspective(float p);
    void setStacks(int count);
    void setRoundDomain();
    void setSquareDomain();

signals:
    void activated();
    void deactivated();
    
public slots:
//    void setAutoRepaint(bool enabled);
};

#endif // JOYSTICKWIDGET_H
