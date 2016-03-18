#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

#include <QWidget>
#include <QtGui>

class ToggleSwitch : public QWidget
{
    Q_OBJECT

private:
    bool mState;
    QString mOffText, mOnText;

    int mR;
    QImage mBack;
    void prepare();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

public:
    explicit ToggleSwitch(QWidget *parent = 0);

    bool state() const {return mState;}
    void setState(bool on);
    QString offText() const {return mOffText;}
    QString onText() const {return mOnText;}
    void setOffText(QString text) {mOffText = text; prepare(); repaint();}
    void setOnText(QString text) {mOnText = text; prepare(); repaint();}
    void setText(QString offText, QString onText) {mOffText = offText; mOnText = onText; prepare(); repaint();}
    
signals:
    
public slots:
    
};

#endif // TOGGLESWITCH_H
