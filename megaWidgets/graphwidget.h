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

class GraphWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    enum GraphType {Line, Points};

private:
    class Graph
    {
    public:
        QOpenGLBuffer vbo;
        QVector<GLfloat> pointBuffer;
        int vboSize;
        int pointCount;
        QColor color;
        float lineWidth;
        float pointSize;
        bool _initialized;
        bool visible;
        GraphType appearance = Line;

        Graph();
        void initialize(int maxPointCount);
        void addPoint(float x, float y);
        void clear();
        void writeBuf();
    };

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

    QColor mBackColor = Qt::white;
    QFont mFont;

    QVector<GLfloat> gridBuf;

    QVector<QString> mGraphNames;
    QMap<QString, Graph> mGraphs;

    QString mXlabel;

    int mMaxPointCount;
    int mPointCount;

    float xMin, xMax;
    float yMin, yMax;

    float xMin0, xMax0;
    float yMin0, yMax0;

    bool mAutoZoom;
    bool mInitialized;

    bool m_pointsVisible;

    QPoint m_mousePos;

protected:
//    void resizeGL(int w, int h) override;
    void paintGL() override;
    void initializeGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

public:
    GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    void addGraph(QString name, QColor color = Qt::black, float lineWidth = 1.0f);
    void addPoint(QString name, float x, float y);

    void setGraphVisible(QString name, bool visible);

    void setBounds(float xmin, float ymin, float xmax, float ymax);
    float minX() const {return xMin;}
    void setXmin(float xmin) {xMin = xmin;}
    float maxX() const {return xMax;}
    void setXmax(float xmax) {xMax = xmax;}
    float minY() const {return yMin;}
    void setYmin(float ymin) {yMin = ymin;}
    float maxY() const {return yMax;}
    void setYmax(float ymax) {yMax = ymax;}

    int maxPointCount() const {return mMaxPointCount;}
    void setMaxPointCount(int cnt) {mMaxPointCount = cnt;}
    int pointCount() const {return mPointCount;}
    void setPointCount(int cnt) {mPointCount = cnt;}

    void clear();
    void clear(QString name);

    void setBackColor(QColor col) {mBackColor = col; update();}
    QFont font() const {return mFont;}
    void setFont(const QFont &font) {mFont = font;}

    void setXlabel(QString label);

    void setAutoBoundsEnabled(bool enabled) {mAutoZoom = enabled;}

    void setGraphType(QString name, GraphType type);
    void setPointSize(QString name, float size);
};

#endif // GRAPHWIDGET_H
