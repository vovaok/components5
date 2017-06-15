#ifndef LED_H
#define LED_H

#include <QtWidgets>

class Led : public QWidget
{
    Q_OBJECT

public:
    typedef enum
    {
        NoFlags             = 0x00,
        Clickable           = 0x01,
        InvisibleWhenOff    = 0x02
    } LedFlag;
    Q_DECLARE_FLAGS(LedFlags, LedFlag)

private:
    QColor mColor;
    bool mState;
    LedFlags mFlags;

    QImage mOffImg, mOnImg;
    void prepareImg();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);

public:
    explicit Led(QWidget *parent = 0);
    Led(QColor col, LedFlags flags=NoFlags, bool on=false, QWidget *parent=0L);

    QColor color() const {return mColor;}
    void setColor(QColor col) {mColor = col; prepareImg(); repaint();}

    bool state() const {return mState;}
    void setState(bool on);

    void setFlags(LedFlags flags);
    
signals:
    void clicked();
    
public slots:
    
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Led::LedFlags)

#endif // LED_H
