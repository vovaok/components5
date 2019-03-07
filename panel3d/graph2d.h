#ifndef GRAPH2D_H
#define GRAPH2D_H

#include "object3d.h"

class Graph2D : public Object3D
{
    Q_OBJECT

public:
	/*! Frame type of the plot.
		Frame (or coordinate system) determines how to represent the plot.
	*/
    typedef enum
	{
		frameCartesian, //!< Cartesian (rectangular) coordinate system. This is default frame type.
		framePolar 		//!< Polar coordinate system. X is alias for angle (phi) and Y is alias for distance (rho).
	} FrameType;

//    typedef struct
//    {
//        int x;
//        int y;
//        QColor color;
//        QString text;
//    } LabelInfo;
//    typedef QVector<LabelInfo> Labels;

private:
    typedef QList<QPointF> GraphVector;
    typedef struct
    {
        GraphVector graph;
        float zoomX, zoomY;
        QColor color;
        float width;
        bool visible;
        //QString unit;
    } GraphInfo;
    typedef QHash<QString, GraphInfo> GraphHash;
    QStringList mGraphNames;

    FrameType mFrameType; // in polar coordinates x is phi and y is rho
    static const QColor mColors[8];
    int mCurColor;
    QString mLabelX;
    bool mPointsVisible;
    bool mEqualZoom;
	int mPointLimit;
    QColor mGridColor;
    bool mAutoBounds;
    QFont mFont;

    float mWidth;
    float mHeight;
    QRectF mBounds, mBoundsReload;

    GraphHash mGraphs;

    bool mLimitsEnabled;
    float mLowerLimit, mUpperLimit;

    float mDataWindowWidth;

    void recalculateBounds(float xmin, float xmax, float ymin, float ymax);

protected:
    virtual void draw();
    virtual QVector3D getMinBounds();
    virtual QVector3D getMaxBounds();

public:
	/*! Constructor of Graph2D object.
		It assigns default values to the attributes and defines frame type
		\param parent The parent object, usual it is Object3D derivative.
		\param frameType The frame type of the graph.
	*/
    explicit Graph2D(QObject *parent = 0, FrameType frameType=frameCartesian);

	/*! Set the object size.
		Set the size of the Graph2D object in units of scene.
		\param width Width of the object.
		\param height Height of the object.
		\return None.
	*/
    void setSize(float width, float height);
	
	/*! Set the bounds of the data plot.
		Bounds of the data plot are given in units of data.
        \param bounds Rectangle where data should be plotted. If this parameter is omitted the bounds are recalculated.
		\return None.
	*/
    void setBounds(QRectF bounds = QRectF());

    /*! Enable auto bounds.
        Enable automatic bounds recalculation on each point added.
        \param enable If true then bounds will be recalculated automatically.
        \return None.
    */
    void setAutoBoundsEnable(bool enable);
	
	/*! Set X coord label.
		\param text Label contents.
		\return None.
	*/
    void setLabelX(QString text) {mLabelX = text;}
	
	/*! Makes the plot data points visible.
		By default data plot represented with lines only. With points visible attribute each point of data will appear.
		\param visible If true data points will be visible.
		\return None.
	*/
    void setPointsVisible(bool visible=true) {mPointsVisible = visible;}
	
	/*! Set equal zoom to X and Y axes.
		Forces graph to zoom proportionally.
		\param enable If true equal zoom is applied.
		\return None.
	*/
    void setEqualZoom(bool enable=true) {mEqualZoom = enable;}
	
	/*! Add new plot.
		A new plot to represent its own relation.
		\param var Name of plotted variable.
		\param color Color of the plot.
		\param lineWidth Width of the line.
		\return None.
	*/
    void addGraph(QString var, QColor color, float lineWidth=2.0);
	
	/*! Add new point to plot.
		If the plot does not exist, the function creates it for you =).
		\param var Name of plot.
		\param x Point X coord.
		\param y Point Y coord.
		\return None.
	*/
    void addPoint(QString var, float x, float y);
	
	/*! Get plot info.
		Get information about the named plot.
		\param var The name of the interesting plot.
		\return Info about the plot.
	*/
    GraphInfo &info(QString var);
	
	/*! Clear the plot.
		Remove the named plot from the Graph2D object with its points.
		\param var Name of plot to be removed.
		\return None.
	*/
    void clear(QString var);
	
	/*! Clear entire Graph2D Object.
		Remove all plots with their points.
		\return None.
	*/
    void clear();

    /*! Set visibility of named plot. But points are being added to the plot.
     * \param var Name of the plot.
     * \return None.
     */
    void setVisible(QString var, bool visible);
	
	/*! Set limit area.
		Show area indicating logical limits of plots. Useful for visual estimation of plot characteristics.
		\param lower The lower limit.
		\param upper The upper limit.
		\return None.
	*/
    void setLimits(float lower, float upper);
	
	/*! Remove limit area.
		Remove limits defined in setLimits() function.
		\return None.
	*/
    void removeLimits() {mLimitsEnabled = false;}

    /*! Set point count limit.
        Set limit to count of data points to each plot.
        \param count Maximum count of points.
        \return None.
    */
    void setPointLimit(int count);

    void setDataWindowWidth(float width) {mDataWindowWidth = width;}

    /*! Set grid color.
        Assign the color to the grid. After that emits settingsChanged to redraw object.
        \param color The color of the grid.
        \return None.
    */
    void setGridColor(QColor color);

    void setFont(QFont font);

//signals:
    
//public slots:
    
};

#endif // GRAPH2D_H
