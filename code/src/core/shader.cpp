#include "aOpenGL/core/shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace a::gl::core {

static std::string loadCode(std::string filename)
{
    // retrieve the source code from file
    std::string   shaderCode;
    std::ifstream shaderFile;

    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // file open
        shaderFile.open(filename.c_str());
        
        // read file's buffer contest into streams
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();

        // close file handlers
        shaderFile.close();

        // convert stream into string
        shaderCode = shaderStream.str();
    }
    catch(const std::exception& e)
    {
        std::cerr << __FILE__ << "(line " << __LINE__ << ")" << e.what() << std::endl;
    }   
    return shaderCode;
}

static void checkShaderCompileError(GLuint handle)
{
    GLint  success;
    GLchar infoLog[1024];
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(handle, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: SHADER \n" 
                  << infoLog 
                  << "\n -- --------------------------------------------------- -- " 
                  << std::endl;
        // glDeleteShader(handle);
    }
}

static void checkProgramCompileError(GLuint handle)
{
    GLint  success;
    GLchar infoLog[1024];
    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(handle, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: PROGRAM \n" 
                  << infoLog 
                  << "\n -- --------------------------------------------------- -- " 
                  << std::endl;
    }
}

static GLuint load(std::string filename, GLenum shaderType)
{
    std::string shaderCodeStr = a::gl::core::loadCode(filename);
    const char* shaderCode = shaderCodeStr.c_str();

    // compile shaders
    int shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    checkShaderCompileError(shader);

    return shader;
}

Shader::Shader() : m_program(GL_INVALID_INDEX), 
                   m_vertexShader(GL_INVALID_INDEX),
                   m_fragmentShader(GL_INVALID_INDEX),
                   m_geometryShader(GL_INVALID_INDEX),
                   m_view_updated(false),
                   m_texture_updated(false)
{}

Shader::Shader(std::string vsPath, std::string fsPath): 
    m_program(GL_INVALID_INDEX), 
    m_vertexShader(GL_INVALID_INDEX),
    m_fragmentShader(GL_INVALID_INDEX),
    m_geometryShader(GL_INVALID_INDEX),
    m_view_updated(false),
    m_texture_updated(false)
{
    m_vertexShader = load(vsPath, GL_VERTEX_SHADER);
    m_fragmentShader = load(fsPath, GL_FRAGMENT_SHADER);
}

Shader Shader::geometry(std::string path)
{
    m_geometryShader = load(path, GL_GEOMETRY_SHADER);
    return *this;
}

Shader Shader::build()
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, m_vertexShader);
    glAttachShader(prog, m_fragmentShader);
    if(m_geometryShader != GL_INVALID_INDEX)
        glAttachShader(prog, m_geometryShader);

    glLinkProgram(prog);
    checkProgramCompileError(prog);

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
    if(m_geometryShader != GL_INVALID_INDEX)
    {
        glDeleteShader(m_geometryShader);
    }
    
    m_program = prog;
    return *this;
}

void Shader::setBool(std::string name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), (int)value);
}

void Shader::setInt(std::string name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setMultipleInt(std::string name, int num, const int* value) const
{
    glUniform1iv(glGetUniformLocation(m_program, name.c_str()), num, &(value[0]));
}

void Shader::setFloat(std::string name, float value) const
{
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setVec2(std::string name, glm::vec2 value) const
{
    glUniform2fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(std::string name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_program, name.c_str()), x, y);
}

void Shader::setVec3(std::string name, glm::vec3 value) const
{
    glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(std::string name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_program, name.c_str()), x, y, z);
}

void Shader::setVec4(std::string name, const glm::vec4 value) const
{
    glUniform4fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(std::string name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(m_program, name.c_str()), x, y, z, w);
}

void Shader::setIvec3(std::string name, glm::ivec3 value) const
{
    glUniform3iv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
}

void Shader::setIvec4(std::string name, glm::ivec4 value) const
{
    glUniform4iv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
}

void Shader::setMat2(std::string name, glm::mat2 mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(std::string name, glm::mat3 mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(std::string name, glm::mat4 mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMultipleMat4(std::string name, int numberOfMatrices ,const glm::mat4* matrices) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), numberOfMatrices, GL_FALSE, &(matrices[0])[0][0]);
}

void Shader::setMultipleVec4(std::string name, int numberOfVectors ,const glm::vec4* vectors) const
{
    glUniform4fv(glGetUniformLocation(m_program, name.c_str()), numberOfVectors, &(vectors[0][0]));
}

void Shader::setMultipleVec3(std::string name, int numberOfVectors ,const glm::vec3* vectors) const
{
    glUniform3fv(glGetUniformLocation(m_program, name.c_str()), numberOfVectors, &(vectors[0][0]));
}

void Shader::setMultipleIvec3(std::string name, int numberOfVectors,  const glm::ivec3* vectors) const
{
    glUniform3iv(glGetUniformLocation(m_program, name.c_str()), numberOfVectors, &(vectors[0][0]));
}

void Shader::setMultipleIvec4(std::string name, int numberOfVectors,  const glm::ivec4* vectors) const
{
    glUniform4iv(glGetUniformLocation(m_program, name.c_str()), numberOfVectors, &(vectors[0][0]));
}

}