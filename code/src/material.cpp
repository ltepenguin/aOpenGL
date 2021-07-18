#include "aOpenGL/material.h"

namespace a::gl {

const Texture& Material::texture(TextureType& type) const
{
    switch(type) {
    case(TextureType::kNormal):
        return normal_map;
    case(TextureType::kSpecular):
    case(TextureType::kMetallic):
        return metallic_map;
    case(TextureType::kGlossiness):
    case(TextureType::kRoughness):
        return roughness_map;
    case(TextureType::kAO):
        return ao_map;
    case(TextureType::kDisplacement):
        return displacement_map;
    case(TextureType::kEmissive):
        return emissive_map;
    case(TextureType::kAlbedo):
    case(TextureType::kDiffuse):
    default:
        return albedo_map;
    };
}

void Material::set_texture(TextureType& type, Texture gl_texture)
{
    switch(type)
    {
    case(TextureType::kAlbedo):
    case(TextureType::kDiffuse):
        albedo_map = gl_texture;
        break;
    case(TextureType::kNormal):
        normal_map = gl_texture;
        break;
    case(TextureType::kSpecular):
        metallic_map = gl_texture;
        is_specular_map = true;
        break;                
    case(TextureType::kMetallic):
        metallic_map = gl_texture;
        is_specular_map = false;
        break;
    case(TextureType::kGlossiness):
        roughness_map = gl_texture;
        is_glossiness_map = true;
        break;
    case(TextureType::kRoughness):
        roughness_map = gl_texture;
        is_glossiness_map = false;
        break;
    case(TextureType::kAO):
        ao_map = gl_texture;
        break;
    case(TextureType::kDisplacement):
        displacement_map = gl_texture;
        break;
    case(TextureType::kEmissive):
        emissive_map = gl_texture;
        break;
    default:
        break;
    }
}

}