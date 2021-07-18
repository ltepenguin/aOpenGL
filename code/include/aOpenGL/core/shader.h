#pragma once
#pragma GCC system_header
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace a::gl {
namespace core {

class Shader
{
public:
    Shader();
    
    /**
     * @param vsPath vertex shader path
     * @param fsPath fragment shader path
     */
    Shader(std::string vsPath, std::string fsPath);

    /**
     * @param path geometry shader path
     */
    Shader geometry(std::string path);

    /**
     * @brief build shader program. this->program 에 handle 저장
     */
    Shader build();

    /**
     * @brief activate the shader
     */
    void use() const { glUseProgram(m_program); }

    /**
     * @return true일 경우 uniform에서 camera와 light parameter들을 update 해야함.
     */
    bool view_update() { return m_view_updated; }
    void view_update(bool b) { m_view_updated = b; }

    /**
     * @return true일 경우 texture의 uniform id update 필요.
     */
    bool texture_update() { return m_texture_updated; }
    void texture_update(bool b) { m_texture_updated = b; }

    void setBool(std::string name, bool value) const;
	void setInt(std::string name, int value) const;
    void setMultipleInt(std::string name, int num, const int* value) const;
	void setFloat(std::string name, float value) const;
	void setVec2(std::string name, glm::vec2 value) const;
	void setVec2(std::string name, float x, float y) const;
	void setVec3(std::string name, glm::vec3 value) const;
	void setVec3(std::string name, float x, float y, float z) const;
	void setVec4(std::string name, glm::vec4 value) const;
	void setVec4(std::string name, float x, float y, float z, float w) const;
    void setIvec3(std::string name, glm::ivec3 value) const;
    void setIvec4(std::string name, glm::ivec4 value) const;
    void setMat2(std::string name, glm::mat2 mat) const;
	void setMat3(std::string name, glm::mat3 mat) const;
	void setMat4(std::string name, glm::mat4 mat) const;
    void setMultipleMat4(std::string name, int numberOfMatrices, const glm::mat4* matrices) const;
    void setMultipleVec4(std::string name, int numberOfVectors,  const glm::vec4* vectors) const;
    void setMultipleVec3(std::string name, int numberOfVectors,  const glm::vec3* vectors) const;
    void setMultipleIvec3(std::string name, int numberOfVectors,  const glm::ivec3* vectors) const;
    void setMultipleIvec4(std::string name, int numberOfVectors,  const glm::ivec4* vectors) const;

private:
    GLuint m_program, m_vertexShader, m_fragmentShader, m_geometryShader;
    bool m_view_updated, m_texture_updated;
};

}}