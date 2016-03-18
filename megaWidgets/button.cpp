#include "button.h"

Button::Button(QString text, QWidget *parent) :
    QWidget(parent),
    mText(text),
    mColor(Qt::lightGray),
    mSize(0),
    mPressed(false),
    mEnabled(false)
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    mColor = Qt::lightGray;
}
//---------------------------------------------------------

void Button::prepareBack()
{
    mBack = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    mBack.fill(Qt::transparent);
    QPainter p(&mBack);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);

    p.setPen(Qt::darkGray);
    QConicalGradient grad(0, 0, 120);
    grad.setCoordinateMode(QGradient::LogicalMode);
    grad.setColorAt(0.0, QColor(255, 255, 255, 150));
    grad.setColorAt(0.1, QColor(200, 200, 200, 50));
    grad.setColorAt(0.3, QColor(100, 100, 100, 100));
    grad.setColorAt(0.4, QColor(200, 200, 200, 50));
    grad.setColorAt(0.5, QColor(255, 255, 255, 150));
    grad.setColorAt(0.6, QColor(200, 200, 200, 50));
    grad.setColorAt(0.8, QColor(100, 100, 100, 100));
    grad.setColorAt(0.9, QColor(200, 200, 200, 50));
    grad.setColorAt(1.0, QColor(255, 255, 255, 150));
    p.setBrush(grad);
    p.drawEllipse(QPoint(0, 0), mSize/2, mSize/2);

    p.scale(mSize, mSize);

    grad.setAngle(210);
    p.setBrush(grad);
    p.setPen(Qt::NoPen);
#warning 0.8 from the potolok))0)
    p.drawEllipse(QPointF(0, 0), 0.4, 0.4);

    QColor mColor = Qt::blue;

    // arrows
    grad.setCenter(2, 0.5);
    grad.setAngle(120);//a*180/M_PI);
    grad.setCoordinateMode(QGradient::ObjectBoundingMode);//::LogicalMode);
    grad.setColorAt(0.0, mColor.lighter());//QColor(255, 255, 255, 150));
    grad.setColorAt(0.1, mColor);//QColor(200, 200, 200, 50));
    grad.setColorAt(0.3, mColor.darker());//QColor(100, 100, 100, 100));
    grad.setColorAt(0.4, mColor);//QColor(200, 200, 200, 50));
    grad.setColorAt(0.5, mColor.lighter());//QColor(255, 255, 255, 150));
    grad.setColorAt(0.6, mColor);//QColor(200, 200, 200, 50));
    grad.setColorAt(0.8, mColor.darker());//QColor(100, 100, 100, 100));
    grad.setColorAt(0.9, mColor);//QColor(200, 200, 200, 50));
    grad.setColorAt(1.0, mColor.lighter());//QColor(255, 255, 255, 150));
    p.setBrush(grad);

    p.end();
}
//---------------------------------------------------------

void Button::pressEv(QEvent *e)
{
    if (!mEnabled)
        return;
    QTouchEvent *t = dynamic_cast<QTouchEvent*>(e);
    QMouseEvent *m = dynamic_cast<QMouseEvent*>(e);
    int x = t? t->touchPoints().first().lastPos().x(): m? m->x(): 0;
    int y = t? t->touchPoints().first().lastPos().y(): m? m->y(): 0;
    int cx = width() / 2;
    int cy = height() / 2;
    int r = hypotf(x-cx, y-cy);
    if (r <= mSize)
    {
        mPressed = true;
        emit pressed();
    }
}

void Button::moveEv(QEvent *e)
{
    if (!mEnabled)
        return;
    QTouchEvent *t = dynamic_cast<QTouchEvent*>(e);
    QMouseEvent *m = dynamic_cast<QMouseEvent*>(e);
    int x = t? t->touchPoints().first().lastPos().x(): m? m->x(): 0;
    int y = t? t->touchPoints().first().lastPos().y(): m? m->y(): 0;
    int cx = width() / 2;
    int cy = height() / 2;
    int r = hypotf(x-cx, y-cy);
    if (r <= mSize)
    {
        // now button is hovered
    }
}

void Button::releaseEv(QEvent *e)
{
    if (!mEnabled)
        return;
    QTouchEvent *t = dynamic_cast<QTouchEvent*>(e);
    QMouseEvent *m = dynamic_cast<QMouseEvent*>(e);
    int x = t? t->touchPoints().first().lastPos().x(): m? m->x(): 0;
    int y = t? t->touchPoints().first().lastPos().y(): m? m->y(): 0;
    int cx = width() / 2;
    int cy = height() / 2;
    int r = hypotf(x-cx, y-cy);
    if (r <= mSize)
    {
        if (mPressed)
            emit clicked();
    }
    emit released();
    mPressed = false;
}
//---------------------------------------------------------

bool Button::event(QEvent *e)
{
    switch (e->type())
    {
        case QEvent::TouchBegin: pressEv(e); return true;
        case QEvent::TouchUpdate: moveEv(e); return true;
        case QEvent::TouchEnd: releaseEv(e); return true;
        case QEvent::TouchCancel: releaseEv(e); return true;
        default: return QWidget::event(e);
    }
}

void Button::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.drawImage(e->rect().topLeft(), mBack, e->rect());

    p.setRenderHint(QPainter::Antialiasing);
    p.save();
    p.translate(rect().center());
    p.scale(mSize/2, -mSize/2);

    if (mPressed)
    {
        QConicalGradient grad(0.5, 0.5, 240);
        grad.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad.setColorAt(0.0, mColor.lighter(250));
        grad.setColorAt(0.05, mColor.lighter(200));
        grad.setColorAt(0.1, mColor);
        grad.setColorAt(0.5, mColor.darker());
        grad.setColorAt(0.9, mColor);
        grad.setColorAt(0.95, mColor.lighter(200));
        grad.setColorAt(1.0, mColor.lighter(250));

        QRadialGradient grad2(0.5, 0.5, 0.6, 0.65, 0.35);
        grad2.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad2.setColorAt(0, mColor.lighter());
        grad2.setColorAt(0.2, mColor);
        grad2.setColorAt(0.8, mColor.darker());
        grad2.setColorAt(1, mColor.darker(300));

        QPen pen(QPen(QBrush(grad), 0.08));
        p.setPen(pen);
        p.setBrush(grad2);
        p.drawEllipse(QPointF(0, 0), 0.65, 0.65);
    }
    else
    {
        QConicalGradient grad(0.5, 0.5, 240);
        grad.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad.setColorAt(0.0, QColor(255, 255, 255, 255));
        grad.setColorAt(0.05, QColor(255, 255, 255, 255));
        grad.setColorAt(0.1, mColor);
        grad.setColorAt(0.5, mColor.darker());
        grad.setColorAt(0.9, mColor);
        grad.setColorAt(0.95, QColor(255, 255, 255, 255));
        grad.setColorAt(1.0, QColor(255, 255, 255, 255));

        QRadialGradient grad2(0.5, 0.5, 0.6, 0.35, 0.65);
        grad2.setCoordinateMode(QGradient::ObjectBoundingMode);
        grad2.setColorAt(0, mColor.lighter(250));
        grad2.setColorAt(0.2, mColor.lighter());
        grad2.setColorAt(0.8, mColor.lighter(125));
        grad2.setColorAt(1, mColor.darker());

        QPen pen(QPen(QBrush(grad), 0.08));
        p.setPen(pen);
        p.setBrush(grad2);
        p.drawEllipse(QPointF(0, 0), 0.65, 0.65);
        p.setPen(Qt::black);
        p.drawText(QRect(0.2, 0.2, 0.6, 0.6), mText);
    }
    p.restore();

    if (!mText.isEmpty())
    {
        if (mEnabled)
            p.setPen(Qt::black);
        else
            p.setPen(mColor);
        QFont font = p.font();
        QFontMetrics fm(font);
        QSize sz = fm.size(0, mText);
        float szk = qMin(width() / sz.width(), height() / sz.height()) * 0.5;
        font.setPointSizeF(font.pointSizeF() * szk);
        p.setFont(font);
        p.drawText(rect(), Qt::AlignCenter, mText);
    }

    p.end();
}

void Button::resizeEvent(QResizeEvent *e)
{
    mSize = qMin(e->size().width(), e->size().height());
    prepareBack();
}

void Button::mouseMoveEvent(QMouseEvent *e)
{
    moveEv(e);
}

void Button::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        pressEv(e);
    }
}

void Button::mouseReleaseEvent(QMouseEvent *e)
{
    releaseEv(e);
}
//---------------------------------------------------------

void Button::setText(QString text)
{
    if (text == mText)
        return;
    update();
}

void Button::setImage(const QImage &image)
{
    mImg = image;
}

void Button::setColor(QColor color)
{
    if (color != mColor)
    {
        mColor = color;
        prepareBack();
        update();
    }
}
//---------------------------------------------------------

void Button::setEnabled(bool enabled)
{
    if (enabled != mEnabled)
    {
        mEnabled = enabled;
        update();
    }
}
//---------------------------------------------------------
