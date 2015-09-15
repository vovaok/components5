#include "vrml2types.h"

using namespace VRML2;

ChildNode::ChildNode(GroupingNode *parent, QString className) :
    SFNodeT(className)
{
    Parent = parent;
    if (parent)
        parent->children().append(this);
}

ChildNode::~ChildNode()
{
    if (Parent)
        Parent->children().removeOne(this);
}
//----------------------------------------------------------

