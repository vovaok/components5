#ifndef VRMLPARSER_H
#define VRMLPARSER_H

#include <QObject>
#include <QMap>
#include <QTextStream>
#include "vrml2types.h"

using namespace VRML2;

class Vrml2Parser : public QObject
{
    Q_OBJECT
private: // typedefs
    typedef QMap<QString, SFNode> NodeMap;

private:
    GroupingNode *SceneGraph;
    NodeMap nodeMap;

    QTextStream *Stream;
    QString Line;

    int bracesCount;
    int bracketsCount;

    QStringList Comments;

private:
    void readNextLine();
    QString takeNextWord();
    QString CurrentWord;
    bool CurrentWordParsed;

    // simple type parsing
    inline SFBool parseSFBool();
    inline SFInt32 parseSFInt32();
    inline SFFloat parseSFFloat();
    inline SFVec2f parseSFVec2f();
    inline SFVec3f parseSFVec3f();
    inline SFRotation parseSFRotation();
    inline SFColor parseSFColor();
    inline QString parseString();

    // multiple field type parsing
    void parseMFInt32(MFInt32 &array);
    void parseMFVec2f(MFVec2f &array);
    void parseMFVec3f(MFVec3f &array);
    void parseMFColor(MFColor &array);

    // node parsing
    SFNode registerNode(SFNode node, QString nodeName);
    SFNode parseSFNode(GroupingNode *parent=0L);
    void parseMFNode(GroupingNode *parent);

    // complex type parsing
    void parseTransform(Transform *node);
    void parseGroup(Group *node);

    void parseShape(Shape *node);
    void parseAppearance(Appearance *node);
    void parseImageTexture(ImageTexture *node);
    void parseMaterial(Material *node);
    void parseIndexedFaceSet(IndexedFaceSet* node);
    void parseColor(Color *node);
    void parseNormal(Normal *node);
    void parseCoordinate(Coordinate *node);
    void parseTextureCoordinate(TextureCoordinate *node);

    void parseUnknown();

public:
    explicit Vrml2Parser(QObject *parent = 0);
    ~Vrml2Parser();

    void clearScene();
    void parse(QTextStream *stream);

    inline QStringList& comments() {return Comments;}
    inline GroupingNode *sceneGraph() {return SceneGraph;}

signals:

public slots:

};

#endif // VRMLPARSER_H
