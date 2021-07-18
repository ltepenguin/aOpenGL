#include "fbxparser.h"

namespace a::gl::fbx {

using FbxProperty = fbxsdk::FbxProperty;
using FbxLayeredTexture = fbxsdk::FbxLayeredTexture;

static TextureParseInfo getFileTexture(FbxTexture* pTexture, int pBlendMode)
{
    TextureParseInfo info;

	FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(pTexture);
	FbxProceduralTexture* lProceduralTexture = FbxCast<FbxProceduralTexture>(pTexture);
    
    info.name = std::string((char*)pTexture->GetName());

	if (lFileTexture)
	{
        info.fileName = std::string((char*)lFileTexture->GetFileName());
	}
	else if (lProceduralTexture)
	{
        return info;
	}

    info.scale_u = pTexture->GetScaleU();
    info.scale_v = pTexture->GetScaleV();
    info.translation_u = pTexture->GetTranslationU();
    info.translation_v = pTexture->GetTranslationV();
    info.swap_uv = pTexture->GetSwapUV();
    info.rotation_u = pTexture->GetRotationU();
    info.rotation_v = pTexture->GetRotationV();
    info.rotation_w = pTexture->GetRotationW();

    static const char* lAlphaSources[] = { "None", "RGB Intensity", "Black" };

    info.alpha_src = std::string(lAlphaSources[pTexture->GetAlphaSource()]);
    info.crop_left = pTexture->GetCroppingLeft();
    info.crop_top = pTexture->GetCroppingTop();
    info.crop_right = pTexture->GetCroppingRight();
    info.crop_left = pTexture->GetCroppingBottom();

    static const char* lMappingTypes[] = { "Null", "Planar", "Spherical", "Cylindrical", 
                                           "Box", "Face", "UV", "Environment" };

    info.mapping_type = std::string(lMappingTypes[pTexture->GetMappingType()]);

    if (pTexture->GetMappingType() == FbxTexture::ePlanar)
    {
        static const char* lPlanarMappingNormals[] = { "X", "Y", "Z" };
        info.planner_mapping_normal = std::string(lPlanarMappingNormals[pTexture->GetPlanarMappingNormal()]);
    }

    static const char* lBlendModes[] = { "Translucent", "Additive", "Modulate", "Modulate2", "Over", "Normal", "Dissolve", "Darken", "ColorBurn", "LinearBurn",
                                         "DarkerColor", "Lighten", "Screen", "ColorDodge", "LinearDodge", "LighterColor", "SoftLight", "HardLight", "VividLight",
                                         "LinearLight", "PinLight", "HardMix", "Difference", "Exclusion", "Substract", "Divide", "Hue", "Saturation", "Color",
                                         "Luminosity", "Overlay"};   
    
    if(pBlendMode >= 0)
    {
        info.blend_mode = std::string(lBlendModes[pBlendMode]);
    }
    
    info.alpha = pTexture->GetDefaultAlpha();

    if (lFileTexture)
    {
        static const char* lMaterialUses[] = { "Model Material", "Default Material" };
        info.material_use = std::string(lMaterialUses[lFileTexture->GetMaterialUse()]);
    }

    static const char* pTextureUses[] = { "Standard", "Shadow Map", "Light Map", "Spherical Reflexion Map", "Sphere Reflexion Map", "Bump Normal Map" };
    info.texture_use = std::string(pTextureUses[pTexture->GetTextureUse()]);

    return info;
}

static void FindAndDisplayTextureInfoByProperty(
    std::vector<TextureParseInfo>& textures, 
    FbxProperty pProperty, 
    int pMaterialIndex)
{
    if(pProperty.IsValid())
    {
		int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();

		for (int j = 0; j < lTextureCount; ++j)
		{
			// Here we have to check if it's layeredtextures, or just textures:
			FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
			if (lLayeredTexture)
			{
                int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
                for(int k = 0; k < lNbTextures; ++k)
                {
                    FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
                    if(lTexture)
                    {
                        // NOTE the blend mode is ALWAYS on the LayeredTexture and NOT the one on the texture.
                        // Why is that?  because one texture can be shared on different layered textures and might
                        // have different blend modes.

                        FbxLayeredTexture::EBlendMode lBlendMode;
                        lLayeredTexture->GetTextureBlendMode(k, lBlendMode);
                        
                        auto textureInfo = getFileTexture(lTexture, (int)lBlendMode);
                        //if(texture != nullptr)
                        {
                            textureInfo.property = std::string(pProperty.GetName());
                            textureInfo._connected_material = pMaterialIndex;
                            textures.push_back(textureInfo);
                        }
                    }

                }
            }
			else
			{
				// no layered texture simply get on the property
                FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
                if(lTexture)
                {
                    // display connected Material header only at the first time
                    auto textureInfo = getFileTexture(lTexture, -1);
                    //if(texture != nullptr)
                    {
                        textureInfo.property = std::string(pProperty.GetName());
                        textureInfo._connected_material = pMaterialIndex;
                        textures.push_back(textureInfo);
                    }
                }
            }
        }
    }
}

std::vector<TextureParseInfo> getTextures(FbxGeometry* pGeometry)
{
    std::vector<TextureParseInfo> textures;

    int lMaterialIndex;
    FbxProperty lProperty;
    if(pGeometry->GetNode()==NULL)
    {
        return textures;
    }
    int lNbMat = pGeometry->GetNode()->GetSrcObjectCount<FbxSurfaceMaterial>();
    for (lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++)
    {
        FbxSurfaceMaterial *lMaterial = pGeometry->GetNode()->GetSrcObject<FbxSurfaceMaterial>(lMaterialIndex);

        //go through all the possible textures
        if(lMaterial)
        {
            int lTextureIndex;
            FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
            {
                lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);
                FindAndDisplayTextureInfoByProperty(textures, lProperty, lMaterialIndex); 
            }
        }
    }
    
    return textures;
}

}