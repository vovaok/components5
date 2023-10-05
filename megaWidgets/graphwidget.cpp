#include "graphwidget.h"

GraphWidget::GraphWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    m_vshader(nullptr),
    m_fshader(nullptr),
    m_program(nullptr),
    mMaxPointCount(16384),
    mPointCount(0),
    xMin(0), xMax(1),
    yMin(0), yMax(1),
    xMin0(0), xMax0(0),
    yMin0(0), yMax0(0),
    xWindow(0),
    mAutoZoom(true),
    mInitialized(false),
    m_innocent(true),
    m_pointsVisible(false)
{
    resetBounds();
}

GraphWidget::~GraphWidget()
{
    // And now release all OpenGL resources.
    makeCurrent();
//    delete m_texture;
    delete m_program;
    delete m_vshader;
    delete m_fshader;
    foreach (QString s, mGraphs.keys())
        mGraphs[s].vbo.destroy();
    m_grid_vbo.destroy();
    doneCurrent();
}

void GraphWidget::addGraph(QString name, QColor color, float lineWidth)
{
    makeCurrent();
    if (!mGraphNames.contains(name))
        mGraphNames << name;
    Graph &g = mGraphs[name];
    if (mInitialized)
        g.initialize(mMaxPointCount);
    g.color = color;
    g.lineWidth = lineWidth;
}

void GraphWidget::removeGraph(QString name)
{
    if (mGraphs.contains(name))
    {
        makeCurrent(); // na vsyakiy
        mGraphs[name].vbo.destroy();
        doneCurrent();
        mGraphNames.removeOne(name);
        mGraphs.remove(name);
    }
}

Graph *GraphWidget::graph(QString name)
{
    return mGraphs.contains(name)? &(mGraphs[name]): nullptr;
}

void GraphWidget::addPoint(QString name, float x, float y)
{    
    if (m_innocent)
    {
        m_innocent = false;
        xMin0 = xMax0 = x;
        yMin0 = yMax0 = y;
    }

    if (xMin0 > x)
        xMin0 = x;
    if (xMax0 < x)
        xMax0 = x;
    if (yMin0 > y)
        yMin0 = y;
    if (yMax0 < y)
        yMax0 = y;

    if (mAutoZoom)
    {
        xMin = xMin0;
        xMax = xMax0;
        yMin = yMin0;
        yMax = yMax0;
    }

    if (!mGraphNames.contains(name))
        addGraph(name);

    mGraphs[name].addPoint(x, y);
}

void GraphWidget::setGraphVisible(QString name, bool visible)
{
    if (mGraphs.contains(name))
        mGraphs[name].visible = visible;
}

void GraphWidget::resetBounds()
{
    xMin = yMin = 0;
    xMax = yMax = 1;
    xMin0 = xMax0 = yMin0 = yMax0 = 0;
    m_innocent = true;
}

void GraphWidget::setBounds(float xmin, float ymin, float xmax, float ymax)
{
    xMin = xmin;
    yMin = ymin;
    xMax = xmax;
    yMax = ymax;

    xMin0 = xMax0 = yMin0 = yMax0 = 0;
    m_innocent = true;
}

void GraphWidget::clear()
{
    foreach (QString s, mGraphs.keys())
        mGraphs[s].clear();
    m_innocent = true;
}

void GraphWidget::clear(QString name)
{
    if (mGraphs.contains(name))
        mGraphs[name].clear();
}

void GraphWidget::setXlabel(QString label)
{
    mXlabel = label;
}

void GraphWidget::setGraphType(QString name, Graph::Type type)
{
    if (mGraphs.contains(name))
        mGraphs[name].appearance = type;
}

void GraphWidget::setPointSize(QString name, float size)
{
    if (mGraphs.contains(name))
        mGraphs[name].pointSize = size;
}

void GraphWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    m_vshader = new QOpenGLShader(QOpenGLShader::Vertex);
    const char *vsrc1 =
        "attribute highp vec4 vertex;\n"
        "uniform mediump mat4 matrix;\n"
        "uniform mediump vec4 lineColor;\n"
        "varying mediump vec4 color;\n"
        "varying mediump vec3 pos;\n"
        "uniform mediump float pointSize;\n"
        "void main(void)\n"
        "{\n"
        "    color = lineColor;\n"
//        "    float logx = 0.4342944819 * log(vertex.x);\n"
//        "    vec4 v = vec4(logx, vertex.yzw);\n"
        "    gl_Position = matrix * vertex;\n"
        "    gl_PointSize = pointSize;\n"
        "    pos = vertex.xyz;\n"
        "}\n";
    m_vshader->compileSourceCode(vsrc1);

    m_fshader = new QOpenGLShader(QOpenGLShader::Fragment);
    const char *fsrc1 =
        "varying mediump vec4 color;\n"
        "varying mediump vec3 pos;\n"
        "uniform mediump float xmin;\n"
        "uniform mediump float xmax;\n"
        "uniform mediump float ymin;\n"
        "uniform mediump float ymax;\n"
        "void main(void)\n"
        "{\n"
        "   if (pos.x < xmin || pos.x > xmax)"
        "       discard;\n"
        "   if (pos.y < ymin || pos.y > ymax)"
        "       discard;\n"
        "    gl_FragColor = color;\n"
        "}\n";
    m_fshader->compileSourceCode(fsrc1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShader(m_vshader);
    m_program->addShader(m_fshader);
    m_program->link();

    m_vertexAttr = m_program->attributeLocation("vertex");
    m_matrixUniform = m_program->uniformLocation("matrix");
    m_lineColorUniform = m_program->uniformLocation("lineColor");
    m_pointSizeUniform = m_program->uniformLocation("pointSize");
    m_xminUniform = m_program->uniformLocation("xmin");
    m_xmaxUniform = m_program->uniformLocation("xmax");
    m_yminUniform = m_program->uniformLocation("ymin");
    m_ymaxUniform = m_program->uniformLocation("ymax");

    //qDebug() << "create grid";
    m_grid_vbo.create();
    m_grid_vbo.bind();
    gridBuf.clear();
    int xCnt = 20;
    int yCnt = 20;
    for (int i=0; i<=xCnt; i++)
    {
        gridBuf.append(-1.0 + 2 * i / (float)xCnt);
        gridBuf.append(-1.0);
        gridBuf.append(-1.0 + 2 * i / (float)xCnt);
        gridBuf.append(1.0);
    }
    for (int i=0; i<=yCnt; i++)
    {
        gridBuf.append(-1.0);
        gridBuf.append(-1.0 + 2 * i / (float)yCnt);
        gridBuf.append(1.0);
        gridBuf.append(-1.0 + 2 * i / (float)yCnt);
    }

    m_grid_vbo.allocate(gridBuf.constData(), gridBuf.count() * sizeof(GLfloat));
    m_grid_vbo.release();

//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LEQUAL);
//    glDepthMask(true);

////    glEnable(GL_CULL_FACE);
////    glCullFace(GL_BACK);

//#if defined(WIN32)
//    glEnable(GL_ALPHA_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
////    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
////    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);


//    glEnable(GL_POINT_SPRITE);
//#endif

    mInitialized = true;

    foreach (QString s, mGraphs.keys())
    {
        //qDebug() << "create graph for" << s;
        mGraphs[s].initialize(mMaxPointCount);
    }
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousePos = event->pos();
    event->accept();
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint dpos = event->pos() - m_mousePos;
        m_mousePos = event->pos();
        float dx = (xMinW - xMaxW) * dpos.x() / width();
        float dy = (yMaxW - yMinW) * dpos.y() / height();
        if (!m_innocent)
        {
            dx = qMax(dx, xMin0 - xMin);
            dx = qMin(dx, xMax0 - xMax);
            dy = qMax(dy, yMin0 - yMin);
            dy = qMin(dy, yMax0 - yMax);
        }
        xMin += dx;
        xMax += dx;
        yMin += dy;
        yMax += dy;

        event->accept();
        emit boundsChanged();
        update();
    }
}

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    float delta = -event->angleDelta().y() / 120.0;
    float k = delta * 0.1;
    m_mousePos = event->pos();
    float xpos = xMinW + (xMaxW - xMinW) * event->x() / (float)width();
    float ypos = yMaxW - (yMaxW - yMinW) * event->y() / (float)height();
    if (!(event->modifiers() & Qt::ShiftModifier))
    {
        xMin -= (xpos - xMin) * k;
        xMax += (xMax - xpos) * k;
    }
    if ((event->modifiers() & Qt::ShiftModifier) || (event->modifiers() & Qt::ControlModifier))
    {
        yMin -= (ypos - yMin) * k;
        yMax += (yMax - ypos) * k;
    }

    if (!m_innocent)
    {
        xMin = qMax(xMin0, xMin);
        xMax = qMin(xMax0, xMax);
        yMin = qMax(yMin0, yMin);
        yMax = qMin(yMax0, yMax);
    }

    event->accept();
    emit boundsChanged();
    update();
}

QString super(int n)
{
    const static QString d("⁰¹²³⁴⁵⁶⁷⁸⁹");
    QString s;
    char buf[16];
    sprintf(buf, "%d", n);
    char *p = buf;
    for (; *p; p++)
    {
        if (*p == '-')
            s.append(L'⁻');
        else
            s.append(d[*p - '0']);
    }
    return s;
}

void GraphWidget::paintGL()
{
    QPainter painter;
    painter.begin(this);

    painter.setFont(mFont);
    QFontMetrics fm(mFont);
    int fh = fm.height();
    int fwmax = fm.width("999999");

    painter.beginNativePainting();

    glEnable(GL_MULTISAMPLE);

    glClearColor(mBackColor.redF(), mBackColor.greenF(), mBackColor.blueF(), mBackColor.alphaF());
    //glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

//    glFrontFace(GL_CW);
//    glCullFace(GL_FRONT);
//    glEnable(GL_CULL_FACE);
//    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA_SATURATE);

    if (mAutoZoom && xWindow > 0 && xMin < xMax - xWindow)
        xMin = (xMax - xWindow);

    // draw grid
    int Nx = 20, Ny = 20;
    float dx = (xMax - xMin) / Nx;
    if (dx <= 0)
        dx = 1;
    int xOrder = floor(log10(dx));
    double xBase = pow(10, xOrder);
    float xStep = dx / xBase;
    xStep = xStep<=2? 2: xStep<=5? 5: 10;
    dx = xStep * xBase;

    float dy = (yMax - yMin) / Ny;
    if (dy <= 0)
        dy = 1;
    int yOrder = floor(log10(dy));
    float yBase = pow(10, yOrder);
    float yStep = dy / yBase;
    yStep = yStep<=2? 2: yStep<=5? 5: 10;
    dy = yStep * yBase;

    float bx = ceil(xMin/dx) * dx;
    float by = ceil(yMin/dy) * dy;

    gridBuf.clear();
    for (float yy=by; yy<=yMax+0.0001; yy+=dy)
    {
        gridBuf.append(xMin);
        gridBuf.append(yy);
        gridBuf.append(xMax);
        gridBuf.append(yy);
//        glVertex3f(0, yy-sy, -1);
//        glVertex3f(w, yy-sy, -1);
    }
    for (float xx=bx; xx<=xMax+0.0001; xx+=dx)
    {
        gridBuf.append(xx);
        gridBuf.append(yMin);
        gridBuf.append(xx);
        gridBuf.append(yMax);
//        glVertex3f(xx-sx, 0, -1);
//        glVertex3f(xx-sx, h, -1);
    }

    QMatrix4x4 modelview;
    float xAdd = (xMax - xMin) * fwmax / width();
    float yAdd = (yMax - yMin) * (fh + 2) / height();
    xMinW = xMin - xAdd;
    xMaxW = xMax + xAdd;
    yMinW = yMin - yAdd;
    yMaxW = yMax + yAdd;
    modelview.ortho(xMinW, xMaxW, yMinW, yMaxW, 0, 1);
//    modelview.rotate(m_fAngle, 0.0f, 1.0f, 0.0f);
//    modelview.rotate(m_fAngle, 1.0f, 0.0f, 0.0f);
//    modelview.rotate(m_fAngle, 0.0f, 0.0f, 1.0f);
//    modelview.scale(m_fScale);
//    modelview.translate(0.0f, -0.2f, 0.0f);

    m_program->bind();
    m_program->setUniformValue(m_matrixUniform, modelview);
    m_program->setUniformValue(m_xminUniform, xMin);
    m_program->setUniformValue(m_xmaxUniform, xMax);
    m_program->setUniformValue(m_yminUniform, yMin);
    m_program->setUniformValue(m_ymaxUniform, yMax);

    m_program->enableAttributeArray(m_vertexAttr);

    foreach (QString s, mGraphNames)
    {
        Graph &g = mGraphs[s];
        if (!g.visible)
            continue;
        m_program->setUniformValue(m_lineColorUniform, g.color);//.redF(), g.color.greenF(), g.color.blueF(), g.color.alphaF());
        glLineWidth(g.lineWidth);
        m_program->setUniformValue(m_pointSizeUniform, g.pointSize);
        g.vbo.bind();
        g.writeBuf();
        m_program->setAttributeBuffer(m_vertexAttr, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
        GLenum mode = GL_LINE_LOOP;
        if (g.appearance == Graph::Line)
            mode = GL_LINE_LOOP;
        else if (g.appearance == Graph::Points)
            mode = GL_POINTS;
        if (!mPointCount)
        {
            //glDrawArrays(GL_LINE_STRIP, g.pointCount, g.vboSize - g.pointCount);
            glDrawArrays(mode, 0, qMin(g.pointCount, g.vboSize)); // vertices count
        }
        else
        {
            int cnt = qMin(g.pointCount, mPointCount);
            glDrawArrays(mode, g.pointCount - cnt, cnt); // vertices count
        }
        g.vbo.release();
    }

//    glLineWidth(2.0f);
//    m_vbo.bind();
//    m_program->setAttributeBuffer(m_vertexAttr, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
//    glDrawArrays(GL_LINE_STRIP, 0, mPointCount); // vertices count
//    m_vbo.release();

    // normal blending for grid
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(1.0f);
    m_program->setUniformValue(m_lineColorUniform, mGridColor);
    m_grid_vbo.bind();
    m_grid_vbo.write(0, gridBuf.constData(), gridBuf.count() * sizeof(GLfloat));
    m_program->setAttributeBuffer(m_vertexAttr, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
    glDrawArrays(GL_LINES, 0, gridBuf.count()/2); // vertices count
    m_grid_vbo.release();

    m_program->disableAttributeArray(m_vertexAttr);

    m_program->release();


//    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);

    painter.endNativePainting();

    // draw 2D

//    painter.setRenderHint(QPainter::Antialiasing);

    QPointF P(1, 1);// = modelview.inverted().map(QPointF(xMin, yMin));

    QMatrix4x4 devm;
    devm.ortho(0, width(), height(), 0, 0, 1);
    QMatrix4x4 comb = devm.inverted() * modelview;
    m_viewTransform = comb;

    P = comb.map(P);

//    painter.drawLine(QPointF(0, 0), P);
//    painter.drawText(P, "0");

    static QStringList pre({"п", "н", "мк", "м", "", "к", "М", "Г", "Т"});

//    float xavg = qMax(fabs(xMax), fabs(xMin)) / 20;
//    if (qFuzzyIsNull(xavg))
//        xavg = 1;

    int ex = floorf(log10f(dx));
    int exi = qBound(-4, ex / 3, 4) * 3;
    float zex = powf(10.f, -exi);
    ex -= exi;
    int epx = qMax(0, -ex);
    QString prex = pre[exi/3 + 4];

    int ey = floorf(log10f(dy));
    int eyi = qBound(-4, ey / 3, 4) * 3;
    float zey = powf(10.f, -eyi);
    ey -= eyi;
    int epy = qMax(0, -ey);
    QString prey = pre[eyi/3 + 4];



    for (float yy=by; yy<=yMax+0.0001; yy+=dy)
    {
        float y1 = yy * zey;
        QPointF P = comb.map(QPointF(xMin, yy));
        QString s = QString::number(y1, 'f', epy).replace('.',',') + prey;
        if (qFuzzyIsNull(y1))
            s = "0";
        int fw = fm.width(s);
        painter.drawText(P + QPointF(-fw-4, fh/2-2), s);
    }
    for (float xx=bx; xx<=xMax+0.0001; xx+=dx)
    {
        float x1 = xx * zex;
        QPointF P = comb.map(QPointF(xx, yMin));
        QString s = QString::number(x1, 'f', epx).replace('.',',');// + prex; // in label
        if (qFuzzyIsNull(x1))
            s = "0";
        int fw = fm.width(s);
        painter.drawText(P + QPointF(-fw/2, fh-3), s);
    }

    QPointF Ps = comb.map(QPointF(xMax, yMin)) + QPointF(2, 0);
    QString xlab;
    if (mXlabel.isEmpty())
        xlab = QString("∙10") + super(exi);
    else
        xlab = prex + mXlabel;
    painter.drawText(Ps, xlab);

    Ps = comb.map(QPointF(xMin, yMax)) + QPointF(0, -4);
    foreach (QString s, mGraphNames)
    {
        if (!mGraphs[s].visible)
            continue;
        int fw = fm.width(s);
        painter.setPen(mGraphs[s].color);
        painter.drawText(Ps, s);
        Ps += QPointF(fw + 16, 0);
    }

    painter.end();
}

//void GraphWidget::resizeGL(int w, int h)
//{

//}
//---------------------------------------------------------------------------


Graph::Graph() :
    vboSize(0),
    curIdx(0),
    pointCount(0),
    lineWidth(1.0f),
    pointSize(3.0f),
    _initialized(false),
    visible(true)
{

}

void Graph::initialize(int maxPointCount)
{
    if (_initialized)
        return;
    if (maxPointCount)
        vboSize = maxPointCount;
    else
        vboSize = 16384;
    vbo.create();
    vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vbo.bind();
    vbo.allocate(vboSize * 2 * sizeof(GLfloat));
    GLfloat *buf = reinterpret_cast<GLfloat*>(vbo.mapRange(0, vboSize * 2 * sizeof(GLfloat), QOpenGLBuffer::RangeWrite));
    for (int i=0; i<vboSize * 2; i++)
        buf[i] = NAN;
    vbo.unmap();
    vbo.release();
    _initialized = true;
}

void Graph::addPoint(float x, float y)
{
    if (bounds.left() > x)
        bounds.setLeft(x);
    if (bounds.right() < x)
        bounds.setRight(x);
    if (bounds.top() > y)
        bounds.setTop(y);
    if (bounds.bottom() < y)
        bounds.setBottom(y);
    pointBuffer << x << y;
}

void Graph::clear()
{
    pointBuffer.clear();
    pointCount = 1;
    curIdx = 1;
    bounds = QRectF();
}

void Graph::writeBuf()
{
    // vbo should have been bound to current context

//    if (pointBuffer.size() > vboSize * 2)
//    {
//        QOpenGLBuffer tempvbo;
//        tempvbo.create();
//        tempvbo.bind();
//        tempvbo.allocate(vboSize * 2 * sizeof(GLfloat) * 2);
//        GLfloat *buf = reinterpret_cast<GLfloat*>(vbo.map(QOpenGLBuffer::ReadOnly));
//        GLfloat *tempbuf = reinterpret_cast<GLfloat*>(tempvbo.mapRange(0, vboSize*2*sizeof(GLfloat), QOpenGLBuffer::RangeWrite));
//        memcpy(tempbuf, buf, vboSize*2*sizeof(GLfloat));
//        tempvbo.unmap();
//        vbo.unmap();
//        tempvbo.release();
//        vbo.release();
//        vbo.destroy();
//        vbo = tempvbo;
//    }

    if (pointBuffer.size())
    {
        pointBuffer << NAN << NAN;
        if (curIdx > 0)
            --curIdx;
        else
            curIdx = vboSize - 1;

        if (pointCount)
            --pointCount;
    }

    while (pointBuffer.size())
    {
        int size = qMin(pointBuffer.size(), (vboSize - curIdx) * 2);

//        vbo.write(pointCount*2*sizeof(GLfloat), pointBuffer.data(), size*sizeof(GLfloat));
        // tipa tak faster than write:
        GLfloat *buf = reinterpret_cast<GLfloat*>(vbo.mapRange(curIdx*2*sizeof(GLfloat), size*sizeof(GLfloat), QOpenGLBuffer::RangeWrite));
        memcpy(buf, pointBuffer.data(), size*sizeof(GLfloat));
        vbo.unmap();

        curIdx += size / 2;
        pointCount += size / 2;
        if (curIdx >= vboSize)
            curIdx = 0;
        pointBuffer.remove(0, size);
    }
}
//---------------------------------------------------------------------------
