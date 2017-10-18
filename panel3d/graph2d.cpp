#include "graph2d.h"
#include "qpanel3d.h"

const QColor Graph2D::mColors[8] = {Qt::red, Qt::blue, Qt::black, QColor(0, 192, 0), QColor(0, 224, 224), Qt::magenta, QColor(192, 192, 0), Qt::darkGray};

Graph2D::Graph2D(QObject *parent, FrameType frameType) :
    Object3D(parent),
    mFrameType(frameType),
    mCurColor(0),
    mPointsVisible(false),
    mEqualZoom(false),
    mPointLimit(0),
    mGridColor(QColor(128, 128, 128)),
    mAutoBounds(true),
    mWidth(100.0),
    mHeight(100.0),
    mBounds(0, 0, 10, 1),
    mBoundsReload(mBounds),
    mLimitsEnabled(false)
{
}

void Graph2D::setSize(float width, float height)
{
    mWidth = width;
    mHeight = height;
}
//---------------------------------------------------------------------------

void Graph2D::recalculateBounds(float xmin, float xmax, float ymin, float ymax)
{
    if (mFrameType == frameCartesian)
    {
        mBounds.setLeft(xmin);
        mBounds.setRight(xmax);
    }

    if (mFrameType == frameCartesian)
    {
        mBounds.setTop(ymin);
        mBounds.setBottom(ymax);
    }
    else if (mFrameType == framePolar)
    {
        float aymin = fabs(ymin);
        float aymax = fabs(ymax);
        if (aymax < aymin)
            aymax = aymin;
        mBounds.setCoords(-aymax, -aymax, aymax, aymax);
    }

    if (mEqualZoom)
    {
        float w = mBounds.width();
        float h = mBounds.height();
        float d = w - h;
        if (d > 0)
        {
            mBounds.setTop(mBounds.top() - d/2);
            mBounds.setHeight(w);
        }
        else if (d < 0)
        {
            mBounds.setLeft(mBounds.left() + d/2);
            mBounds.setWidth(h);
        }
    }
}
//---------------------------------------------------------------------------

void Graph2D::setBounds(QRectF bounds)
{
    if (bounds.isNull())
    {
#ifdef __GNUC__
        float xmin=INFINITY, xmax=-INFINITY, ymin=INFINITY, ymax=-INFINITY;
#else
        float xmin=INFINITE, xmax=-INFINITE, ymin=INFINITE, ymax=-INFINITE;
#endif
        bool recalc = false;
        // recalculate
        foreach (QString key, mGraphs.keys())
        {
            GraphInfo &g = mGraphs[key];
            foreach (QPointF p, g.graph)
            {
                float px = p.x() * g.zoomX;
                float py = p.y() * g.zoomY;
                if (px == NAN || py == NAN)
                    continue;
                if (xmin > px)
                    xmin = px;
                else if (xmax < px)
                    xmax = px;
                if (ymin > py)
                    ymin = py;
                else if (ymax < py)
                    ymax = py;
                recalc = true;
            }
        }
        if (recalc)
            recalculateBounds(xmin, xmax, ymin, ymax);
    }
    else
    {
        mBounds = bounds;
        mBoundsReload = mBounds;
    }
}

void Graph2D::setAutoBoundsEnable(bool enable)
{
    mAutoBounds = enable;
    if (mAutoBounds)
        setBounds();
}
//---------------------------------------------------------------------------

void Graph2D::addGraph(QString var, QColor color, float lineWidth)
{
    GraphInfo info;
    info.color = color;
    info.zoomX = 1;
    info.zoomY = 1;
    info.width = lineWidth;
    mGraphs[var] = info;
    mGraphNames << var;
}

void Graph2D::addPoint(QString var, float x, float y)
{
    if (!mGraphs.contains(var))
        addGraph(var, mColors[(mCurColor++)%8]);
    GraphInfo &g = mGraphs[var];
    g.graph << QPointF(x, y);
    if (mPointLimit > 0 && g.graph.size() > mPointLimit)
        g.graph.removeFirst();

    if (!mAutoBounds)
        return;

    if (mFrameType == frameCartesian)
    {
        float zx = x * g.zoomX;
        if (zx < mBounds.left())
            mBounds.setLeft(zx);
        if (zx > mBounds.right())
            mBounds.setRight(zx);
    }

    if (y != NAN)
    {
        float zy = y * g.zoomY;
        if (mFrameType == frameCartesian)
        {
            if (zy < mBounds.top())
                mBounds.setTop(zy);
            if (zy > mBounds.bottom())
                mBounds.setBottom(zy);
        }
        else if (mFrameType == framePolar)
        {
            float azy = fabs(zy);
            if (azy > mBounds.right())
            {
                mBounds.setCoords(-azy, -azy, azy, azy);
            }
        }
    }

    if (mEqualZoom)
    {
        float w = mBounds.width();
        float h = mBounds.height();
        float d = w - h;
        if (d > 0)
        {
            mBounds.setTop(mBounds.top() - d/2);
            mBounds.setHeight(w);
        }
        else if (d < 0)
        {
            mBounds.setLeft(mBounds.left() + d/2);
            mBounds.setWidth(h);
        }
    }
}

Graph2D::GraphInfo &Graph2D::info(QString var)
{
    if (!mGraphs.contains(var))
    {
        int idx = (mCurColor++)%8;
        QColor col = mColors[idx];
        addGraph(var, col);
    }
    return mGraphs[var];
}

void Graph2D::clear(QString var)
{
    if (mGraphs.contains(var))
        mGraphs[var].graph.clear();
}

void Graph2D::clear()
{
    foreach (QString key, mGraphs.keys())
        mGraphs[key].graph.clear();
    mBounds = mBoundsReload;
}

void Graph2D::setLimits(float lower, float upper)
{
    mLimitsEnabled = true;
    mLowerLimit = lower;
    mUpperLimit = upper;
}

void Graph2D::setPointLimit(int count)
{
    mPointLimit = count;
    foreach (QString key, mGraphs.keys())
    {
        GraphInfo &g = mGraphs[key];
        int oversize = g.graph.size() - mPointLimit;
        while (oversize-- > 0)
            g.graph.removeFirst();
    }
}

void Graph2D::setGridColor(QColor color)
{
    mGridColor = color;
    emit settingsChanged();
}

void Graph2D::setFont(QFont font)
{
    mFont = font;
}
//---------------------------------------------------------------------------

void Graph2D::draw()
{
    float sx = mBounds.left();
    float sy = mBounds.top();
    float ex = mBounds.right();
    float ey = mBounds.bottom();
    float w = mBounds.width();
    float h = mBounds.height();
    float zx = mWidth / w;
    float zy = mHeight / h;
    sx -= w*0.00001;
    sy -= h*0.00001;
    ex += w*0.00001;
    ey += h*0.00001;

    glDisable(GL_LIGHTING);

    glPushMatrix();
    glScalef(zx, zy, 1.0);

    foreach (QString key, mGraphNames)
    {
        GraphInfo &g = mGraphs[key];
        glLineWidth(g.width);
        glPushMatrix();
        glTranslatef(-sx, -sy, 0);
        if (mFrameType == framePolar)
            glRotatef(90, 0, 0, 1);
        glScalef(g.zoomX, g.zoomY, 1.0);
        glColor3f(g.color.redF(), g.color.greenF(), g.color.blueF());
        glBegin(GL_LINE_STRIP);
        bool beginned = true;
        foreach (QPointF p, g.graph)
        {
            if (p.y() == NAN)
            {
                glEnd();
                beginned = false;
            }
            else
            {
                if (!beginned)
                    glBegin(GL_LINE_STRIP);
                if (mFrameType == frameCartesian)
                    glVertex2f(p.x(), p.y());
                else if (mFrameType == framePolar)
                    glVertex2f(p.y()*cos(p.x()), p.y()*sin(p.x()));
            }
        }
        if (beginned)
            glEnd();
        if (mPointsVisible)
        {
            glPointSize(g.width*4);
            glBegin(GL_POINTS);
            foreach (QPointF p, g.graph)
            {
                if (p.y() != NAN)
                {
                    if (mFrameType == frameCartesian)
                        glVertex2f(p.x(), p.y());
                    else if (mFrameType == framePolar)
                        glVertex2f(p.y()*cos(p.x()), p.y()*sin(p.x()));
                }
            }
            glEnd();
        }
        glPopMatrix();
    }

    // draw grid
    int Nx = 20, Ny = 20;
    float dx = (ex - sx) / Nx;
    if (dx <= 0)
        dx = 1;
    int xOrder = floor(log10(dx));
    double xBase = pow(10, xOrder);
    float xStep = dx / xBase;
    xStep = xStep<=2? 2: xStep<=5? 5: 10;
    dx = xStep * xBase;

    float dy = (ey - sy) / Ny;
    if (dy <= 0)
        dy = 1;
    int yOrder = floor(log10(dy));
    float yBase = pow(10, yOrder);
    float yStep = dy / yBase;
    yStep = yStep<=2? 2: yStep<=5? 5: 10;
    dy = yStep * yBase;

    float bx = ceil(sx/dx) * dx;
    float by = ceil(sy/dy) * dy;

    int phiCount = 36;

    glLineWidth(1.0);
    glColor3f(mGridColor.redF(), mGridColor.greenF(), mGridColor.blueF());

    if (mFrameType == frameCartesian)
    {
        glBegin(GL_LINES);
        for (float yy=by; yy<=ey; yy+=dy)
        {
            glVertex3f(0, yy-sy, -1);
            glVertex3f(w, yy-sy, -1);
        }
        for (float xx=bx; xx<=ex; xx+=dx)
        {
            glVertex3f(xx-sx, 0, -1);
            glVertex3f(xx-sx, h, -1);
        }
        glEnd();
    }
    else if (mFrameType == framePolar)
    {
        glPushMatrix();
        glTranslatef(-sx, -sy, 0);
        for (float ro=0; ro<=ey; ro+=dy)
        {
            glBegin(GL_LINE_STRIP);
            for (int i=0; i<=60; i++)
                glVertex2f(ro*cos(i*2*M_PI/60), ro*sin(i*2*M_PI/60));
            glEnd();
        }
        glBegin(GL_LINES);
        for (int i=0; i<phiCount; i++)
        {
            glVertex2f(0, 0);
            glVertex2f(ey*cos(i*2*M_PI/phiCount), ey*sin(i*2*M_PI/phiCount));
        }
        glEnd();
        if (mLimitsEnabled)
        glColor3f(0.5, 1.0, 0.5);
        glTranslatef(0, 0, -1);
        GLUquadricObj *quad = gluNewQuadric();
        gluDisk(quad, mLowerLimit, mUpperLimit, 60, 1);
        gluDeleteQuadric(quad);
        glPopMatrix();
    }

    QFont font = mFont;
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    int fontsz = mFont.pixelSize();
    font.setPixelSize(qMin(vp[2], vp[3]) * fontsz / (40 * 16));
    //QFontMetricsF fm(font, scene());

//    QColor backColor = scene()->backColor();
//    float Y = 0.299*backColor.redF() + 0.587*backColor.greenF() + 0.114*backColor.blueF();
//    if (Y > 0.5)
//        glColor3f(0, 0, 0);
//    else
//        glColor3f(1, 1, 1);
    if (mFrameType == frameCartesian)
    {
        for (float yy=by; yy<=ey; yy+=dy)
        {
            if (fabs(yy) < 0.000001)
                yy = 0;
            QString yt = QString::number(yy, 'g', 6);
            yt = yt.replace('.', ',');
            glColor3f(0, 0, 0);
            scene()->renderText(-w/30, yy-sy, 1, yt, font);
        }
        for (float xx=bx; xx<=ex; xx+=dx)
        {
            if (fabs(xx) < 0.000001)
                xx = 0;
            QString xt = QString::number(xx, 'g', 6);
            xt = xt.replace('.', ',');
            glColor3f(0, 0, 0);
            scene()->renderText(xx-sx, 0 - h/40, 1, xt, font);
        }
    }
    else if (mFrameType == framePolar)
    {
        for (float yy=0; yy<=ey; yy+=dy)
        {
            if (fabs(yy) < 0.000001)
                yy = 0;
            QString yt = QString::number(yy, 'g', 6);
            yt = yt.replace('.', ',');
//            scene()->renderText(yy-sy, h/2+1, 1, yt, font);
            glColor3f(0, 0, 0);
            scene()->renderText(w/2, yy-sy, 1, yt, font);
        }
        for (int i=0; i<phiCount; i++)
        {
            QString fit = QString::number(i*360.0/phiCount) + "°";
            glColor3f(0, 0, 0);
            scene()->renderText(-ey*sin(i*2*M_PI/phiCount)-sx, ey*cos(i*2*M_PI/phiCount)-sy, 1, fit, font);
        }
    }

    glPopMatrix(); // scale

    int nx = 0;
    foreach (QString key, mGraphNames)
    {
        GraphInfo &g = mGraphs[key];
        glColor3f(g.color.redF(), g.color.greenF(), g.color.blueF());
        QString text = key;
        if (g.zoomY != 1)
            text += "*" + QString::number(g.zoomY, 'g', 6);
        text += "; ";
        scene()->renderText(nx, mHeight*1.01, 1, text, font);
        nx += mWidth / 5;
        if (nx > mWidth)
            break;
    }
    glColor3f(0, 0, 0);
    scene()->renderText(mWidth-mLabelX.length(), 0-mHeight/20, 1, mLabelX, font);

    glEnable(GL_MULTISAMPLE);
}

QVector3D Graph2D::getMinBounds()
{
    return QVector3D(0, 0, 0);
}

QVector3D Graph2D::getMaxBounds()
{
    return QVector3D(mWidth, mHeight, 0);
}
//---------------------------------------------------------------------------
