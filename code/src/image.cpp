#include "aOpenGL/image.h"
#include "aOpenGL/file.h"
#include <iostream>

#pragma warning(push, 0)
#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
    #define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb_image_write.h>
#pragma warning(pop)

namespace a::gl {

Image::Image()
{}

Image::spData Image::_load(std::string path, int in_channel)
{
    if(m_images.find(path) == m_images.end())
    {
        if(!file_check(path))
            std::cout << "file not found: " << path << std::endl;
        else
            std::cout << "image load: " << path << std::endl;
        
        stbi_set_flip_vertically_on_load(true);
        int width, height, channel;
        auto img = stbi_load(path.c_str(), &width, &height, &channel, in_channel);
        if(img == nullptr)
        {
            std::cout << "- stbi_load failed: " << stbi_failure_reason() << std::endl;
        }
        
        auto data = std::make_shared<Image::Data>();
        data->path = path;
        data->width = width;
        data->height = height;
        if(in_channel == 0)
            data->channel = channel;
        else
            data->channel = in_channel;
        data->image = img;
        m_images.insert(std::pair<std::string, Image::spData>(path, data));        
    }
    return m_images.at(path);
}

Image::spData Image::_create(std::string path, unsigned char* img, int width, int height, int channel)
{
    auto iter = m_images.find(path);
    if(iter != m_images.end())
    {
        Image::_free_image(path);
        m_images.erase(path);
    }

    auto data = std::make_shared<Image::Data>();
    data->path = path;
    data->width = width;
    data->height = height;
    data->channel = channel;
    data->image = img;
    m_images.insert(std::pair<std::string, Image::spData>(path, data));
    
    return data;
}

Image::spDataHDR Image::_load_hdr(std::string path)
{
    auto iter = m_hdrs.find(path);
    if(iter == m_hdrs.end())
    {
        if(!file_check(path))
            std::cout << "file not found: " << path << std::endl;
        
        int width, height, channel;
        stbi_set_flip_vertically_on_load(true);
        auto img = stbi_loadf(path.c_str(), &width, &height, &channel, 0);
        if(img == nullptr)
            std::cout << "- stbi_loadf failed: " << stbi_failure_reason() << std::endl;

        auto data = std::make_shared<Image::DataHDR>();
        data->path = path;
        data->width = width;
        data->height = height;
        data->channel = channel;
        data->image = img;
        m_hdrs.insert(std::pair<std::string, Image::spDataHDR>(path, data));
    }
    return m_hdrs.at(path);
}

void Image::_free_image(std::string path)
{
    auto iter = m_images.find(path);
    if(iter == m_images.end())
        return;
    stbi_image_free(iter->second->image);
    //free(iter->second->image);
    m_images.erase(iter);
}

void Image::save_image(std::string save_name, spData data, bool flip)
{
    if(flip)
        stbi_flip_vertically_on_write(true);
    stbi_write_png(save_name.c_str(), data->width, data->height, data->channel, data->image, 0);
}

void Image::save_image(std::string save_name, const char* data, int width, int height, int channel, bool flip)
{
    if(flip)
        stbi_flip_vertically_on_write(true);
    stbi_write_png(save_name.c_str(), width, height, channel, data, 0);
}

}