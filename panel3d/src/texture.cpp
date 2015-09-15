#include "texture.h"
#include "qpanel3d.h"

Texture::Texture(QPanel3D *scene) :
    QObject(scene),
    mScene(scene),
    mHandle(-1)
{
}

void Texture::bind()
{
    if (!glIsTexture(mHandle))
        return;
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glEnable(GL_TEXTURE_2D);
}

void Texture::disable()
{
    glDisable(GL_TEXTURE_2D);
}
//---------------------------------------------------------------------------

StaticTexture::StaticTexture(QPanel3D *scene, const QImage &image) :
    Texture(scene),
    mImage(image)
{
    mScene->makeCurrent();
    mImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    glGenTextures(1, &mHandle);
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mImage.width(), mImage.height(), 1, GL_BGRA, GL_UNSIGNED_BYTE, mImage.constBits());
}

void StaticTexture::update()
{
    mScene->makeCurrent();
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mImage.width(), mImage.height(), 1, GL_BGRA, GL_UNSIGNED_BYTE, mImage.constBits());
}
//---------------------------------------------------------------------------

DynamicTexture::DynamicTexture(QPanel3D *scene, QSize size):
    Texture(scene),
    mFbo(0L)
{

    mScene->makeCurrent();
    mFbo = new QGLFramebufferObject(size);
    mHandle = mFbo->texture();
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glDisable(GL_TEXTURE_2D);
}

DynamicTexture::~DynamicTexture()
{
    delete mFbo;
}

QPainter *DynamicTexture::paintBegin()
{
    mScene->makeCurrent();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_CULL_FACE);
    mPainter.begin(mFbo);
    return &mPainter;
}

void DynamicTexture::paintEnd()
{
    mPainter.end();
    glPopAttrib();
    glEnable(GL_MULTISAMPLE_ARB);
//    glEnable(GL_CULL_FACE);
}
//---------------------------------------------------------------------------
