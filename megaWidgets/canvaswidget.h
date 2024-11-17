#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>

class CanvasWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasWidget(int width, int height, QWidget *parent = nullptr);

    void setPixel(int x, int y, QColor c);
    void setPixelValue(int x, int y, qreal v);
    void setPixelBipolar(int x, int y, qreal v);

    void drawPoint(int x, int y, QColor c);
    void drawCircle(int x, int y, QColor c);

    QImage &image() {return m_img;}
    void setImage(const QImage &image);

    void setBackgroundColor(QColor color) {m_backgroundColor = color;}
    void setScale(qreal s);

    void clear() {m_img.fill(m_backgroundColor);}

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QImage m_img;
    QColor m_backgroundColor;
    qreal m_scale = 1.0;
    int m_w, m_h;

signals:

};

#endif // CANVASWIDGET_H
