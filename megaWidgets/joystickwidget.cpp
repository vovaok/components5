#include "joystickwidget.h"

JoystickWidget::JoystickWidget(QWidget *parent) :
    QWidget(parent),
    mColor(Qt::lightGray),//gray),
    mSize(32),
    mStacks(8),//(8),
    mScale(1.0),
//    mPerspective(2),
    r(0.23), R(-0.10),
    mRoundDomain(true),
    mCaptured(false)
{
    setMinimumSize(32, 32);
    setAttribute(Qt::WA_AcceptTouchEvents);
    calcParams();
    createPipki();
}
//---------------------------------------------------------------------------

void JoystickWidget::calcParams()
{
    float s = 0.5 - fabs(R); // s0
//    float p = mPerspective;
//    for (int i=0; i<100; i++)
//        s = 0.5 - fabs(R) * p / (p - sqrtf(1 - s*s));
    mScale = s;
}
//---------------------------------------------------------------------------

void JoystickWidget::resizeEvent(QResizeEvent *e)
{
    mSize = qMin(e->size().width(), e->size().height());
    prepareBack();
    resizePipki();
}

bool JoystickWidget::event(QEvent *e)
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

void JoystickWidget::pressEv(QEvent *e)
{
    moveEv(e);
}

void JoystickWidget::moveEv(QEvent *e)
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
        setCursor(Qt::BlankCursor);
        emit activated();
    }
    mCaptured = true;
    mPos.setX(((float)x - width() / 2.0) / (mSize * mScale));
    mPos.setY((height() / 2.0 - (float)y) / (mSize * mScale));

    if ((mRoundDomain && mPos.lengthSquared() > 1) || (!mRoundDomain && (fabs(mPos.x()) > 1 || fabs(mPos.y()) > 1)))
    {
        float xf = gx - mPos.x() * mSize * mScale; // globalX
        float yf = gy + mPos.y() * mSize * mScale; // globalY
        if (mRoundDomain)
            mPos.normalize();
        else
            mPos /= qMax(fabs(mPos.x()), fabs(mPos.y()));
        xf += mPos.x() * mSize * mScale;
        yf -= mPos.y() * mSize * mScale;
        QCursor::setPos(lrintf(xf), lrintf(yf));
    }
//    if (mAutoRepaint)
//        update();

    updatePipki();
}

void JoystickWidget::releaseEv(QEvent *e)
{
    Q_UNUSED(e);
    mPos = QVector2D(0, 0);
    mCaptured = false;
    setCursor(Qt::ArrowCursor);
    emit deactivated();
    updatePipki();
}

void JoystickWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        pressEv(e);
    }
}

void JoystickWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        moveEv(e);
    }
}

void JoystickWidget::mouseReleaseEvent(QMouseEvent *e)
{
    releaseEv(e);
}
//---------------------------------------------------------------------------

void JoystickWidget::paintEvent(QPaintEvent *e)
{
//    static int cnt = 0;
    //qDebug() << "paint" << cnt++ << e->rect();
    QPainter p(this);
    p.drawImage(e->rect().topLeft(), mBack, e->rect());
//    p.setRenderHint(QPainter::Antialiasing);
//    p.translate(width()/2, height()/2);
//    p.scale(mSize, mSize);

//    QVector2D pos = mPos * mScale;
//    pos.setY(-pos.y());

//    QRadialGradient gradj(0.5, 0.5, 0.5, 0.25, 0.25);
//    gradj.setCoordinateMode(QGradient::ObjectBoundingMode);
//    gradj.setColorAt(0.0, Qt::white);
//    gradj.setColorAt(0.3, mColor.lighter());
//    gradj.setColorAt(0.8, mColor);
//    gradj.setColorAt(1.0, mColor.darker());

//    p.setBrush(gradj);
//    p.setPen(Qt::NoPen);

//    for (int i=0; i<=mStacks; i++)
//    {
//        float k = i / (float)mStacks;
//        float z = sqrtf(1 - pos.lengthSquared()) * k;
//        float kr = mPerspective / (mPerspective - z);
//        float Ri = ((R - r) * k + r) * kr;
//        p.drawEllipse((pos*k).toPointF(), Ri, Ri);
//    }

    p.end();
}


void JoystickWidget::prepareBack()
{
    mBack = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    mBack.fill(Qt::transparent);
    QPainter p(&mBack);
    //QPen defaultPen = p.pen();
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
    p.drawEllipse(QPointF(0, 0), mScale, mScale);

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

    //QTransform trans;
    //trans = trans.rotateRadians(a);

    float dr = (0.5 - mScale) * 0.13;

//    for (int i=0; i<4; i++)
//    {
//        trans.rotateRadians(M_PI/2 * i);
//        QVector<QPointF> pts;
//        pts << trans.map(QPointF(0.5 - dr, 0));
//        pts << trans.map(QPointF(mScale+dr, (0.5 - mScale)/2));
//        pts << trans.map(QPointF(mScale+dr, -(0.5 - mScale)/2));
//        p.drawConvexPolygon(pts);
//    }

    for (int i=0; i<4; i++)
    {
        p.rotate(90*i);
        QVector<QPointF> pts;
        pts << QPointF(0.5 - dr, 0);
        pts << QPointF(mScale+dr, (0.5 - mScale)/2);
        pts << QPointF(mScale+dr, -(0.5 - mScale)/2);
        p.drawConvexPolygon(pts);
    }

    p.end();
}

void JoystickWidget::createPipki()
{
    for (int i=0; i<pipki.size(); i++)
        delete pipki[i];
    pipki.clear();
    for (int i=0; i<=mStacks; i++)
    {
        Pipka *p = new Pipka(this);
        p->setColor(mColor);
        pipki << p;
    }
    resizePipki();
}

void JoystickWidget::resizePipki()
{
    QVector2D pos = mPos * mScale;
    pos.setY(-pos.y());
    for (int i=0; i<=mStacks; i++)
    {
        float k = i / (float)mStacks;
        float Ri = ((R - r) * k + r);
        Ri = fabs(Ri * mSize);
        QVector2D p = pos*k*mSize - QVector2D(Ri, Ri);
        QRect geom(p.x()+width()/2, p.y()+height()/2, Ri*2, Ri*2);
        pipki[i]->setGeometry(geom);
    }
}

void JoystickWidget::updatePipki()
{
    QVector2D pos = mPos * mScale;
    pos.setY(-pos.y());
    for (int i=0; i<=mStacks; i++)
    {
        float k = i / (float)mStacks;
        float Ri = ((R - r) * k + r);
        Ri = fabs(Ri * mSize);
        QVector2D p = pos*k*mSize - QVector2D(Ri, Ri);
        pipki[i]->move(p.x()+width()/2, p.y()+height()/2);
    }
}
//---------------------------------------------------------------------------

void JoystickWidget::setColor(QColor color)
{
    mColor = color;
    for (int i=0; i<pipki.size(); i++)
        pipki[i]->setColor(mColor);
    prepareBack();
    repaint();
}

void JoystickWidget::setRadius(float origin, float end)
{
    r = origin / 2;
    R = end / 2;
    calcParams();
    prepareBack();
    update();
}

//void JoystickWidget::setPerspective(float p)
//{
//    mPerspective = p;
//    calcParams();
//    prepareBack();
//    update();
//}

void JoystickWidget::setStacks(int count)
{
    mStacks = count;
    createPipki();
}

void JoystickWidget::setRoundDomain()
{
    mRoundDomain = true;
    update();
}

void JoystickWidget::setSquareDomain()
{
    mRoundDomain = false;
    update();
}

void JoystickWidget::setPos(float x, float y)
{
    mPos.setX(x);
    mPos.setY(y);
    updatePipki();
}

//void JoystickWidget::setAutoRepaint(bool enabled)
//{
//    mAutoRepaint = enabled;
//}
//---------------------------------------------------------------------------
