#pragma once
#pragma GCC system_header
#include <glad/glad.h>
#include <memory>
#include <mutex>
#include "mesh.h"

namespace a::gl {
namespace core {

/**
 * @brief Primitive Type 도형들의 VAO 정의
 * @author 
 * @since Mon Aug 10 2020
 */
class VAOPrimitive
{
public:
    static VAO cube()
    {
        static VAO p = build_cube();
        return p;
    }

    static VAO plane()
    {
        static VAO p = build_plane();
        return p;
    }

    static VAO sphere()
    {
        static VAO p = build_sphere();
        return p;
    }

    static VAO cylinder()
    {
        static VAO p = build_cylinder();
        return p;
    }

    static VAO cone()
    {
        static VAO p = build_cone();
        return p;
    }
    
    static VAO pyramid()
    {
        static VAO p = build_pyramid();
        return p;
    }

private:
    static VAO build_cube();
    static VAO build_plane();
    static VAO build_sphere();
    static VAO build_cylinder();
    static VAO build_cone();
    static VAO build_pyramid();
};

}
}