#include "fbxparser.h"
#include <iostream>

namespace a::gl {

using FbxGeometry      = ::fbxsdk::FbxGeometry;
using FbxSkin          = ::fbxsdk::FbxSkin;
using FbxDeformer      = ::fbxsdk::FbxDeformer;
using EOrder           = ::fbxsdk::FbxEuler::EOrder;
using FbxScene         = ::FbxScene;
using FbxDouble3       = ::FbxDouble3;
using FbxNode          = ::FbxNode;
using FbxImporter      = ::FbxImporter;
using FbxNodeAttribute = ::FbxNodeAttribute;

const static float toRad = M_PI / 180.0;

static glm::quat toQuat(const FbxDouble3& x, EOrder order)
{
    glm::quat rx = glm::angleAxis(toRad * (float)x[0], glm::vec3(1, 0, 0));
    glm::quat ry = glm::angleAxis(toRad * (float)x[1], glm::vec3(0, 1, 0));
    glm::quat rz = glm::angleAxis(toRad * (float)x[2], glm::vec3(0, 0, 1));

    switch(order)
    {
    case EOrder::eOrderXYZ:
        return rz * ry * rx;
    case EOrder::eOrderXZY:
        return ry * rz * rx;
    case EOrder::eOrderYXZ:
        return rz * rx * ry;
    case EOrder::eOrderYZX:
        return rx * rz * ry;
    case EOrder::eOrderZXY:
        return ry * rx * rz;
    case EOrder::eOrderZYX:
        return rx * ry * rz;
    default:
        break;
    }
    
    return glm::quat();
}

static glm::vec3 toVec3(const FbxDouble3& x)
{
    return glm::vec3(x[0], x[1], x[2]);
}

static void parseNodesByType(FbxNode* node, 
                             std::vector<JointData>& joints, 
                             int parIndex, 
                             FbxNodeAttribute::EType type,
                             float scale)
{
    bool isType = false;
    
    for(int i = 0; i < node->GetNodeAttributeCount(); ++i)
    {
        auto attrib = node->GetNodeAttributeByIndex(i);
        if(attrib->GetAttributeType() == type)
        {
            isType = true;
            break;
        }
    }

    if(isType)
    {
        std::string name(node->GetName());
        EOrder order = EOrder::eOrderXYZ;
        node->GetRotationOrder(FbxNode::eDestinationPivot, order);
        glm::quat localR = toQuat(node->LclRotation.Get(), order);
        glm::vec3 localS = toVec3(node->LclScaling.Get());
        glm::vec3 localT = toVec3(node->LclTranslation.Get());
        
        ::fbxsdk::FbxVector4 destPreR = node->GetPreRotation(FbxNode::eDestinationPivot);
        glm::quat preR = toQuat(destPreR, order);
        
        JointData jnt;
        jnt.name = name;
        jnt.local_S = localS;
        jnt.local_T = scale * localT;
        jnt.local_R = localR;
        jnt.pre_R = preR;
        jnt.parent_index = parIndex;
        joints.push_back(jnt);
        parIndex = joints.size() - 1;
    }
    else
    {
        return;
    }
    
    for(int i = 0; i < node->GetChildCount(); ++i)
    {
        parseNodesByType(node->GetChild(i), joints, parIndex, type, scale);
    }
}

void FBXParser::character_data(CharacterData& data, float scale)
{   
    auto root = m_scene->GetRootNode();
    data.name = root->GetName();
    for(int i = 0; i < root->GetChildCount(); ++i)
    {
        parseNodesByType(root->GetChild(i), data.joint_data, -1, FbxNodeAttribute::eSkeleton, scale);
    }
}

}