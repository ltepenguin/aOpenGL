#include "fbxparser.h"
#include <iostream>

namespace a::gl {

using FbxMesh = ::fbxsdk::FbxMesh;
using FbxNode = ::fbxsdk::FbxNode;

static glm::vec2 mesh_uvmap(FbxMesh* pMesh, int lControlPointIndex, int i, int j);
//static glm::vec4 mesh_vcolor(FbxMesh* pMesh, int lControlPointIndex, int vertexId); // not used
static glm::vec3 mesh_normal(FbxMesh* pMesh, int vertexId, int i, int j);
static glm::vec3 mesh_tangent(FbxMesh* pMesh, int vertexId, int i, int j);
//static glm::vec3 mesh_binormal(FbxMesh* pMesh, int vertexId); // not used

static void get_mesh_data(FbxMesh* pMesh, MeshData& data, float scale)
{
    if(pMesh->GetElementNormalCount() == 0)
    {
        if(pMesh->GenerateNormals() == false)
        {
            std::cout << __FILE__ << " (line: " << __LINE__ <<"): Fbx normal import error." << std::endl;
        }
    }

    if(pMesh->GetElementTangentCount() == 0)
    {
        if(pMesh->GenerateTangentsDataForAllUVSets() == false)
        {
            std::cout << __FILE__ << " (line: " << __LINE__ <<"): Fbx tangent import error." << std::endl;
        }
    }

    const int lPolygonCount = pMesh->GetPolygonCount();
    FbxVector4* lControlPoints = pMesh->GetControlPoints();

    data.controlPointIdx2vertexId.clear();
    
    data.indices.clear();
    data.positions.clear();
    data.normals.clear();
    data.uvs.clear();
    data.tangents.clear();
    data.bitangents.clear();

    data.indices.resize(lPolygonCount * 3);
    data.positions.resize(lPolygonCount * 3);
    data.normals.resize(lPolygonCount * 3);
    data.uvs.resize(lPolygonCount * 3);
    data.tangents.resize(lPolygonCount * 3);
    data.bitangents.resize(lPolygonCount * 3);

    int vertexId = 0;
    for(int i = 0; i < lPolygonCount; i++)
    {
        int lPolygonSize = pMesh->GetPolygonSize(i);
		for (int j = 0; j < lPolygonSize; j++)
		{
            assert(lPolygonSize == 3); // triangle only
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

            data.indices.at(vertexId) = vertexId;
            data.controlPointIdx2vertexId.insert(std::make_pair(lControlPointIndex, vertexId));

            data.positions.at(vertexId) = glm::vec3(lControlPoints[lControlPointIndex].mData[0],
                                                    lControlPoints[lControlPointIndex].mData[1],
                                                    lControlPoints[lControlPointIndex].mData[2]);
            data.positions.at(vertexId) = scale * data.positions.at(vertexId);

            data.uvs.at(vertexId)        = mesh_uvmap(pMesh, lControlPointIndex, i, j);
            auto vN = mesh_normal(pMesh, vertexId, i, j);
            auto vT = mesh_tangent(pMesh, vertexId, i, j);
            data.normals.at(vertexId)    = vN;
            data.tangents.at(vertexId)   = vT;
            data.bitangents.at(vertexId) = glm::normalize(glm::cross(vN, vT));            
            vertexId++;
		}
    }
    assert(vertexId == (lPolygonCount * 3));
}

void FBXParser::mesh_data(std::vector<MeshData>& meshes, float scale)
{
    std::vector<FbxNode*> meshNodes;

    auto root = m_scene->GetRootNode();
    for(int i = 0; i < root->GetChildCount(); ++i)
    {
        FbxNode* node = root->GetChild(i);
        
        bool isMesh = false;
        for(int j = 0; j < node->GetNodeAttributeCount(); ++j)
        {
            auto attrib = node->GetNodeAttributeByIndex(j);
            if(attrib->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                isMesh = true;
            }
        }        
        if(isMesh)
        {
            meshNodes.push_back(node);
        }
    }

    meshes.clear();
    meshes.resize(meshNodes.size());
    for(int i = 0; i < (int)meshes.size(); ++i)
    {
        auto node = meshNodes.at(i);
        FbxMesh* pMesh = node->GetMesh();

        // read materials and textures

        std::vector<TextureParseInfo> textures = ::a::gl::fbx::getTextures(pMesh);
        std::vector<MaterialParseInfo> materials = ::a::gl::fbx::getMaterials(pMesh);
        std::vector<int> materialConnection = ::a::gl::fbx::getPolygonMaterialConnection(pMesh);

        for(int i = 0; i < (int)textures.size(); ++i)
        {
            bool textureSet = false;
            for(int j = 0; j < (int)materials.size(); ++j)
            {
                if(textures.at(i)._connected_material == materials.at(j).material_id)
                {
                    //materials.at(j)->addTexture(textures.at(i));
                    materials.at(j).textureIDs.push_back(i);
                    textureSet = true;
                    break;
                }
            }
            
            if(!textureSet)
            {
                printf("texture is not used %s\n", textures.at(i).fileName.c_str());
            }
            textures.at(i).is_used = textureSet;
        }

        get_mesh_data(pMesh, meshes.at(i), scale);
        meshes.at(i).textures = textures;
        meshes.at(i).materials = materials;
        meshes.at(i).polygonMaterialConnection = materialConnection;
        
        int vnum = meshes.at(i).positions.size();
        
        meshes.at(i).is_skinned =
            fbx::getSkinning(meshes.at(i).skinning_data, 
                             pMesh, 
                             meshes.at(i).controlPointIdx2vertexId, 
                             vnum, 
                             scale);
    }
}

#if 0
glm::vec4 mesh_vcolor(FbxMesh* pMesh, int lControlPointIndex, int vertexId)
{
    glm::vec4 color(0, 0, 0, 1);
    for (int l = 0; l < pMesh->GetElementVertexColorCount(); l++)
    {
        ::fbxsdk::FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor( l);
        //FBXSDK_sprintf(header, 100, "            Color vertex: "); 

        switch (leVtxc->GetMappingMode())
        {
        default:
            break;
        case FbxGeometryElement::eByControlPoint:
            switch (leVtxc->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                // DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
                {
                    auto val = leVtxc->GetDirectArray().GetAt(lControlPointIndex);
                    color[0] = val.mRed;
                    color[1] = val.mGreen;
                    color[2] = val.mBlue;
                    color[3] = val.mAlpha;
                }
                break;
            case FbxGeometryElement::eIndexToDirect:
                {
                    int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
                    // DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
                    {
                        auto val = leVtxc->GetDirectArray().GetAt(id);
                        color[0] = val.mRed;
                        color[1] = val.mGreen;
                        color[2] = val.mBlue;
                        color[3] = val.mAlpha;
                    }
                }
                break;
            default:
                break; // other reference modes not shown here!
            }
            break;

        case FbxGeometryElement::eByPolygonVertex:
            {
                switch (leVtxc->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    // DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
                    {
                        auto val = leVtxc->GetDirectArray().GetAt(vertexId);
                        color[0] = val.mRed;
                        color[1] = val.mGreen;
                        color[2] = val.mBlue;
                        color[3] = val.mAlpha;
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int id = leVtxc->GetIndexArray().GetAt(vertexId);
                        // DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
                        {
                            auto val = leVtxc->GetDirectArray().GetAt(id);
                            color[0] = val.mRed;
                            color[1] = val.mGreen;
                            color[2] = val.mBlue;
                            color[3] = val.mAlpha;
                        }
                    }
                    break;
                default:
                    break; // other reference modes not shown here!
                }
            }
            break;
        case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
        case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
        case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
            break;
        }
    }
    return color;
}
#endif

glm::vec2 mesh_uvmap(FbxMesh* pMesh, int lControlPointIndex, int i, int j)
{
    glm::vec2 uv(0, 0);
    for (int l = 0; l < pMesh->GetElementUVCount(); ++l)
    {
        ::fbxsdk::FbxGeometryElementUV* leUV = pMesh->GetElementUV( l);
        // FBXSDK_sprintf(header, 100, "            Texture UV: "); 

        switch (leUV->GetMappingMode())
        {
        default:
            break;
        case FbxGeometryElement::eByControlPoint:
            switch (leUV->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                // Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
                {
                    auto uvValue = leUV->GetDirectArray().GetAt(lControlPointIndex);
                    uv[0] = uvValue[0];
                    uv[1] = uvValue[1];
                }
                break;
            case FbxGeometryElement::eIndexToDirect:
                {
                    int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
                    // Display2DVector(header, leUV->GetDirectArray().GetAt(id));
                    auto uvValue = leUV->GetDirectArray().GetAt(id);
                    uv[0] = uvValue[0];
                    uv[1] = uvValue[1];
                }
                break;
            default:
                break; // other reference modes not shown here!
            }
            break;

        case FbxGeometryElement::eByPolygonVertex:
            {
                int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
                switch (leUV->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                case FbxGeometryElement::eIndexToDirect:
                    {
                        // Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
                        auto uvValue = leUV->GetDirectArray().GetAt(lTextureUVIndex);
                        uv[0] = uvValue[0];
                        uv[1] = uvValue[1];
                    }
                    break;
                default:
                    break; // other reference modes not shown here!
                }
            }
            break;

        case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
        case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
        case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
            break;
        }
    }
    return uv;
}

glm::vec3 mesh_normal(FbxMesh* pMesh, int vertexId, int i, int j)
{
    glm::vec3 normal(0, 1, 0);

    for(int l = 0; l < pMesh->GetElementNormalCount(); ++l)
    {
        ::fbxsdk::FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal( l);
        // FBXSDK_sprintf(header, 100, "            Normal: "); 

        if(leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
        {
            switch (leNormal->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                // Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
                {
                    auto val = leNormal->GetDirectArray().GetAt(vertexId);
                    normal[0] = val.mData[0];
                    normal[1] = val.mData[1];
                    normal[2] = val.mData[2];
                }
                break;
            case FbxGeometryElement::eIndexToDirect:
                {
                    int id = leNormal->GetIndexArray().GetAt(vertexId);
                    //Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
                    {
                        auto val = leNormal->GetDirectArray().GetAt(id);
                        normal[0] = val.mData[0];
                        normal[1] = val.mData[1];
                        normal[2] = val.mData[2];
                    }
                }
                break;
            default:
                break; // other reference modes not shown here!
            }
        }
        else if (leNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
        {
            int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

            int lNormalIndex = 0;
            if(leNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
                lNormalIndex = lControlPointIndex;
            else if(leNormal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                lNormalIndex = leNormal->GetIndexArray().GetAt(lControlPointIndex);

            //Got normals of each vertex.
            FbxVector4 lNormal = leNormal->GetDirectArray().GetAt(lNormalIndex);
            normal[0] = lNormal.mData[0];
            normal[1] = lNormal.mData[1];
            normal[2] = lNormal.mData[2];
        }        
    }
    
    return glm::normalize(normal);
}

glm::vec3 mesh_tangent(FbxMesh* pMesh, int vertexId, int i, int j)
{
    glm::vec3 tangent(1, 0, 0);

    //printf("pMesh->GetElementTangentCount(): %d\n", pMesh->GetElementTangentCount());

    for(int l = 0; l < pMesh->GetElementTangentCount(); ++l)
    {
        ::fbxsdk::FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent( l);
        // FBXSDK_sprintf(header, 100,v "            Tangent: "); 

        if(leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
        {
            switch (leTangent->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                //Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));
                {
                    auto val = leTangent->GetDirectArray().GetAt(vertexId);
                    
                    if(val.mData[3] > 0)
                    {
                        tangent[0] = val.mData[0];
                        tangent[1] = val.mData[1];
                        tangent[2] = val.mData[2];
                    }
                    else
                    {
                        tangent[0] = -val.mData[0];
                        tangent[1] =  val.mData[1];
                        tangent[2] =  val.mData[2];                      
                    }
                }
                break;
            case FbxGeometryElement::eIndexToDirect:
                {
                    int id = leTangent->GetIndexArray().GetAt(vertexId);
                    // Display3DVector(header, leTangent->GetDirectArray().GetAt(id));
                    {
                        auto val = leTangent->GetDirectArray().GetAt(id);
                        
                        if(val.mData[3] > 0)
                        {
                            tangent[0] = val.mData[0];
                            tangent[1] = val.mData[1];
                            tangent[2] = val.mData[2];
                        }
                        else
                        {
                            tangent[0] = -val.mData[0];
                            tangent[1] = val.mData[1];
                            tangent[2] = val.mData[2];
                        }

                    }

                }
                break;
            default:
                break; // other reference modes not shown here!
            }
        }
        else if (leTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
        {
            int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

            int tidx = 0;
            if(leTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
                tidx = lControlPointIndex;
            else if(leTangent->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                tidx = leTangent->GetIndexArray().GetAt(lControlPointIndex);

            //Got normals of each vertex.
            FbxVector4 lTangent = leTangent->GetDirectArray().GetAt(tidx);
            if(lTangent.mData[3] > 0)
            {
                tangent[0] = lTangent.mData[0];
                tangent[1] = lTangent.mData[1];
                tangent[2] = lTangent.mData[2];
            }
            else
            {
                tangent[0] = -lTangent.mData[0];
                tangent[1] = lTangent.mData[1];
                tangent[2] = lTangent.mData[2];                
            }
        }
    }
   
    return glm::normalize(tangent);
}

#if 0
glm::vec3 mesh_binormal(FbxMesh* pMesh, int vertexId)
{
    glm::vec3 binormal(0, 0, 0);
    for(int l = 0; l < pMesh->GetElementBinormalCount(); ++l)
    {
        FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal( l);

        // FBXSDK_sprintf(header, 100, "            Binormal: "); 
        if(leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
        {
            switch (leBinormal->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                // Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));
                {
                    auto val = leBinormal->GetDirectArray().GetAt(vertexId);
                    binormal[0] = val.mData[0];
                    binormal[1] = val.mData[1];
                    binormal[2] = val.mData[2];
                }
                break;
            case FbxGeometryElement::eIndexToDirect:
                {
                    int id = leBinormal->GetIndexArray().GetAt(vertexId);
                    // Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));
                    {
                        auto val = leBinormal->GetDirectArray().GetAt(id);
                        binormal[0] = val.mData[0];
                        binormal[1] = val.mData[1];
                        binormal[2] = val.mData[2];
                    }
                }
                break;
            default:
                break; // other reference modes not shown here!
            }
        }
        else
        {
        }
        
    }
    return binormal;
}
#endif

}