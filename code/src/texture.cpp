#include "aOpenGL/texture.h"
#include "aOpenGL/core/primitive.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

namespace a::gl {

static GLuint generate_texture(unsigned char* img, int width, int height, int channel, bool nearest = false)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the image data
    GLenum format = GL_RGB;
    if(channel == 1) format = GL_RED;
    else if(channel == 3) format = GL_RGB;
    else if(channel == 4) format = GL_RGBA;
    else
    {
        std::cerr << "unknown format. channel: " << channel << std::endl;
        assert(false);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);

    // testing. 비등방성 필터링
    if(1)
    {
        GLfloat value, max_anisotropy = 8.0f; /* don't exceed this value...*/
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, & value);
        value = (value > max_anisotropy) ? max_anisotropy : value;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
    }
    
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    if(nearest)
    {
        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    return texture;
}

Texture TextureLoader::_create(std::string path, unsigned char* img, int width, int height, int channel, bool nearest)
{
    auto iter = m_textures.find(path);
    if(iter != m_textures.end())
    {
        //newTexture = iter->second;
        glDeleteTextures(1, &(iter->second.handle));
        m_textures.erase(path);
    }

    // read image data
    //auto data = a::gl::Image::create(path, img, width, height, channel);
    
    Texture newTexture;
    newTexture.path = path;
    newTexture.handle  = generate_texture(img, width, height, channel, nearest);
    newTexture.width   = width;
    newTexture.height  = height;
    newTexture.channel = channel;
    m_textures[path]   = newTexture;
    
    return m_textures.at(path);
}

Texture TextureLoader::_load(std::string path, int channel)
{
    auto iter = m_textures.find(path);
    if(iter == m_textures.end())
    {
        // read image data
        auto data = a::gl::Image::load(path, channel);
        
        Texture newTexture;
        newTexture.path = path;
        newTexture.handle = generate_texture(data->image, data->width, data->height, data->channel);
        newTexture.width = data->width;
        newTexture.height = data->height;
        newTexture.channel = data->channel;
        m_textures[path] = newTexture;
    }
    return m_textures.at(path);
}

static GLuint generate_hdr_texture(float* img, int width, int height, int channel)
{
    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    // note how we specify the texture's data value to be float
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, img);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return hdrTexture;
}

Texture TextureLoader::_load_hdr(std::string path)
{
    auto iter = m_textures_hdrs.find(path);
    if(iter == m_textures_hdrs.end())
    {
        // read image data
        auto data = a::gl::Image::load_hdr(path);

        Texture newTexture;
        newTexture.path = path;
        newTexture.handle = generate_hdr_texture(data->image, data->width, data->height, data->channel);
        newTexture.width = data->width;
        newTexture.height = data->height;
        newTexture.channel = data->channel;
        m_textures_hdrs[path] = newTexture;
    }
    return m_textures_hdrs.at(path);
}

static void render_cube()
{
    auto vao = core::VAOPrimitive::cube();
    glBindVertexArray(vao.vao);
    glDrawElements(GL_TRIANGLES, vao.idx_num, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

static GLuint generate_env_texture(Texture& hdr, core::Shader* equirectangularToCubemapShader)
{
    // pbr: setup framebuffer
    // ----------------------
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    equirectangularToCubemapShader->use();
    equirectangularToCubemapShader->setInt("u_equirectangularMap", 0);
    equirectangularToCubemapShader->setMat4("u_projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr.handle);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader->setMat4("u_view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_cube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return envCubemap;
}

Texture TextureLoader::_load_envmap(std::string path, core::Shader* to_cubemap)
{
    auto iter = m_textures_env.find(path);
    if(iter == m_textures_env.end())
    {
        Texture hdr = this->_load_hdr(path);
        Texture newTexture;
        newTexture.handle = generate_env_texture(hdr, to_cubemap);
        m_textures_env[path] = newTexture;
        std::cout << "envmap generated\n";
    }
    return m_textures_env.at(path);
}

}