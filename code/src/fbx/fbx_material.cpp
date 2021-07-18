#include "fbxparser.h"

namespace a::gl::fbx {

std::vector<MaterialParseInfo> getMaterials(FbxGeometry* pGeometry)
{
    std::vector<MaterialParseInfo> materials;

    int lMaterialCount = 0;
    FbxNode* lNode = NULL;
    if(pGeometry)
    {
        lNode = pGeometry->GetNode();
        if(lNode)
        {
            lMaterialCount = lNode->GetMaterialCount();    
        }
    }

    if (lMaterialCount > 0)
    {
        FbxPropertyT<FbxDouble3> lKFbxDouble3;
        FbxPropertyT<FbxDouble> lKFbxDouble1;
        FbxColor theColor;

        for (int lCount = 0; lCount < lMaterialCount; lCount ++)
        {
            MaterialParseInfo info;
            info.material_id = lCount;

            FbxSurfaceMaterial *lMaterial = lNode->GetMaterial(lCount);
            info.name = std::string((char *)lMaterial->GetName());

            //Get the implementation to see if it's a hardware shader.
            const FbxImplementation* lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_HLSL);
            FbxString lImplemenationType = "HLSL";
            if(!lImplementation)
            {
                lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_CGFX);
                lImplemenationType = "CGFX";
            }

            if(lImplementation)
            {
                // No Hardware Shader
            }
            else if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
            {
                // We found a Phong material.  Display its properties.
                info.type = "Phong";

                // Display the Ambient Color
                lKFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Ambient;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                info.ambient = glm::vec3(theColor.mRed, theColor.mGreen, theColor.mBlue);

                // Display the Diffuse Color
                lKFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Diffuse;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                info.diffuse = glm::vec3(theColor.mRed, theColor.mGreen, theColor.mBlue);

                // Display the Specular Color (unique to Phong materials)
                lKFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Specular;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                info.specular = glm::vec3(theColor.mRed, theColor.mGreen, theColor.mBlue);

                // Display the Emissive Color
                lKFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Emissive;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                info.emissive = glm::vec3(theColor.mRed, theColor.mGreen, theColor.mBlue);

                //Opacity is Transparency factor now
                lKFbxDouble1 =((FbxSurfacePhong *) lMaterial)->TransparencyFactor;
                info.opacity = 1.0 - lKFbxDouble1.Get();

                // Display the Shininess
                lKFbxDouble1 =((FbxSurfacePhong *) lMaterial)->Shininess;
                info.shininess = lKFbxDouble1.Get();

                // Display the Reflectivity
                lKFbxDouble1 =((FbxSurfacePhong *) lMaterial)->ReflectionFactor;
                info.reflectivity = lKFbxDouble1.Get();

                //Material* material = new Material(info);
                materials.push_back(info);
            }
            else if(lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) )
            {
                // We found a Lambert material. Display its properties.
                info.type = "Lambert";

                // Display the Ambient Color
                lKFbxDouble3=((FbxSurfaceLambert *)lMaterial)->Ambient;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                info.ambient = glm::vec3(theColor.mRed, theColor.mGreen, theColor.mBlue);

                // Display the Diffuse Color
                lKFbxDouble3 =((FbxSurfaceLambert *)lMaterial)->Diffuse;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                info.diffuse = glm::vec3(theColor.mRed, theColor.mGreen, theColor.mBlue);

                // Display the Emissive
                lKFbxDouble3 =((FbxSurfaceLambert *)lMaterial)->Emissive;
                theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
                info.emissive = glm::vec3(theColor.mRed, theColor.mGreen, theColor.mBlue);

                // Display the Opacity
                lKFbxDouble1 =((FbxSurfaceLambert *)lMaterial)->TransparencyFactor;
                info.opacity = 1.0 - lKFbxDouble1.Get();
            
                //Material* material = new Material(info);
                materials.push_back(info);
            }
            else
            {
                printf("Unknown material type\n");
            }
        }
    }    
    return materials;
}

std::vector<int> getPolygonMaterialConnection(FbxMesh* pMesh)
{
    int i, l, lPolygonCount = pMesh->GetPolygonCount();
    std::vector<int> matConnection;
    
    //check whether the material maps with only one mesh
    bool lIsAllSame = true;
    for (l = 0; l < pMesh->GetElementMaterialCount(); l++)
    {
        FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
		if( lMaterialElement->GetMappingMode() == FbxGeometryElement::eByPolygon) 
		{
			lIsAllSame = false;
			break;
		}
    }

    //For eAllSame mapping type, just out the material and texture mapping info once
    if(lIsAllSame)
    {
        if(pMesh->GetElementMaterialCount() == 0)
        {
            // No material
            matConnection = std::vector<int>(lPolygonCount, -1);
        }
        else
        {
            // Support only single material per vertex
            FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(0);
			if(lMaterialElement->GetMappingMode() == FbxGeometryElement::eAllSame)
            {
                // FbxSurfaceMaterial* lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(0));    
                int mid = lMaterialElement->GetIndexArray().GetAt(0);
                matConnection = std::vector<int>(lPolygonCount, mid);
            }
        }
    }
    //For eByPolygon mapping type, just out the material and texture mapping info once
    else
    {
        matConnection = std::vector<int>(lPolygonCount, 0);
        for (i = 0; i < lPolygonCount; i++)
        {
            int materialNum = pMesh->GetElementMaterialCount();
            
            if(materialNum != 1)
            {
                // printf("polygon %d: material %d use\n", i, pMesh->GetElementMaterialCount());
            }
            
            if(materialNum >= 1)
            {
                FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(0);
                // FbxSurfaceMaterial* lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(i));
                matConnection.at(i) = lMaterialElement->GetIndexArray().GetAt(i);
            }
        }
    }

    return matConnection;
}


}