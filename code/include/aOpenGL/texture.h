#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "image.h"
#include "core/shader.h"

namespace a::gl {

/**
 * @brief TextureType
 */
enum class TextureType 
{ 
    kAlbedo = 0, kNormal, kMetallic, kRoughness, kAO, kDisplacement,
    kDiffuse, kEmissive, kSpecular, kGlossiness,
    kUnknown
};

/**
 * @brief Texture
 * @author ckm
 * @since Mon Aug 24 2020
 */
struct Texture
{
    std::string path{""}; 
    GLuint handle{0};
    int width{0}, height{0}, channel{0};
};
//using spTexture = std::shared_ptr<Texture>;

/**
 * @brief 모든 texture들은 여기서 로드하기
 * @author ckm
 * @since Mon Aug 24 2020
 */
class TextureLoader
{
public:
    /**
     * @param path
     * @param channel channel을 강제. 0일 경우는 자동으로 읽음. default로 RGB로 읽음
     * @return 
     */
    static Texture load(std::string path, int channel = 3)
    {
        return instance()->_load(path, channel);
    }
    
    /**
     * @brief environment map 생성시 사용
     */
    static Texture load_hdr(std::string path)
    {
        return instance()->_load_hdr(path);
    }

    /**
     * @brief Redner::initialize_shaders 에서 사용
     */
    static Texture load_envmap(std::string path, core::Shader* to_cubemap)
    {
        return instance()->_load_envmap(path, to_cubemap);
    }

    static Texture create(std::string name, Image::spData data, bool nearest = false)
    {
        return instance()->_create(name, data->image, data->width, data->height, data->channel, nearest);
    }

    static Texture create(std::string name, unsigned char* img, int width, int height, int channel, bool nearest = false)
    {
        return instance()->_create(name, img, width, height, channel, nearest);
    }

private:    
    /**
     * @brief singleton 
     */
    static TextureLoader* instance() 
    {
        static TextureLoader* loader = new TextureLoader;
        return loader;
    }

    TextureLoader(){};

    Texture _create(std::string path, unsigned char* img, int width, int height, int channel, bool nearest);

    Texture _load(std::string path, int channel);
    Texture _load_hdr(std::string path);
    Texture _load_envmap(std::string path, core::Shader* to_cubemap);

    std::map<std::string, Texture> m_textures;
    std::map<std::string, Texture> m_textures_hdrs;
    std::map<std::string, Texture> m_textures_env;
};

}