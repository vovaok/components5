#include "canvaswidget.h"
#include <QPainter>
#include <math.h>

CanvasWidget::CanvasWidget(int width, int height, QWidget *parent)
    : QWidget{parent},
    m_w(width), m_h(height)
{
    setFixedSize(m_w, m_h);
    m_img = QImage(m_w, m_h, QImage::Format_ARGB32_Premultiplied);
    clear();
}

void CanvasWidget::setPixel(int x, int y, QColor c)
{
    m_img.setPixel(x, y, c.rgba());
}

void CanvasWidget::setPixelValue(int x, int y, qreal v)
{
    if (x >= 0 && x < m_w && y >= 0 && y < m_h)
    {
        v = qBound(0.0, v, 1.0);
        QColor c(255*v, 255*v, 255*v);
        m_img.setPixel(x, y, c.rgba());
    }
}

void CanvasWidget::setPixelBipolar(int x, int y, qreal v)
{
    if (x >= 0 && x < m_w && y >= 0 && y < m_h)
    {
        v = qBound(-1.0, v, 1.0);
        QColor c;
        if (v < 0)
            c = QColor(0, round(255 + 255*v), round(-255*v));
        else
            c = QColor(round(255*v), round(255 - 255*v), 0);
        m_img.setPixel(x, y, c.rgba());
    }
}

void CanvasWidget::drawPoint(int x, int y, QColor c)
{
    QPainter p(&m_img);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(c, 3.0));
    p.drawPoint(x, y);
    p.end();
}

void CanvasWidget::drawCircle(int x, int y, QColor c)
{
    QPainter p(&m_img);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(c);
    p.drawEllipse(QPoint(x, y), 5, 5);
    p.end();
}

void CanvasWidget::setImage(const QImage &image)
{
    m_img = image;
    m_w = m_img.width();
    m_h = m_img.height();
    setScale(m_scale);
}

void CanvasWidget::setScale(qreal s)
{
    m_scale = s;
    setFixedSize(m_w * m_scale, m_h * m_scale);
}

void CanvasWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
//    p.translate(m_w/2, m_h/2);
    p.scale(m_scale, m_scale);
//    p.drawImage(-m_img.width()/2, -m_img.height()/2, m_img);
    p.drawImage(0, 0, m_img);
    p.end();
}

