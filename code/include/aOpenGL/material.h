#pragma once
#include "texture.h"
#include <glm/glm.hpp>
#include <vector>

namespace a::gl {

struct Material
{
    glm::vec3 albedo{0.7f, 0.7f, 0.7f};
    float alpha{1.0f};
    float metallic{0.0f};
    float roughness{1.0f};

    Texture albedo_map, normal_map, metallic_map, emissive_map;
    Texture roughness_map, ao_map, displacement_map;

    bool is_specular_map{false};   // if true, metallic map is specular map
    bool is_glossiness_map{false}; // if true, roughness map is glossiness map

    const Texture& texture(TextureType& type) const;
    void set_texture(TextureType& type, Texture gl_texture);
};

using vMaterial = std::vector<Material>;

}