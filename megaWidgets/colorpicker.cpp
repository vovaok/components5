#include "colorpicker.h"

ColorPicker::ColorPicker(QWidget *parent) :
    QWidget(parent),
    mColor(Qt::white),
    mBrightness(255),
    mSize(33),
    mBulbSize(70),
    mColorRadius(15),
    mx(0), my(0),
    mCaptured(false),
    mCursorCapture(true)
{
    setMinimumSize(32, 32);
    setAttribute(Qt::WA_AcceptTouchEvents);

    pipka = new Pipka(this);
    pipka->setColor(Qt::white);
}

void ColorPicker::setBrightness(int value)
{
    mBrightness = value;
    prepareBack();
    repaint();
}

void ColorPicker::prepareBack()
{
    mBack = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    mBack.fill(Qt::transparent);
    QPainter p(&mBack);
    //QPen defaultPen = p.pen();
    p.translate(width()/2, height()/2);

    p.setRenderHint(QPainter::Antialiasing);

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

    p.setRenderHint(QPainter::Antialiasing, false);

//    p.setPen(Qt::NoPen);
//    p.setBrush(Qt::white);
//    p.drawEllipse(QPoint(0, 0), mColorRadius+1, mColorRadius+1);

    for (int x=-mSize/2; x<=mSize/2; x++)
    {
        for (int y=-mSize/2; y<=mSize/2; y++)
        {
            float len = hypotf(x, y);
            int a = 255;
            if (len > mColorRadius+1)
            {
                a = 255 - (len - (mColorRadius+1)) * 255 / 3;
                if (a < 0)
                    a = 0;
                len = mColorRadius + 1;
            }
            int h = lrintf(360 + atan2f(y, x) * 180 / M_PI) % 360;
            int s = lrintf(len * 255 / (mColorRadius + 1));
            h = h / 10 * 10;
            s = 255 - ((255 - s) / 20 * 21);
            if (s < 0)
                s = 0;
            QColor col = QColor::fromHsv(h, s, mBrightness, a);
            p.setPen(col);
            p.drawPoint(x, y);
            if (x == mx && y == my)
                mColor = col;
            //QRgb c = col.rgba();
            //mBack.setPixel(x+width()/2, y+height()/2, c);
        }
    }

    p.end();
}

void ColorPicker::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
//    p.drawImage(0, 0, mBack);
    p.drawImage(e->rect().topLeft(), mBack, e->rect());
//    p.translate(width()/2, height()/2);
//    p.setRenderHint(QPainter::Antialiasing);

//    QRadialGradient gradj(0.5, 0.5, 0.5, 0.25, 0.25);
//    gradj.setCoordinateMode(QGradient::ObjectBoundingMode);
//    gradj.setColorAt(0.0, Qt::white);
//    gradj.setColorAt(0.3, mColor.lighter());
//    gradj.setColorAt(0.8, mColor);
//    gradj.setColorAt(1.0, mColor.darker());

//    p.setBrush(gradj);
//    p.setPen(Qt::NoPen);

//    p.drawEllipse(QPoint(mx, my), mBulbSize/2, mBulbSize/2);

    p.end();
}

void ColorPicker::resizeEvent(QResizeEvent *e)
{
    mSize = qMin(e->size().width(), e->size().height());
    mBulbSize = mSize / 6;
    mColorRadius = (mSize - mBulbSize) / 2;
    prepareBack();
    pipka->resize(mBulbSize, mBulbSize);
    int w = pipka->width();
    int h = pipka->height();
    pipka->move(mx-w/2+width()/2, my-h/2+height()/2);
}

bool ColorPicker::event(QEvent *e)
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

void ColorPicker::pressEv(QEvent *e)
{
    moveEv(e);
}

void ColorPicker::moveEv(QEvent *e)
{
    QTouchEvent *t = dynamic_cast<QTouchEvent*>(e);
    QMouseEvent *m = dynamic_cast<QMouseEvent*>(e);
    int x = 0, y = 0;
    int gx = 0, gy = 0;
    if (t && !t->touchPoints().isEmpty())
    {
        x = t->touchPoints().first().lastPos().x();
        y = t->touchPoints().first().lastPos().y();
    }
    else if (m)
    {
        x = m->pos().x();
        y = m->pos().y();
        gx = m->globalPos().x();
        gy = m->globalPos().y();
    }
    else
    {
        return;
    }

    if (!mCaptured)
    {
        if (mCursorCapture)
            setCursor(Qt::BlankCursor);
        emit activated();
    }
    mCaptured = true;

    mx = x - width()/2;
    my = y - height()/2;
    int len = (hypotf(mx, my));
    if (len > mColorRadius)
    {
        gx -= mx;
        gy -= my;
        mx = lrintf(mx * mColorRadius / len);
        my = lrintf(my * mColorRadius / len);
        gx += mx;
        gy += my;
        if (mCursorCapture)
            QCursor::setPos(gx, gy);
    }
    mColor = QColor::fromRgba(mBack.pixel(mx+width()/2, my+height()/2));

    int w = pipka->width();
    int h = pipka->height();
    pipka->setColor(mColor);
    pipka->move(mx-w/2+width()/2, my-h/2+height()/2);

    //repaint();
}

void ColorPicker::releaseEv(QEvent *e)
{
    Q_UNUSED(e);
    mCaptured = false;
    if (mCursorCapture)
        setCursor(Qt::ArrowCursor);
    emit picked();
    update();
}

void ColorPicker::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        pressEv(e);
    }
}

void ColorPicker::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        moveEv(e);
    }
}

void ColorPicker::mouseReleaseEvent(QMouseEvent *e)
{
    releaseEv(e);
}
