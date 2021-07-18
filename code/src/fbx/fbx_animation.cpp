#include "fbxparser.h"
#include <iostream>

namespace a::gl {

using FbxNode      = ::FbxNode;
using FbxAnimStack = ::FbxAnimStack;
using FbxAnimCurve = ::FbxAnimCurve;
using FbxProperty  = ::FbxProperty;
using FbxAnimLayer = ::FbxAnimLayer;
using FbxScene     = ::FbxScene;

static spSceneKeyFrames getSceneAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, float scale);
static void getAnimations(spSceneKeyFrames scene_keyframes, FbxAnimLayer* pAnimLayer, FbxNode* pNode, float scale);
static spNodeKeyFrames getKeyFrameAnimation(FbxNode* pNode, FbxAnimLayer* pAnimLayer, float scale);
static std::vector<KeyFrame> getKeyFrames(FbxAnimCurve* pCurve, float scale, bool debug = false);

void FBXParser::keyframes(std::vector<spSceneKeyFrames>& data, float scale)
{
    data.clear();
    FbxScene* pScene = m_scene;

    int n = pScene->GetSrcObjectCount<FbxAnimStack>();
    data.reserve(n);
    for (int i = 0; i < n; i++)
    {
        FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);
        
        auto skf = getSceneAnimation(lAnimStack, pScene->GetRootNode(), scale);
        data.push_back(skf);
    }
}

spSceneKeyFrames getSceneAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, float scale)
{
    int nbAnimLayers = pAnimStack->GetMemberCount<::FbxAnimLayer>();
    
    if(nbAnimLayers > 1)
    {
        std::cout << __FILE__ << "(line: " << __LINE__ 
            << "): cannot import multiple animation layers. (FbxAnimLayer number: " << nbAnimLayers << ")" << std::endl;
    }

    auto scene_keyframes = std::make_shared<SceneKeyFrames>();
    scene_keyframes->name = std::string(pAnimStack->GetName());

    for (int l = 0; l < nbAnimLayers; l++)
    {
        FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);
        getAnimations(scene_keyframes, lAnimLayer, pNode, scale);
    }
    
    char lTimeString[256];    
    pAnimStack->LocalStart.Get().GetTimeString(lTimeString, FbxUShort(256));
    float startFrame = std::atof(lTimeString);
    pAnimStack->LocalStop.Get().GetTimeString(lTimeString, FbxUShort(256));
    float endFrame = std::atof(lTimeString);
    
    float dt = 1.0f / 60.0f;
    //scene_keyframes->dt = dt;
    scene_keyframes->start_time = dt * startFrame;
    scene_keyframes->end_time = dt * endFrame;
    return scene_keyframes;
}

void getAnimations(spSceneKeyFrames scene_keyframes, FbxAnimLayer* pAnimLayer, FbxNode* pNode, float scale)
{
    int lModelCount;
    FbxString lOutputString;
    spNodeKeyFrames animation = getKeyFrameAnimation(pNode, pAnimLayer, scale);
    
    scene_keyframes->node_keyframes.push_back(animation);   
    for(lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
    {
        getAnimations(scene_keyframes, pAnimLayer, pNode->GetChild(lModelCount), scale);
    }
}

spNodeKeyFrames getKeyFrameAnimation(FbxNode* pNode, FbxAnimLayer* pAnimLayer, float scale)
{
    auto nkf = std::make_shared<NodeKeyFrames>();
    nkf->name = pNode->GetName();
    
    bool isRoot = false;
    if(nkf->name == std::string("mixamorig:Hips"))
        isRoot = true;

    // Euler order

    using EOrder = ::fbxsdk::FbxEuler::EOrder;
    EOrder order;
    pNode->GetRotationOrder(::fbxsdk::FbxNode::EPivotSet::eSourcePivot, order);
    
    switch(order)
    {
    case EOrder::eOrderXYZ:
        nkf->euler_order = glm::ivec3(0, 1, 2);
        break;
    case EOrder::eOrderXZY:
        nkf->euler_order = glm::ivec3(0, 2, 1);
        break;
    case EOrder::eOrderYXZ:
        nkf->euler_order = glm::ivec3(1, 0, 2);
        break;
    case EOrder::eOrderYZX:
        nkf->euler_order = glm::ivec3(1, 2, 0);
        break;
    case EOrder::eOrderZXY:
        nkf->euler_order = glm::ivec3(2, 0, 1);
        break;
    case EOrder::eOrderZYX:
        nkf->euler_order = glm::ivec3(2, 1, 0);
        break;
    default:
        nkf->euler_order = glm::ivec3(0, 1, 2);
        break;
    }

    // get keyframes

    FbxAnimCurve* lAnimCurve = NULL;
    lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
    if (lAnimCurve)
    {
        nkf->pos[0] = getKeyFrames(lAnimCurve, scale);
    }
    lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
    if (lAnimCurve)
    {
        nkf->pos[1] = getKeyFrames(lAnimCurve, scale);
    }
    lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
    if (lAnimCurve)
    {
        nkf->pos[2] = getKeyFrames(lAnimCurve, scale);
    }

    lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
    if (lAnimCurve)
    {
        nkf->euler[0] = getKeyFrames(lAnimCurve, 1.0, isRoot);
    }

    lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
    if (lAnimCurve)
    {
        nkf->euler[1] = getKeyFrames(lAnimCurve, 1.0);
    }
    lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
    if (lAnimCurve)
    {
        nkf->euler[2] = getKeyFrames(lAnimCurve, 1.0);
    }

    lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
    if (lAnimCurve)
    {
        nkf->scale[0] = getKeyFrames(lAnimCurve, 1.0);
    }    

    lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
    if (lAnimCurve)
    {
        nkf->scale[1] = getKeyFrames(lAnimCurve, 1.0);
    }
    
    lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
    if (lAnimCurve)
    {
        nkf->scale[2] = getKeyFrames(lAnimCurve, 1.0);
    }

    return nkf;
}

static ::a::gl::KeyInterpType getInterpolationType(int flags)
{
    using FbxAnimCurveDef = ::FbxAnimCurveDef;
	if( (flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant ) return KeyInterpType::kConstant;
    if( (flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear ) return KeyInterpType::kLinear;
	if( (flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic ) return KeyInterpType::kCubic;
    return KeyInterpType::kUnknown;
}

std::vector<KeyFrame> getKeyFrames(FbxAnimCurve* pCurve, float scale, bool debug)
{
    std::vector<KeyFrame> keys;
    if(pCurve == nullptr)
        return keys;
    
    int lKeyCount = pCurve->KeyGetCount();
    keys.reserve(lKeyCount);
   
    for(int lCount = 0; lCount < lKeyCount; lCount++)
    {
        KeyFrame key;
        key.value = scale * static_cast<float>(pCurve->KeyGetValue(lCount));

        ::fbxsdk::FbxTime lKeyTime = pCurve->KeyGetTime(lCount);
        key.time = (float)lKeyTime.GetSecondDouble();
        key.type = getInterpolationType(pCurve->KeyGetInterpolation(lCount));
        keys.push_back(key);
    }

    return keys;
}



}