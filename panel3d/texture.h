#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include <QGLFramebufferObject>
#include <GL/gl.h>

class QPanel3D;

class Texture : public QObject
{
protected:
    QPainter mPainter;
    QPanel3D *mScene;
    GLuint mHandle;
    //virtual int handle() const = 0;

public:
    explicit Texture(QPanel3D *scene);// : mScene(scene), mHandle(-1) {setParent(mScene);}

//    int handle() {return mHandle;}
    //virtual QPaintDevice *paintDevice() = 0;

    virtual QPainter *paintBegin() = 0;
    virtual void paintEnd() = 0;

    void bind();
    static void disable();
};

class StaticTexture: public Texture
{
private:
    QImage mImage;

protected:
    //int handle() const {return mHandle;}

public:
    StaticTexture(QPanel3D *scene, const QImage &image);

    //QPaintDevice *paintDevice() {return &mImage;}
    QPainter *paintBegin() {mPainter.begin(&mImage); return &mPainter;}
    void paintEnd() {mPainter.end();}

    void update();
};

class DynamicTexture : public Texture
{
private:
    QGLFramebufferObject *mFbo;

protected:
    //int handle() const {return mFbo->texture();}

public:
    DynamicTexture(QPanel3D *scene, QSize size);
    ~DynamicTexture();

    //QPaintDevice *paintDevice() {return mFbo;}
//    QGLFramebufferObject *fbo() {return mFbo;}

    QPainter *paintBegin();// {mScene->makeCurrent(); glPushAttrib(GL_ALL_ATTRIB_BITS); mPainter.begin(&mImage); return mPainter;}
    void paintEnd();
};

#endif // TEXTURE3D_H
