#ifndef LIGHT3D_H
#define LIGHT3D_H

#include "object3D.h"
#include <QVector3D>
#include <QColor>

class Light3D : public Object3D
{
    Q_OBJECT

private:
    int mLightNo;
    quint16 mLightID;

    QVector3D FPosition;
    QVector3D FDirection;
    qreal FSpotExponent;
    qreal FSpotCutoff;
    qreal FConstantAtt;
    qreal FLinearAtt;
    qreal FQuadraticAtt;
    QColor FAmbientColor;
    QColor FDiffuseColor;
    QColor FSpecularColor;
    bool FEnabled;

    GLfloat p[4];
    GLfloat d[3];
    GLfloat acol[4];
    GLfloat dcol[4];
    GLfloat scol[4];

protected:
    void applySettings();

    void setLightNo(int lightNo);

    friend class QPanel3D;

public:
    explicit Light3D(QObject *parent);

    QVector3D position() const {return FPosition;}
    QVector3D direction() const {return FDirection;}
    qreal spotExponent() const {return FSpotExponent;}
    qreal spotCutoff() const {return FSpotCutoff;}
    qreal constantAtt() const {return FConstantAtt;}
    qreal linearAtt() const {return FLinearAtt;}
    qreal quadraticAtt() const {return FQuadraticAtt;}
    QColor ambientColor() const {return FAmbientColor;}
    QColor diffuseColor() const {return FDiffuseColor;}
    QColor specularColor() const {return FSpecularColor;}
    bool enabled() const {return FEnabled;}
    int lightNo() const {return mLightNo;}

    void setPosition(QVector3D pos);
    void setDirection(QVector3D dir);
    void setSpotExponent(qreal val);
    void setSpotCutoff(qreal val);
    void setConstantAtt(qreal val);
    void setLinearAtt(qreal val);
    void setQuadraticAtt(qreal val);
    void setAmbientColor(QColor color);
    void setDiffuseColor(QColor color);
    void setSpecularColor(QColor color);
    void setEnabled(bool enabled);

signals:

public slots:

};

#endif // LIGHT3D_H
