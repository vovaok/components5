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
    mAutoZoom(true),
    mInitialized(false)
{
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
    mGraphNames << name;
    Graph &g = mGraphs[name];
    if (mInitialized)
        g.initialize(mMaxPointCount);
    g.color = color;
    g.lineWidth = lineWidth;
}

void GraphWidget::addPoint(QString name, float x, float y)
{
    if (mAutoZoom)
    {
        if (xMin > x)
            xMin = x;
        if (xMax < x)
            xMax = x;
        if (yMin > y)
            yMin = y;
        if (yMax < y)
            yMax = y;
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

void GraphWidget::setBounds(float xmin, float ymin, float xmax, float ymax)
{
    xMin = xmin;
    yMin = ymin;
    xMax = xmax;
    yMax = ymax;
}

void GraphWidget::clear()
{
    foreach (QString s, mGraphs.keys())
        mGraphs[s].clear();
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

void GraphWidget::initializeGL()
{
    initializeOpenGLFunctions();

//    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    m_vshader = new QOpenGLShader(QOpenGLShader::Vertex);
    const char *vsrc1 =
        "attribute highp vec4 vertex;\n"
        "uniform mediump mat4 matrix;\n"
        "uniform mediump vec4 lineColor;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    color = lineColor;\n"    //vec4(lineColor, 1.0);\n"
//        "    color = clamp(color, 0.0, 1.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n";
    m_vshader->compileSourceCode(vsrc1);

    m_fshader = new QOpenGLShader(QOpenGLShader::Fragment);
    const char *fsrc1 =
        "varying mediump vec4 color;\n"
//        "uniform mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n";
    m_fshader->compileSourceCode(fsrc1);

//    m_vshader = new QOpenGLShader(QOpenGLShader::Vertex);
//    const char *vsrc1 =
//        "attribute highp vec4 vertex;\n"
//        "attribute mediump vec3 normal;\n"
//        "uniform mediump mat4 matrix;\n"
//        "varying mediump vec4 color;\n"
//        "void main(void)\n"
//        "{\n"
//        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
//        "    float angle = max(dot(normal, toLight), 0.0);\n"
//        "    vec3 col = vec3(1.0, 0.5, 0.0);\n"
//        "    color = vec4(col, 1.0);// * 0.2 + col * 0.8 * angle, 1.0);\n"
//        "    color = clamp(color, 0.0, 1.0);\n"
//        "    gl_Position = matrix * vertex;\n"
//        "}\n";
//    m_vshader->compileSourceCode(vsrc1);

//    m_fshader = new QOpenGLShader(QOpenGLShader::Fragment);
//    const char *fsrc1 =
//        "varying mediump vec4 color;\n"
////        "uniform mediump vec4 color;\n"
//        "void main(void)\n"
//        "{\n"
//        "    gl_FragColor = color;\n"
//        "}\n";
//    m_fshader->compileSourceCode(fsrc1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShader(m_vshader);
    m_program->addShader(m_fshader);
    m_program->link();

    m_vertexAttr = m_program->attributeLocation("vertex");
    m_matrixUniform = m_program->uniformLocation("matrix");
    m_lineColorUniform = m_program->uniformLocation("lineColor");

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

void GraphWidget::paintGL()
{
    QPainter painter;
    painter.begin(this);

    painter.setFont(mFont);
    QFontMetrics fm(mFont);
    int fh = fm.height();
    int fwmax = fm.width("9999");

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
    modelview.ortho(xMin-xAdd, xMax+xAdd, yMin-yAdd, yMax+yAdd, 0, 1);
//    modelview.rotate(m_fAngle, 0.0f, 1.0f, 0.0f);
//    modelview.rotate(m_fAngle, 1.0f, 0.0f, 0.0f);
//    modelview.rotate(m_fAngle, 0.0f, 0.0f, 1.0f);
//    modelview.scale(m_fScale);
//    modelview.translate(0.0f, -0.2f, 0.0f);

    m_program->bind();
    m_program->setUniformValue(m_matrixUniform, modelview);

    m_program->enableAttributeArray(m_vertexAttr);

    foreach (QString s, mGraphNames)
    {
        Graph &g = mGraphs[s];
        if (!g.visible)
            continue;
        m_program->setUniformValue(m_lineColorUniform, g.color);//.redF(), g.color.greenF(), g.color.blueF(), g.color.alphaF());
        glLineWidth(g.lineWidth);
        g.vbo.bind();
        g.writeBuf();
        m_program->setAttributeBuffer(m_vertexAttr, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
        if (!mPointCount)
        {
            //glDrawArrays(GL_LINE_STRIP, g.pointCount, g.vboSize - g.pointCount);
            glDrawArrays(GL_LINE_STRIP, 0, g.pointCount); // vertices count
        }
        else
        {
            int cnt = qMin(g.pointCount, mPointCount);
            glDrawArrays(GL_LINE_STRIP, g.pointCount - cnt, cnt); // vertices count
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
    m_program->setUniformValue(m_lineColorUniform, 0.0, 0.0, 0.0, 0.5);
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

    P = comb.map(P);

//    painter.drawLine(QPointF(0, 0), P);
//    painter.drawText(P, "0");

    for (float yy=by; yy<=yMax+0.0001; yy+=dy)
    {
        QPointF P = comb.map(QPointF(xMin, yy));
        QString s = QString::number(yy, 'g', 3).replace('.',',');
        int fw = fm.width(s);
        painter.drawText(P + QPointF(-fw-4, fh/2-2), s);
    }
    for (float xx=bx; xx<=xMax+0.0001; xx+=dx)
    {
        QPointF P = comb.map(QPointF(xx, yMin));
        QString s = QString::number(xx, 'g', 3).replace('.',',');
        int fw = fm.width(s);
        painter.drawText(P + QPointF(-fw/2, fh-3), s);
    }

    QPointF Ps = comb.map(QPointF(xMax, yMin)) + QPointF(2, 0);
    painter.drawText(Ps, mXlabel);

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


GraphWidget::Graph::Graph() :
    vboSize(16384),
    pointCount(0),
    lineWidth(0),
    _initialized(false),
    visible(true)
{

}

void GraphWidget::Graph::initialize(int maxPointCount)
{
    if (_initialized)
        return;
    if (maxPointCount)
        vboSize = maxPointCount;
    vbo.create();
    vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vbo.bind();
    vbo.allocate(vboSize * 2 * sizeof(GLfloat));
    vbo.release();
    _initialized = true;
}

void GraphWidget::Graph::addPoint(float x, float y)
{
    //if (pointBuffer.size() < vboSize)
    {
        pointBuffer << x << y;
    }
}

void GraphWidget::Graph::clear()
{
    pointBuffer.clear();
    pointCount = 0;
}

void GraphWidget::Graph::writeBuf()
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

    while (pointBuffer.size())
    {
        int size = qMin(pointBuffer.size(), (vboSize - pointCount) * 2);

//        vbo.write(pointCount*2*sizeof(GLfloat), pointBuffer.data(), size*sizeof(GLfloat));
        // tipa tak faster than write:
        GLfloat *buf = reinterpret_cast<GLfloat*>(vbo.mapRange(pointCount*2*sizeof(GLfloat), size*sizeof(GLfloat), QOpenGLBuffer::RangeWrite));
        memcpy(buf, pointBuffer.data(), size*sizeof(GLfloat));
        vbo.unmap();

        pointCount += size / 2;
        if (pointCount >= vboSize)
            pointCount = 0;
        pointBuffer.remove(0, size);
    }
}
//---------------------------------------------------------------------------
