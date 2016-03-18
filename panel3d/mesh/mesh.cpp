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
//----------------------------------------------------------

void Mesh::clear()
{
    for (int i=0; i<Shapes.count(); i++)
        delete Shapes[i];
    Shapes.clear();
}

void Mesh::load(QTextStream *stream)
{
    clear();
    // if stream is vrml2.0
    loadVrml2(stream);
}

//void Mesh::scaleUniform(float factor)
//{
//    for (int i=0; i<Shapes.count(); i++)
//    {
//        MeshShape *shape = Shapes[i];
//        for (int j=0; j<shape->faces.count(); j++)
//        {
//            MeshFace *face = shape->faces[j];
//            for (int k=0; k<face->vertices.count(); k++)
//            {
//                MeshVertex *vertex = face->vertices[k];
//                for (int v=0; v<3; v++)
//                    vertex->point[v] *= factor;
//            }
//        }
//    }
//}

void Mesh::loadVrml2(QTextStream *stream)
{
    Vrml2Parser *parser = new Vrml2Parser;
    parser->parse(stream);
    loadChildren(parser->sceneGraph());
    delete parser;
}
//----------------------------------------------------------

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
            shape->material.diffuseColor[0] = material->diffuseColor.redF();
            shape->material.diffuseColor[1] = material->diffuseColor.greenF();
            shape->material.diffuseColor[2] = material->diffuseColor.blueF();
            shape->material.diffuseColor[3] = 1.0 - material->transparency;
            shape->material.ambientColor[0] = shape->material.diffuseColor[0] * material->ambientIntensity;
            shape->material.ambientColor[1] = shape->material.diffuseColor[1] * material->ambientIntensity;
            shape->material.ambientColor[2] = shape->material.diffuseColor[2] * material->ambientIntensity;
            shape->material.ambientColor[3] = shape->material.diffuseColor[3];
            shape->material.emissiveColor[0] = material->emissiveColor.redF();
            shape->material.emissiveColor[1] = material->emissiveColor.greenF();
            shape->material.emissiveColor[2] = material->emissiveColor.blueF();
            shape->material.emissiveColor[3] = 1.0;
            shape->material.specularColor[0] = material->specularColor.redF();
            shape->material.specularColor[1] = material->specularColor.greenF();
            shape->material.specularColor[2] = material->specularColor.blueF();
            shape->material.specularColor[3] = 1.0;
            shape->material.shininess = material->shininess * 128;
        }
    }

    IndexedFaceSet *mesh = dynamic_cast<IndexedFaceSet*>(node->geometry());
    if (mesh)
    {
        QVector3D pt, norm;
        shape->normalPerVertex = mesh->normalPerVertex;
        bool normalIndexEmpty = mesh->normalIndex.isEmpty();

        // !!!!!! only triangles supported for speeding up!!!!
        for (int i=0; i<mesh->coordIndex.count(); i++)
        {
            SFInt32 idx = mesh->coordIndex[i];
            if (idx < 0)
                continue;

            pt = mesh->coord()->point[idx];
            shape->vertices << GLfloat3(pt.x(), pt.y(), pt.z());

            if (mesh->normal() && mesh->normalPerVertex)
            {
                if (normalIndexEmpty)
                    norm = mesh->normal()->vector[idx];
                else
                    norm = mesh->normal()->vector[mesh->normalIndex[i]];
            }
            shape->normals << GLfloat3(norm.x(), norm.y(), norm.z());
        }



//        for (int i=0; i<mesh->coordIndex.count(); i++)
//        {
//            MeshFace *face = new MeshFace;
//            shape->faces << face;
////            if (!mesh->normalPerVertex)
////            {
////                face->normal = blabla;
////                if (!mesh->normalIndex.isEmpty())

////            }

//            while (i < mesh->coordIndex.count())
//            {
//                SFInt32 idx = mesh->coordIndex[i];
//                if (idx < 0)
//                    break;

//                MeshVertex *vertex = new MeshVertex;
//                shape->vertices << vertex;
//                face->vertices << vertex;

//                pt = mesh->coord()->point[idx];
//                vertex->point[0] = pt.x();
//                vertex->point[1] = pt.y();
//                vertex->point[2] = pt.z();

//                if (mesh->normal() && mesh->normalPerVertex)
//                {
//                    if (normalIndexEmpty)
//                        norm = mesh->normal()->vector[idx];
//                    else
//                        norm = mesh->normal()->vector[mesh->normalIndex[i]];
//                }
//                vertex->normal[0] = norm.x();
//                vertex->normal[1] = norm.y();
//                vertex->normal[2] = norm.z();

//                i++;
//            }
//        }
    }
}
//---------------------------------------------------------------------------

//void Mesh::operator =(Mesh &mesh)
//void Mesh::copy(Mesh *mesh)
//{
//    clear();
//    for (int i=0; i<mesh->shapes().count(); i++)
//    {
//        MeshShape *oshape = mesh->shapes()[i];
//        MeshShape *shape = new MeshShape;
//        Shapes << shape;

//        shape->material = oshape->material;
//        shape->normalPerVertex = oshape->normalPerVertex;

//        for (int j=0; j<oshape->faces.count(); j++)
//        {
//            MeshFace *oface = oshape->faces[j];
//            MeshFace *face = new MeshFace;
//            shape->faces << face;
//            for (int idx=0; idx<3; idx++)
//            {
//                face->color[idx] = oface->color[idx];
//                face->normal[idx] = oface->normal[idx];
//            }
//            for (int k=0; k<oface->vertices.count(); k++)
//            {
//                MeshVertex *overtex = oface->vertices[k];
//                MeshVertex *vertex = new MeshVertex;
//                face->vertices << vertex;
//                *vertex = *overtex;
//            }
//        }
//    }
//}
//---------------------------------------------------------------------------
