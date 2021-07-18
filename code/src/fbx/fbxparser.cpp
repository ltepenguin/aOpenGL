#include "fbxparser.h"
#include <iostream>
#include <assert.h>

namespace a::gl {

using FbxScene         = ::FbxScene;
using FbxDouble3       = ::FbxDouble3;
using FbxNode          = ::FbxNode;
using FbxNodeAttribute = ::FbxNodeAttribute;
using EOrder           = ::fbxsdk::FbxEuler::EOrder;
using FbxImporter      = ::FbxImporter;

static void bakeNode(FbxNode* pNode);

static void checkSameName(FbxNode* pNode, std::map<std::string, int>& counter)
{
    std::string name(pNode->GetName());
    auto iter = counter.find(name);
    if(iter == counter.end())
    {
        counter.insert(std::make_pair(name, 0));
    }
    else
    {
        counter.at(name)++;
        
        std::string newName = name + std::string("_") + std::to_string(counter.at(name));
        pNode->SetName(newName.c_str());
        printf("node name changed: %s -> %s\n", name.c_str(), newName.c_str());
    }
    
    for(int i = 0, n = pNode->GetChildCount(); i < n; ++i)
    {
        checkSameName(pNode->GetChild(i), counter);
    }
}

FBXParser::FBXParser(std::string filepath)
{
    std::cout << "fbx file import: " << filepath << std::endl;

    // Initialize the SDK manager. This object handles memory management.
    m_manager = ::fbxsdk::FbxManager::Create();

    // Create the IO settings object.
    ::fbxsdk::FbxIOSettings *ios = ::fbxsdk::FbxIOSettings::Create(m_manager, IOSROOT);
    m_manager->SetIOSettings(ios);

    // Create an importer using the SDK manager.
    FbxImporter* importer = FbxImporter::Create(m_manager, "");

    // Use the first argument as the filename for the importer.
    bool success = importer->Initialize(filepath.c_str(), -1, m_manager->GetIOSettings());
    
    if(!success)
    {
        std::cerr << __FILE__ << "(line " << __LINE__ << ")" << importer->GetStatus().GetErrorString() << std::endl;
        clear();
        return;
    }

    // Create a new scene so that it can be populated by the imported file.
    m_scene = FbxScene::Create(m_manager, "scene");

    // Import the contents of the file into the scene.
    importer->Import(m_scene);

    // time setting (60 fps)
    ::fbxsdk::FbxGlobalSettings* lTimeSettings = &m_scene->GetGlobalSettings();
    ::fbxsdk::FbxTime::EMode lTimeMode = lTimeSettings->GetTimeMode();
    ::fbxsdk::FbxTime::SetGlobalTimeMode(::fbxsdk::FbxTime::EMode::eFrames60);

    // axis setting
    ::fbxsdk::FbxAxisSystem sceneAxisSystem = m_scene->GetGlobalSettings().GetAxisSystem();
    ::fbxsdk::FbxAxisSystem::OpenGL.ConvertScene(m_scene);

    // triangulate
    ::fbxsdk::FbxGeometryConverter geometryConverter(m_manager);
    geometryConverter.Triangulate(m_scene, true);

    // bake
    m_scene->ConnectSrcObject(m_scene, fbxsdk::FbxConnection::EType::eDefault);
    bakeNode(m_scene->GetRootNode());
    
    // When the setup is done, call ConvertPivotAnimationRecursive to the scene’s root node.
    m_scene->GetRootNode()->ConvertPivotAnimationRecursive(NULL, FbxNode::eDestinationPivot, 60, true);

    // Name check
    std::map<std::string, int> nameCounter;
    checkSameName(m_scene->GetRootNode(), nameCounter);

    // The file is imported, so get rid of the importer.
    importer->Destroy();
}

void FBXParser::clear()
{
    // Destroy the SDK manager and all the other objects it was handling.
    if(m_manager)
    {
        m_manager->Destroy();
        m_manager = nullptr;
    }
}

void bakeNode(FbxNode* pNode)
{
    // Do this setup for each node (FbxNode).
    // We set up what we want to bake via ConvertPivotAnimationRecursive.
    // When the destination is set to 0, baking will occur.
    // When the destination value is set to the source’s value, the source values will be retained and not baked.
    const static ::fbxsdk::FbxVector4 lZero(0,0,0);

    // Activate pivot converting
    pNode->SetPivotState(FbxNode::eSourcePivot, FbxNode::ePivotActive);
    pNode->SetPivotState(FbxNode::eDestinationPivot, FbxNode::ePivotActive);

    // We want to set all these to 0 and bake them into the transforms.
    pNode->SetPostRotation(FbxNode::eDestinationPivot, lZero);
    pNode->SetPreRotation(FbxNode::eDestinationPivot, pNode->GetPreRotation(FbxNode::eSourcePivot));
    pNode->SetRotationOffset(FbxNode::eDestinationPivot, lZero);
    pNode->SetScalingOffset(FbxNode::eDestinationPivot, lZero);
    pNode->SetRotationPivot(FbxNode::eDestinationPivot, lZero);
    pNode->SetScalingPivot(FbxNode::eDestinationPivot, lZero);

    // This is to import in a system that supports rotation order.
    // If rotation order is not supported, do this instead:
    // pNode->SetRotationOrder(FbxNode::eDestinationPivot, FbxNode::eEulerXYZ);
    ::fbxsdk::FbxEuler::EOrder lRotationOrder;
    pNode->GetRotationOrder(FbxNode::eSourcePivot, lRotationOrder);
    pNode->SetRotationOrder(FbxNode::eDestinationPivot, lRotationOrder);

    // Similarly, this is the case where geometric transforms are supported by the system.
    // If geometric transforms are not supported, set them to zero instead of
    // the source’s geometric transforms.
    // Geometric transform = local transform, not inherited by children.
    pNode->SetGeometricTranslation(FbxNode::eDestinationPivot, pNode->GetGeometricTranslation(FbxNode::eSourcePivot));
    pNode->SetGeometricRotation(FbxNode::eDestinationPivot, pNode->GetGeometricRotation(FbxNode::eSourcePivot));
    pNode->SetGeometricScaling(FbxNode::eDestinationPivot, pNode->GetGeometricScaling(FbxNode::eSourcePivot));
    
    // Idem for quaternions.
    //pNode->SetUseQuaternionForInterpolation(FbxNode::eDestinationPivot, pNode->GetUseQuaternionForInterpolation(FbxNode::eSourcePivot));
    pNode->SetQuaternionInterpolation(FbxNode::eDestinationPivot, pNode->GetQuaternionInterpolation(FbxNode::eSourcePivot));
    
    //auto pivots = pNode->GetPivots();
    pNode->ConvertPivotAnimationRecursive(NULL, FbxNode::eDestinationPivot, 60, false);

    for(int i = 0; i < pNode->GetChildCount(); ++i)
    {
        bakeNode(pNode->GetChild(i));
    }
}

}