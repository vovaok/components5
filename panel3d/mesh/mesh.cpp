#include "mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(QTextStream *stream)
{
    load(stream);
}

Mesh::~Mesh()
{
    clear();
}

void Mesh::clear()
{
    qDeleteAll(Shapes);

    Shapes.clear();
}

void Mesh::load(QTextStream *stream)
{
    clear();
    loadVrml2(stream);
}

void Mesh::scaleUniform(float factor)
{
    for (auto& shape : Shapes)
    {
        for (auto& point : shape->points)
            point *= factor;
        for (auto& vertex : shape->vertices)
            vertex.point *= factor;
    }
}

void Mesh::translate(QVector3D p)
{
    for (auto& shape : Shapes)
    {
        for (auto& point : shape->points)
            point += p;
        for (auto& vertex : shape->vertices)
            vertex.point += p;
    }
}

void Mesh::rotate(float angle, QVector3D axis)
{
    QQuaternion rot = QQuaternion::fromAxisAndAngle(axis, angle);
    for (auto& shape : Shapes)
    {
        for (auto& point : shape->points)
            point = rot * point;
        for (auto& n : shape->normals)
            n = rot * n;
        for (auto& vertex : shape->vertices)
        {
            vertex.point = rot * vertex.point;
            vertex.normal = rot * vertex.normal;
        }
    }
}

void Mesh::loadVrml2(QTextStream *stream)
{
    Vrml2Parser *parser = new Vrml2Parser();
    parser->parse(stream);
    loadChildren(parser->sceneGraph());
    delete parser;
}

void Mesh::loadChildren(GroupingNode *par)
{
    for (VRML2::MFNode::Iterator it=par->children().begin(); it!=par->children().end(); it++)
    {
        ChildNode *node = (*it);

        loadShape(dynamic_cast<Shape*>(node));

        GroupingNode *par2 = dynamic_cast<GroupingNode*>(node);
        if (par2)
            loadChildren(par2);
    }
}

void Mesh::loadShape(Shape *node)
{
    if (!node)
        return;

    MeshShape *shape = new MeshShape;
    Shapes << shape;

    Appearance *appearance = node->appearance();
    if (appearance)
    {
        Material *material = appearance->material();
        if (material)
        {
            shape->material.diffuseColor[0] = float(material->diffuseColor.redF());
            shape->material.diffuseColor[1] = float(material->diffuseColor.greenF());
            shape->material.diffuseColor[2] = float(material->diffuseColor.blueF());
            shape->material.diffuseColor[3] = 1.0f - material->transparency;
            shape->material.ambientColor[0] = shape->material.diffuseColor[0] * material->ambientIntensity;
            shape->material.ambientColor[1] = shape->material.diffuseColor[1] * material->ambientIntensity;
            shape->material.ambientColor[2] = shape->material.diffuseColor[2] * material->ambientIntensity;
            shape->material.ambientColor[3] = shape->material.diffuseColor[3];
            shape->material.emissiveColor[0] = float(material->emissiveColor.redF());
            shape->material.emissiveColor[1] = float(material->emissiveColor.greenF());
            shape->material.emissiveColor[2] = float(material->emissiveColor.blueF());
            shape->material.emissiveColor[3] = 1.0;
            shape->material.specularColor[0] = float(material->specularColor.redF());
            shape->material.specularColor[1] = float(material->specularColor.greenF());
            shape->material.specularColor[2] = float(material->specularColor.blueF());
            shape->material.specularColor[3] = 1.0;
            shape->material.shininess = material->shininess * 128;
        }
        ImageTexture *tex = dynamic_cast<ImageTexture*>(appearance->texture());
        if (tex)
        {
            shape->texture.load(mUrlPrefix + "/" + tex->url);
            shape->texture = shape->texture.mirrored();
        }
    }

    IndexedFaceSet *mesh = dynamic_cast<IndexedFaceSet*>(node->geometry());
    if (mesh)
    {
        QVector3D pt, norm;
        QVector2D texCoord;
        shape->normalPerVertex = mesh->normalPerVertex;
        bool normalIndexEmpty = mesh->normalIndex.isEmpty();
        bool texCoordIndexEmpty = mesh->texCoordIndex.isEmpty();

        for (int i=0; i<mesh->coordIndex.count(); i++)
        {
            SFInt32 idx = mesh->coordIndex[i];
            if (idx < 0)
                continue;

            pt = mesh->coord()->point[idx];
            shape->points << GLfloat3(pt.x(), pt.y(), pt.z());

            if (mesh->normal() && mesh->normalPerVertex)
            {
                if (normalIndexEmpty)
                    norm = mesh->normal()->vector[idx];
                else
                    norm = mesh->normal()->vector[mesh->normalIndex[i]];
            }
            shape->normals << GLfloat3(norm.x(), norm.y(), norm.z());

            if (mesh->texCoord()) // assuming texCoord per vertex
            {
                if (texCoordIndexEmpty)
                    texCoord = mesh->texCoord()->point[idx];
                else
                    texCoord = mesh->texCoord()->point[mesh->texCoordIndex[i]];
            }
            shape->texCoord << texCoord;
        }

        // indexed array filling
        shape->vertices.clear();
        shape->indices.clear();
        if (mesh->normalPerVertex) // only this supported!!!!!!!
        {
            for (int i = 0; i < mesh->coord()->point.count(); i++)
            {
                MeshShape::Vertex v;
                v.point = mesh->coord()->point[i];
                if (mesh->normal() && normalIndexEmpty)
                    v.normal = mesh->normal()->vector[i];
                if (mesh->texCoord() && texCoordIndexEmpty)
                    v.texCoord = mesh->texCoord()->point[i];
                shape->vertices << v;
            }

            for (int i = 0; i < mesh->coordIndex.count(); i++)
            {
                int idx = mesh->coordIndex[i];
                if (idx >= 0)
                    shape->indices << mesh->coordIndex[i];
            }
        }
    }
}
