#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>
#include <QPainter>
#include <QMap>
#include <QMouseEvent>
#include <math.h>

class Graph
{
public:
    Graph();

    enum Type {Line, Points};

    void addPoint(float x, float y);
    void clear();

private:
    friend class GraphWidget;

    void initialize(int maxPointCount);
    void writeBuf();

    QOpenGLBuffer vbo;
    QVector<GLfloat> pointBuffer;
    QRectF bounds;
    int vboSize;
    int curIdx;
    int pointCount;
    QColor color;
    float lineWidth;
    float pointSize;
    bool _initialized;
    bool visible;
    Type appearance = Line;
    QString unit;
};

class GraphWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:


//private:


private:
    QOpenGLShader *m_vshader;
    QOpenGLShader *m_fshader;
    QOpenGLShaderProgram *m_program;
//    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_grid_vbo;
    int m_vertexAttr;
    int m_matrixUniform;
    int m_lineColorUniform;
    int m_pointSizeUniform;
    int m_xminUniform, m_xmaxUniform;
    int m_yminUniform, m_ymaxUniform;

    QColor mBackColor = Qt::white;
    QColor mGridColor = QColor(0, 0, 0, 128);
    QFont mFont;

    QVector<GLfloat> gridBuf;

    QStringList mGraphNames;
    QMap<QString, Graph> mGraphs;

    QString mXlabel;

    int mMaxPointCount;
    int mPointCount;

    float xMin, xMax;
    float yMin, yMax;

    float xMin0, xMax0;
    float yMin0, yMax0;

    float xMinW, xMaxW;
    float yMinW, yMaxW;

    QMatrix4x4 m_viewTransform;

    float xWindow;

    bool mAutoZoom;
    bool mInitialized;

    bool m_innocent; // no data yet

    bool m_pointsVisible;

    QPoint m_mousePos;

protected:
//    void resizeGL(int w, int h) override;
    void paintGL() override;
    void initializeGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    void boundsChanged();

public:
    GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    Graph *addGraph(QString name, QColor color = Qt::black, float lineWidth = 1.0f);
    void removeGraph(QString name);
    void removeAll();
    Graph *graph(QString name);

    void addPoint(QString name, float x, float y);

    void setGraphVisible(QString name, bool visible);

    void resetBounds();
    void setBounds(float xmin, float ymin, float xmax, float ymax);
    float minX() const {return xMin;}
    void setXmin(float xmin) {xMin = xmin;}
    float maxX() const {return xMax;}
    void setXmax(float xmax) {xMax = xmax;}
    float minY() const {return yMin;}
    void setYmin(float ymin) {yMin = ymin;}
    float maxY() const {return yMax;}
    void setYmax(float ymax) {yMax = ymax;}
    QRectF bounds() const {return QRectF(QPointF(xMin, yMin), QPointF(xMax, yMax));}
    QRectF globalBounds() const {return QRectF(QPointF(xMin0, yMin0), QPointF(xMax0, yMax0));}

    const QMatrix4x4 &viewTransform() const {return m_viewTransform;}

    int maxPointCount() const {return mMaxPointCount;}
    void setMaxPointCount(int cnt) {mMaxPointCount = cnt;}
    int pointCount() const {return mPointCount;}
    void setPointCount(int cnt) {mPointCount = cnt;}

    void clear();
    void clear(QString name);

    void setBackColor(QColor col) {mBackColor = col; update();}
    void setGridColor(QColor col) {mGridColor = col; update();}
    QFont font() const {return mFont;}
    void setFont(const QFont &font) {mFont = font;}

    void setXlabel(QString label);

    void setAutoBoundsEnabled(bool enabled) {mAutoZoom = enabled;}
    void setXwindow(float value) {xWindow = value;}

    void setGraphType(QString name, Graph::Type type);
    void setPointSize(QString name, float size);

    void setUnit(QString name, QString unit);

    const QStringList &graphNames() const {return mGraphNames;}
};

#endif // GRAPHWIDGET_H
