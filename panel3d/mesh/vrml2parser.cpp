#include "vrml2parser.h"

Vrml2Parser::Vrml2Parser(QObject *parent) :
    QObject(parent)
{
    SceneGraph = new GroupingNode(0L);
    CurrentWordParsed = true;
}

Vrml2Parser::~Vrml2Parser()
{
    delete SceneGraph;
}
//----------------------------------------------------------

void Vrml2Parser::clearScene()
{
    for (NodeMap::Iterator it=nodeMap.begin(); it!=nodeMap.end(); it++)
        delete (*it);
    nodeMap.clear();
    Comments.clear();
    //CurrentNode = SceneGraph;
    bracesCount = 0;
    bracketsCount = 0;
}

void Vrml2Parser::readNextLine()
{
    QString line = Stream->readLine();
    int pos = line.indexOf('#');
    if (pos >= 0)
        Comments.append(line.mid(pos+1).trimmed());
    Line = line.left(pos).simplified();
}

QString Vrml2Parser::takeNextWord()
{
    if (!CurrentWordParsed)
    {
        CurrentWordParsed = true;
        return CurrentWord;
    }

    while (Line.isEmpty() && !Stream->atEnd())
        readNextLine();

    for (int i=0; i<Line.length(); i++)
    {
        char c = Line[i].toLatin1();
        switch (c)
        {
          case ' ':
          case ',':
          case '{':
          case '}':
          case '[':
          case ']':
            if (i == 0)
                i = 1;
            CurrentWord = Line.left(i);
            Line = Line.mid(i).trimmed();
            return CurrentWord;
        }
    }

    // delimiters not found
    CurrentWord = Line;
    Line = "";
    return CurrentWord;
}
//----------------------------------------------------------

void Vrml2Parser::parse(QTextStream *stream)
{
    clearScene();
    Stream = stream;

    while (!Stream->atEnd())
    {
        parseSFNode(SceneGraph);
    }

    //qDebug(QString().sprintf("braces = %d; brackets = %d;", bracesCount, bracketsCount).toLatin1().constData());
}
//----------------------------------------------------------

// simple types parsing
inline SFBool Vrml2Parser::parseSFBool()
{
    return takeNextWord()=="TRUE"? true: false;
}

inline SFInt32 Vrml2Parser::parseSFInt32()
{
    return takeNextWord().toInt(&CurrentWordParsed);
}

inline SFFloat Vrml2Parser::parseSFFloat()
{
    return takeNextWord().toFloat(&CurrentWordParsed);
}

inline SFVec2f Vrml2Parser::parseSFVec2f()
{
    SFFloat x = parseSFFloat();
    SFFloat y = parseSFFloat();
    return SFVec2f(x, y);
}

inline SFVec3f Vrml2Parser::parseSFVec3f()
{
    SFFloat x = parseSFFloat();
    SFFloat y = parseSFFloat();
    SFFloat z = parseSFFloat();
    return SFVec3f(x, y, z);
}

inline SFRotation Vrml2Parser::parseSFRotation()
{
    SFFloat x = parseSFFloat();
    SFFloat y = parseSFFloat();
    SFFloat z = parseSFFloat();
    SFFloat a = parseSFFloat();
    return SFRotation(x, y, z, a);
}

inline SFColor Vrml2Parser::parseSFColor()
{
    SFFloat r = parseSFFloat();
    SFFloat g = parseSFFloat();
    SFFloat b = parseSFFloat();
    return SFColor(r*255, g*255, b*255);
}

QString Vrml2Parser::parseString()
{
    QString str = takeNextWord();
    if (str.startsWith('\"'))
    {
        str.remove(0, 1);
        while (1) // ne nado tak, no mne len' po-drugomu
        {
            if (str.contains('\"'))
            {
                str.remove('\"');
                return str;
            }
            QString w = takeNextWord();
            str += " " + w;
            if (w.isEmpty())
                break;
        }
    }
    return QString();
}

// multiple fields parsing
void Vrml2Parser::parseMFInt32(MFInt32 &array)
{
    if (takeNextWord() == "[")
    {
        bracketsCount++;
        do
        {
            array.append(parseSFInt32());
        }
        while (takeNextWord() != "]");
        bracketsCount--;
    }
}

void Vrml2Parser::parseMFVec2f(MFVec2f &array)
{
    if (takeNextWord() == "[")
    {
        bracketsCount++;
        do
        {
            array.append(parseSFVec2f());
        }
        while (takeNextWord() != "]");
        bracketsCount--;
    }
}

void Vrml2Parser::parseMFVec3f(MFVec3f &array)
{
    if (takeNextWord() == "[")
    {
        bracketsCount++;
        do
        {
            array.append(parseSFVec3f());
        }
        while (takeNextWord() != "]");
        bracketsCount--;
    }
}

void Vrml2Parser::parseMFColor(MFColor &array)
{
    if (takeNextWord() == "[")
    {
        bracketsCount++;
        do
        {
            array.append(parseSFColor());
        }
        while (takeNextWord() != "]");
        bracketsCount--;
    }
}
//----------------------------------------------------------

SFNode Vrml2Parser::registerNode(SFNode node, QString nodeName)
{
//    qDebug(QString("found "+node->className()+" with name = '"+nodeName+"'").toLatin1().constData());
    node->setName(nodeName);
    nodeMap[nodeName] = node;
    return node;
}

SFNode Vrml2Parser::parseSFNode(GroupingNode *parent)
{
    QString nodeName;
    QString token = takeNextWord();
    if (token == "DEF")
    {
        nodeName = takeNextWord();
        token = takeNextWord();
    }
    else if (token == "USE")
    {
        nodeName = takeNextWord();
        return nodeMap.value(nodeName, 0L);
    }

    SFNode node = 0L;
    QString className = token;

    if (className == "Transform")
    {
        node = registerNode(new Transform(parent), nodeName);
        parseTransform(dynamic_cast<Transform*>(node));
    }
    else if (className == "Group")
    {
        node = registerNode(new Group(parent), nodeName);
        parseGroup(dynamic_cast<Group*>(node));
    }
    else if (className == "Shape")
    {
        node = registerNode(new Shape(parent), nodeName);
        parseShape(dynamic_cast<Shape*>(node));
    }
    else if (className == "Appearance")
    {
        node = registerNode(new Appearance, nodeName);
        parseAppearance(dynamic_cast<Appearance*>(node));
    }
    else if (className == "ImageTexture")
    {
        node = registerNode(new ImageTexture, nodeName);
        parseImageTexture(dynamic_cast<ImageTexture*>(node));
    }
    else if (className == "Material")
    {
        node = registerNode(new Material, nodeName);
        parseMaterial(dynamic_cast<Material*>(node));
    }
    else if (className == "IndexedFaceSet")
    {
        node = registerNode(new IndexedFaceSet, nodeName);
        parseIndexedFaceSet(dynamic_cast<IndexedFaceSet*>(node));
    }
    else if (className == "Color")
    {
        node = registerNode(new Color, nodeName);
        parseColor(dynamic_cast<Color*>(node));
    }
    else if (className == "Normal")
    {
        node = registerNode(new Normal, nodeName);
        parseNormal(dynamic_cast<Normal*>(node));
    }
    else if (className == "Coordinate")
    {
        node = registerNode(new Coordinate, nodeName);
        parseCoordinate(dynamic_cast<Coordinate*>(node));
    }
    else if (className == "TextureCoordinate")
    {
        node = registerNode(new TextureCoordinate, nodeName);
        parseTextureCoordinate(dynamic_cast<TextureCoordinate*>(node));
    }
    else
    {
        parseUnknown();
        qDebug(QString("found unknown type "+className+" with name = '"+nodeName+"'").toLatin1().constData());
    }

    return node;
}

void Vrml2Parser::parseMFNode(GroupingNode *parent)
{
    if (takeNextWord() == "[")
    {
        bracketsCount++;
        do
        {
            //CurrentWordParsed = false;
            parseSFNode(parent);
            QString token = takeNextWord();
            if (token == "]")
                break;
            else if (token == ",")
                continue;
            else
                CurrentWordParsed = false;
        }
        while (1);//takeNextWord() != "]");
        bracketsCount--;
    }
}

void Vrml2Parser::parseUnknown()
{
    int _braces = 0;
    QString token;
    do
    {
        token = takeNextWord();
        if (token == "{")
            _braces++;
        else if (token == "}")
            _braces--;
    }
    while (_braces);
}
//----------------------------------------------------------

void Vrml2Parser::parseTransform(Transform *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "center")
                node->center = parseSFVec3f();
            else if (token == "rotation")
                node->rotation = parseSFRotation();
            else if (token == "scale")
                node->scale = parseSFVec3f();
            else if (token == "scaleOrientation")
                node->scaleOrientation = parseSFRotation();
            else if (token == "translation")
                node->translation = parseSFVec3f();
            else if (token == "bboxCenter")
                node->bboxCenter = parseSFVec3f();
            else if (token == "bboxSize")
                node->bboxSize = parseSFVec3f();
            else if (token == "children")
                parseMFNode(node);
        }
        while (token != "}");
        bracesCount--;
    }
}

void Vrml2Parser::parseGroup(Group *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "bboxCenter")
                node->bboxCenter = parseSFVec3f();
            else if (token == "bboxSize")
                node->bboxSize = parseSFVec3f();
            else if (token == "children")
                parseMFNode(node);
        }
        while (token != "}");
        bracesCount--;
    }
}
//----------------------------------------------------------

void Vrml2Parser::parseShape(Shape *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "appearance")
                node->setAppearance(dynamic_cast<Appearance*>(parseSFNode()));
            else if (token == "geometry")
                node->setGeometry(parseSFNode());
        }
        while (token != "}");
        bracesCount--;
    }
}

void Vrml2Parser::parseAppearance(Appearance *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "material")
                node->setMaterial(dynamic_cast<Material*>(parseSFNode()));
            else if (token == "texture")
                node->setTexture(dynamic_cast<Texture*>(parseSFNode()));
            else if (token == "textureTransform")
                node->setTextureTransform(dynamic_cast<TextureTransform*>(parseSFNode()));
        }
        while (token != "}");
        bracesCount--;
    }
}

void Vrml2Parser::parseImageTexture(ImageTexture *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "url")
            {
                node->url = parseString();
            }
        }
        while (token != "}");
        bracesCount--;
    }
}

void Vrml2Parser::parseMaterial(Material *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "ambientIntensity")
                node->ambientIntensity = parseSFFloat();
            else if (token == "diffuseColor")
                node->diffuseColor = parseSFColor();
            else if (token == "emissiveColor")
                node->emissiveColor = parseSFColor();
            else if (token == "shininess")
                node->shininess = parseSFFloat();
            else if (token == "specularColor")
                node->specularColor = parseSFColor();
            else if (token == "transparency")
                node->transparency = parseSFFloat();
        }
        while (token != "}");
        bracesCount--;
    }
}

void Vrml2Parser::parseIndexedFaceSet(IndexedFaceSet *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "color")
                node->setColor(dynamic_cast<Color*>(parseSFNode()));
            else if (token == "coord")
                node->setCoordinate(dynamic_cast<Coordinate*>(parseSFNode()));
            else if (token == "normal")
                node->setNormal(dynamic_cast<Normal*>(parseSFNode()));
            else if (token == "texCoord")
                node->setTexCoord(dynamic_cast<TextureCoordinate*>(parseSFNode()));
            else if (token == "ccw")
                node->ccw = parseSFBool();
            else if (token == "colorIndex")
                parseMFInt32(node->colorIndex);
            else if (token == "colorPerVertex")
                node->colorPerVertex = parseSFBool();
            else if (token == "convex")
                node->convex = parseSFBool();
            else if (token == "coordIndex")
                parseMFInt32(node->coordIndex);
            else if (token == "creaseAngle")
                node->creaseAngle = parseSFFloat();
            else if (token == "normalIndex")
                parseMFInt32(node->normalIndex);
            else if (token == "normalPerVertex")
                node->normalPerVertex = parseSFBool();
            else if (token == "solid")
                node->solid = parseSFBool();
            else if (token == "texCoordIndex")
                parseMFInt32(node->texCoordIndex);
        }
        while (token != "}");
        bracesCount--;
    }
}

void Vrml2Parser::parseColor(Color *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "color")
                parseMFColor(node->color);
        }
        while (token != "}");
        bracesCount--;
    }
}

void Vrml2Parser::parseNormal(Normal *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "vector")
                parseMFVec3f(node->vector);
        }
        while (token != "}");
        bracesCount--;
    }
}

void Vrml2Parser::parseCoordinate(Coordinate *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "point")
                parseMFVec3f(node->point);
        }
        while (token != "}");
        bracesCount--;
    }
}

void Vrml2Parser::parseTextureCoordinate(TextureCoordinate *node)
{
    QString token = takeNextWord();
    if (token == "{")
    {
        bracesCount++;
        do
        {
            token = takeNextWord();
            if (token == "point")
                parseMFVec2f(node->point);
        }
        while (token != "}");
        bracesCount--;
    }
}
//----------------------------------------------------------
