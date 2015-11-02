#ifndef VRMLTYPES_H
#define VRMLTYPES_H

#include <QList>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QQuaternion>
#include <QColor>

namespace VRML2
{

// typedefs
typedef bool                SFBool;
typedef qint32              SFInt32;
typedef float               SFFloat;
typedef QVector2D           SFVec2f;
typedef QVector3D           SFVec3f;
typedef QQuaternion         SFRotation;
typedef QColor              SFColor;

typedef QVector<SFInt32>    MFInt32;
typedef QVector<SFVec2f>    MFVec2f;
typedef QVector<SFVec3f>    MFVec3f;
typedef QVector<SFColor>    MFColor;

// base classes:
class SFNodeT
{
private:
    QString Name;
protected:
    QString ClassName;
public:
    explicit SFNodeT(QString className="") : ClassName(className) {}
    virtual ~SFNodeT() {}
    inline const QString name() {return Name;}
    inline void setName(QString name) {Name = name;}
    inline const QString className() {return ClassName;}
};

typedef SFNodeT*    SFNode;

class GroupingNode;

class ChildNode : public SFNodeT
{
private:
    GroupingNode *Parent;
public:
    explicit ChildNode(GroupingNode *parent, QString className="");
    virtual ~ChildNode();
    inline const GroupingNode* parent() {return Parent;}
};

typedef QList<ChildNode*> MFNode;

class GroupingNode : public ChildNode
{
private:
    MFNode _children;
public:
    explicit GroupingNode(GroupingNode *parent, QString className="") : ChildNode(parent, className) {}
    virtual ~GroupingNode()
    {
//        for (MFNode::Iterator it=_children.begin(); it!=_children.end(); it++)
//            delete (*it);
    }
    inline MFNode& children() {return _children;}
    inline void appendChild(ChildNode *child) {_children.append(child);}
};
//----------------------------------------------------------

// multiple grouping nodes:
class Transform : public GroupingNode
{
public:
    SFVec3f center;
    SFRotation rotation;
    SFVec3f scale;
    SFRotation scaleOrientation;
    SFVec3f translation;
    SFVec3f bboxCenter;
    SFVec3f bboxSize;

    explicit Transform(GroupingNode *parent) : GroupingNode(parent, "Transform") {}
};

class Group : public GroupingNode
{
public:
    SFVec3f bboxCenter;
    SFVec3f bboxSize;

    explicit Group(GroupingNode *parent) : GroupingNode(parent, "Group") {}
};
//----------------------------------------------------------

// simple nodes:
class Material : public SFNodeT
{
public:
    SFFloat ambientIntensity;
    SFColor diffuseColor;
    SFColor emissiveColor;
    SFFloat shininess;
    SFColor specularColor;
    SFFloat transparency;

    Material() : SFNodeT("Material"),
        ambientIntensity(0.2),
        diffuseColor(0.8*255, 0.8*255, 0.8*255),
        emissiveColor(0, 0, 0),
        shininess(0.2),
        specularColor(0, 0, 0),
        transparency(0)
    {}
};

typedef SFNodeT Texture; // false class, need implementation
typedef SFNodeT TextureTransform; // false class, need implementation

class Appearance : public SFNodeT
{
private:
    SFNode _material;
    SFNode _texture;
    SFNode _textureTransform;

    inline void deleteMaterial()         {} //{if (_material) delete _material;}
    inline void deleteTexture()          {} //{if (_texture) delete _texture;}
    inline void deleteTextureTransform() {} //{if (_textureTransform) delete _textureTransform;}

public:
    Appearance() : SFNodeT("Appearance"),
        _material(0L),
        _texture(0L),
        _textureTransform(0L)
    {}
    ~Appearance()
    {
        deleteMaterial();
        deleteTexture();
        deleteTextureTransform();
    }

    inline Material* material() {return dynamic_cast<Material*>(_material);}
    inline Texture* texture() {return dynamic_cast<Texture*>(_texture);}
    inline TextureTransform* textureTransform() {return dynamic_cast<TextureTransform*>(_textureTransform);}

    inline void setMaterial(Material *material) {deleteMaterial(); _material = material;}
    inline void setTexture(Texture *texture) {deleteTexture(); _texture = texture;}
    inline void setTextureTransform(TextureTransform *textureTransform) {deleteTextureTransform(); _textureTransform = textureTransform;}
};
//----------------------------------------------------------

class Color : public SFNodeT
{
public:
    MFColor color;

    Color() : SFNodeT("Color") {}
};

class Coordinate : public SFNodeT
{
public:
    MFVec3f point;

    Coordinate() : SFNodeT("Coordinate") {}
};

class Normal : public SFNodeT
{
public:
    MFVec3f vector;

    Normal() : SFNodeT("Normal") {}
};

class TextureCoordinate : public SFNodeT
{
public:
    MFVec2f point;

    TextureCoordinate() : SFNodeT("TextureCoordinate") {}
};

typedef SFNodeT SFBox; // false class, need implementation
typedef SFNodeT Cone; // false class, need implementation
typedef SFNodeT Cylinder; // false class, need implementation
typedef SFNodeT ElevationGrid; // false class, need implementation
typedef SFNodeT Extrusion; // false class, need implementation
typedef SFNodeT IndexedLineSet; // false class, need implementation
typedef SFNodeT PointSet; // false class, need implementation
typedef SFNodeT Sphere; // false class, need implementation
typedef SFNodeT Text; // false class, need implementation

class IndexedFaceSet : public SFNodeT
{
private:
    SFNode _color;
    SFNode _coord;
    SFNode _normal;
    SFNode _texCoord;

    inline void deleteColor() {} //{if (_color) delete _color;}
    inline void deleteCoord() {} //{if (_coord) delete _coord;}
    inline void deleteNormal() {} //{if (_normal) delete _normal;}
    inline void deleteTexCoord() {} //{if (_texCoord) delete _texCoord;}

public:
    SFBool  ccw;
    MFInt32 colorIndex;
    SFBool  colorPerVertex;
    SFBool  convex;
    MFInt32 coordIndex;
    SFFloat creaseAngle;
    MFInt32 normalIndex;
    SFBool  normalPerVertex;
    SFBool  solid;
    MFInt32 texCoordIndex;

    IndexedFaceSet() : SFNodeT("IndexedFaceSet"),
        _color(0L),
        _coord(0L),
        _normal(0L),
        _texCoord(0L),
        ccw(true),
        colorPerVertex(true),
        convex(true),
        creaseAngle(0),
        normalPerVertex(true),
        solid(true)
    {}
    ~IndexedFaceSet()
    {
        deleteColor();
        deleteCoord();
        deleteNormal();
        deleteTexCoord();
    }

    inline Color* color() {return dynamic_cast<Color*>(_color);}
    inline Coordinate* coord() {return dynamic_cast<Coordinate*>(_coord);}
    inline Normal* normal() {return dynamic_cast<Normal*>(_normal);}
    inline TextureCoordinate* texCoord() {return dynamic_cast<TextureCoordinate*>(_texCoord);}

    inline void setColor(Color* color) {deleteColor(); _color = color;}
    inline void setCoordinate(Coordinate* coord) {deleteCoord(); _coord = coord;}
    inline void setNormal(Normal* normal) {deleteNormal(); _normal = normal;}
    inline void setTexCoord(TextureCoordinate* texCoord) {deleteTexCoord(); _texCoord = texCoord;}
};
//----------------------------------------------------------

// multiple child nodes
class Shape : public ChildNode
{
private:
    SFNode _appearance;
    SFNode _geometry;

    inline void deleteAppearance() {} //{if (_appearance) delete _appearance;}
    inline void deleteGeometry() {} //{if (_geometry) delete _geometry;}

public:
    explicit Shape(GroupingNode *parent) : ChildNode(parent, "Shape"),
        _appearance(0L),
        _geometry(0L)
    {}
    ~Shape()
    {
        deleteAppearance();
        deleteGeometry();
    }

    inline Appearance* appearance() {return dynamic_cast<Appearance*>(_appearance);}
    inline SFNode geometry() {return _geometry;}

    inline void setAppearance(Appearance* appearance) {deleteAppearance(); _appearance = appearance;}
    inline void setGeometry(SFNode geometry) {deleteGeometry(); _geometry = geometry;}
};
//----------------------------------------------------------

} // end of namespace VRML

#endif // VRMLTYPES_H
