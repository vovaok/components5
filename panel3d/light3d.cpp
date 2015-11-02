#include "light3d.h"

Light3D::Light3D(QObject *parent) :
    Object3D(parent)
{
    FEnabled = true;

    FPosition = QVector3D(0, 100, 0);
    p[0] = FPosition.x();
    p[1] = FPosition.y();
    p[2] = FPosition.z();
    p[3] = 1.0;

    FDirection = QVector3D(0, -1, 0);
    d[0] = FDirection.x();
    d[1] = FDirection.y();
    d[2] = FDirection.z();

    FSpotExponent = 0;
    FSpotCutoff = 180;
    FConstantAtt = 1.0;
    FLinearAtt = 0;
    FQuadraticAtt = 0;

    FAmbientColor = Qt::black;
    acol[0] = FAmbientColor.redF();
    acol[1] = FAmbientColor.greenF();
    acol[2] = FAmbientColor.blueF();
    acol[3] = FAmbientColor.alphaF();

    FDiffuseColor = Qt::white;
    dcol[0] = FDiffuseColor.redF();
    dcol[1] = FDiffuseColor.greenF();
    dcol[2] = FDiffuseColor.blueF();
    dcol[3] = FDiffuseColor.alphaF();

    FSpecularColor = Qt::white;
    scol[0] = FSpecularColor.redF();
    scol[1] = FSpecularColor.greenF();
    scol[2] = FSpecularColor.blueF();
    scol[3] = FSpecularColor.alphaF();

    setSettingsChanged();
}

void Light3D::applySettings()
{
    if (FEnabled)
        glEnable(mLightID);
    else
        glDisable(mLightID);

    glLightf(mLightID, GL_SPOT_EXPONENT, (GLfloat)FSpotExponent);
    glLightf(mLightID, GL_SPOT_CUTOFF, (GLfloat)FSpotCutoff);
    glLightf(mLightID, GL_CONSTANT_ATTENUATION, (GLfloat)FConstantAtt);
    glLightf(mLightID, GL_LINEAR_ATTENUATION, (GLfloat)FLinearAtt);
    glLightf(mLightID, GL_QUADRATIC_ATTENUATION, (GLfloat)FQuadraticAtt);
    glLightfv(mLightID, GL_POSITION, p);
    glLightfv(mLightID, GL_SPOT_DIRECTION, d);
    glLightfv(mLightID, GL_AMBIENT, acol);
    glLightfv(mLightID, GL_DIFFUSE, dcol);
    glLightfv(mLightID, GL_SPECULAR, scol);
}

void Light3D::setPosition(QVector3D pos)
{
    FPosition = pos;
    p[0] = FPosition.x();
    p[1] = FPosition.y();
    p[2] = FPosition.z();
    p[3] = 1.0;
    setSettingsChanged();
}

void Light3D::setDirection(QVector3D dir)
{
    FDirection = dir;
    d[0] = FDirection.x();
    d[1] = FDirection.y();
    d[2] = FDirection.z();
    setSettingsChanged();
}

void Light3D::setSpotExponent(qreal val)
{
    FSpotExponent = val;
    setSettingsChanged();
}

void Light3D::setSpotCutoff(qreal val)
{
    FSpotCutoff = val;
    setSettingsChanged();
}

void Light3D::setConstantAtt(qreal val)
{
    FConstantAtt = val;
    setSettingsChanged();
}

void Light3D::setLinearAtt(qreal val)
{
    FLinearAtt = val;
    setSettingsChanged();
}

void Light3D::setQuadraticAtt(qreal val)
{
    FQuadraticAtt = val;
    setSettingsChanged();
}

void Light3D::setAmbientColor(QColor color)
{
    FAmbientColor = color;
    acol[0] = FAmbientColor.redF();
    acol[1] = FAmbientColor.greenF();
    acol[2] = FAmbientColor.blueF();
    acol[3] = FAmbientColor.alphaF();
    setSettingsChanged();
}

void Light3D::setDiffuseColor(QColor color)
{
    FDiffuseColor = color;
    dcol[0] = FDiffuseColor.redF();
    dcol[1] = FDiffuseColor.greenF();
    dcol[2] = FDiffuseColor.blueF();
    dcol[3] = FDiffuseColor.alphaF();
    setSettingsChanged();
}

void Light3D::setSpecularColor(QColor color)
{
    FSpecularColor = color;
    scol[0] = FSpecularColor.redF();
    scol[1] = FSpecularColor.greenF();
    scol[2] = FSpecularColor.blueF();
    scol[3] = FSpecularColor.alphaF();
    setSettingsChanged();
}

void Light3D::setEnabled(bool enabled)
{
    FEnabled = enabled;
    setSettingsChanged();
}

void Light3D::setLightNo(int lightNo)
{
    mLightNo = lightNo;
    switch (mLightNo)
    {
      case 0: mLightID = GL_LIGHT0; break;
      case 1: mLightID = GL_LIGHT1; break;
      case 2: mLightID = GL_LIGHT2; break;
      case 3: mLightID = GL_LIGHT3; break;
      case 4: mLightID = GL_LIGHT4; break;
      case 5: mLightID = GL_LIGHT5; break;
      case 6: mLightID = GL_LIGHT6; break;
      case 7: mLightID = GL_LIGHT7; break;
      default:
          mLightID = GL_LIGHT0;
          qWarning("too many lights created, it will replace GL_LIGHT0.");
    }
}
