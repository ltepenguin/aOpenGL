#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "image.h"
#include "core/shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace a::gl {

class FontTexture
{
public:
    // Holds all state information relevant to a character as loaded using FreeType
    struct CharInfo
    {
        unsigned int TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    explicit FontTexture(const std::string& font_path);

    const CharInfo& character(char c) const
    {
        return m_chars.at(c);
    }

    unsigned int vao() { return m_vao; }
    unsigned int vbo() { return m_vbo; }

private:
    std::map<char, CharInfo> m_chars;
    unsigned int m_vao, m_vbo;
};

}