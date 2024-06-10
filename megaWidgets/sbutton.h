#ifndef SBUTTON_H
#define SBUTTON_H

#include <QAbstractButton>
//#include <QtGui>

class SButton : public QAbstractButton
{
    Q_OBJECT

private:
    bool mState;
    QColor mColor;
    QString mText;

    int mR;
    QPointF mC;
    QImage mBack;
    void prepare();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

public:
    explicit SButton(QWidget *parent = 0);
    
    bool state() const {return mState;}
    void setState(bool on);
    QColor color() const {return mColor;}
    void setColor(QColor col) {mColor = col; prepare(); repaint();}
    QString text() const {return mText;}
    void setText(QString text) {mText = text; prepare(); repaint();}

signals:
    
public slots:
    
};

#endif // SBUTTON_H
