#include "fbxparser.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

#include <glm/gtx/string_cast.hpp>

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

#if 1
bool fbx::getSkinning(
    SkinningData& data,
    FbxGeometry* pGeometry, 
    std::multimap<int, int>& ctrlIdx2vtxIdx,
    int vertex_num,
    float scale
    )
{
    int lClusterCount=0;
    ::fbxsdk::FbxCluster* lCluster;

    int lSkinCount = pGeometry->GetDeformerCount(FbxDeformer::eSkin);
    if(lSkinCount > 1)
    {
        std::cout << __FILE__ << " (line: " << __LINE__ << "): error. lSkinCount > 1." << std::endl;
        lSkinCount = 1;
    }

    if(lSkinCount == 0)
    {
        return false;
    }

    // create skinning data
    data.joint_indices1.clear();
    data.joint_indices2.clear();
    data.joint_weights1.clear();
    data.joint_weights2.clear();
    data.joint_indices1.resize(vertex_num, glm::ivec4(-1, -1, -1, -1));
    data.joint_indices2.resize(vertex_num, glm::ivec4(-1, -1, -1, -1));
    data.joint_weights1.resize(vertex_num, glm::vec4(0, 0, 0, 0));
    data.joint_weights2.resize(vertex_num, glm::vec4(0, 0, 0, 0));

    std::vector<int>   vertex_skinning_cnt(vertex_num, 0);
    std::vector<float> vertex_skinning_wsum(vertex_num, 0);

    for(int i = 0; i < lSkinCount; ++i)
    {
        lClusterCount = ((FbxSkin*)pGeometry->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();

        // for each joint
        for(int j = 0; j < lClusterCount; ++j)
        {
            lCluster = ((FbxSkin*)pGeometry->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);
            //lLink = pGeometry->GetLink(i);
            
            using ELinkMode = ::fbxsdk::FbxCluster::ELinkMode;
            if(lCluster->GetLinkMode() != ELinkMode::eNormalize)
            {
                std::cout << __FILE__ << " (line: " << __LINE__ << "): skinning mode unknown." << std::endl;
                lCluster->SetLinkMode(ELinkMode::eNormalize);
            }

            int jointIndex = 0;
            if(lCluster->GetLink() != NULL)
            {
                std::string jointName((char*)lCluster->GetLink()->GetName());
                auto iter = data.name2idx.find(jointName);
                if(iter == data.name2idx.end())
                {
                    data.joint_order.push_back(jointName);
                    jointIndex = data.joint_order.size() - 1;
                    data.name2idx[jointName] = jointIndex;
                }
                else
                {
                    jointIndex = iter->second;
                }
            }
            else
            {
                std::cout << __FILE__ << " (line: " << __LINE__ << "): link error." << std::endl;
                continue;
            }          

            // control point당 skinning weight
            int     lIndexCount = lCluster->GetControlPointIndicesCount();
            int*    lIndices    = lCluster->GetControlPointIndices();
            double* lWeights    = lCluster->GetControlPointWeights();

            for(int k = 0; k < lIndexCount; k++)
            {
                int controlPointIndex = lIndices[k];
                float vertexWeight = (float)lWeights[k];
                
                // control point에 해당하는 모든 vertex에 skinning weight 값 넣어주기
                auto iterPair = ctrlIdx2vtxIdx.equal_range(controlPointIndex);                
                for(auto iter = iterPair.first; iter != iterPair.second; ++iter)
                {
                    int vertexIndex = iter->second;
                    
                    switch(vertex_skinning_cnt.at(vertexIndex))
                    {
                    case 0:
                        data.joint_indices1.at(vertexIndex).x = jointIndex;
                        data.joint_weights1.at(vertexIndex).x = vertexWeight;
                        break;
                    case 1:
                        data.joint_indices1.at(vertexIndex).y = jointIndex;
                        data.joint_weights1.at(vertexIndex).y = vertexWeight;
                        break;
                    case 2:
                        data.joint_indices1.at(vertexIndex).z = jointIndex;
                        data.joint_weights1.at(vertexIndex).z = vertexWeight;
                        break;
                    case 3:
                        data.joint_indices1.at(vertexIndex).w = jointIndex;
                        data.joint_weights1.at(vertexIndex).w = vertexWeight;
                        break;
                    case 4:
                        data.joint_indices2.at(vertexIndex).x = jointIndex;
                        data.joint_weights2.at(vertexIndex).x = vertexWeight;
                        break;
                    case 5:
                        data.joint_indices2.at(vertexIndex).y = jointIndex;
                        data.joint_weights2.at(vertexIndex).y = vertexWeight;
                        break;
                    case 6:
                        data.joint_indices2.at(vertexIndex).z = jointIndex;
                        data.joint_weights2.at(vertexIndex).z = vertexWeight;
                        break;
                    case 7:
                        data.joint_indices2.at(vertexIndex).w = jointIndex;
                        data.joint_weights2.at(vertexIndex).w = vertexWeight;
                        break;
                    default:
                        break;
                    }
                    
                    vertex_skinning_wsum.at(vertexIndex) += vertexWeight;
                    vertex_skinning_cnt.at(vertexIndex)++;
                }
            }

            ::fbxsdk::FbxAMatrix lMatrix;

            // the global initial transform of the geometry node that contains the link node.

            lMatrix = lCluster->GetTransformMatrix(lMatrix);            
            ::fbxsdk::FbxQuaternion lmQ = lMatrix.GetQ();
            ::fbxsdk::FbxVector4    lmT = lMatrix.GetT();
            ::fbxsdk::FbxVector4    lmS = lMatrix.GetS();           
            
            glm::quat glmmQ(lmQ[3], lmQ[0], lmQ[1], lmQ[2]);
            glm::vec3 glmmT(lmT[0], lmT[1], lmT[2]);
            glm::vec3 glmmS(lmS[0], lmS[1], lmS[2]);
            glmmT = scale * glmmT;
            
            glm::mat4 glmgTrf = glm::translate(glm::mat4(1.0), glmmT) * 
                                glm::toMat4(glmmQ) * 
                                glm::scale(glm::mat4(1.0), glmmS);
            
            // joint transformations at binding pose

            lMatrix = lCluster->GetTransformLinkMatrix(lMatrix);
            
            ::fbxsdk::FbxQuaternion lQ = lMatrix.GetQ();
            ::fbxsdk::FbxVector4    lT = lMatrix.GetT();
            ::fbxsdk::FbxVector4    lS = lMatrix.GetS();
            
            glm::quat glmQ(lQ[3], lQ[0], lQ[1], lQ[2]);
            glm::vec3 glmT(lT[0], lT[1], lT[2]);
            glm::vec3 glmS(lS[0], lS[1], lS[2]);
            glmT = scale * glmT;
            
            glm::mat4 glmTrf = glm::translate(glm::mat4(1.0), glmT) * 
                               glm::toMat4(glmQ) * 
                               glm::scale(glm::mat4(1.0), glmS);
            
            // v_global = node_trf * jnt_global_trf * v_local
            // v_local  = jnt_global_trf.inv * node_trf.inv * v_global;
            //data.offset_transform.push_back(glm::inverse(glmTrf) * glm::inverse(glmgTrf));

            // ? not sure.
            // node_trf * v_global =  jnt_global_trf * v_local
            data.offset_transform.push_back(glm::inverse(glmTrf) * glmgTrf);
            
            if (lCluster->GetAssociateModel() != NULL)
            {
                std::cout << __FILE__ << " (line: " << __LINE__ << "): AssociateModel ?" << std::endl;
            }

            //std::cout << i << ", " << j << ":\n" << glm::to_string(glmgTrf) << std::endl;
        }
    }
    return true;
}
#endif

}