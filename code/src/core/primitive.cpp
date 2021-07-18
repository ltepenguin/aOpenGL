#include "aOpenGL/core/primitive.h"
#include "aOpenGL/core/mesh.h"

#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "primitive/Icosphere.h"
#include "primitive/Cubesphere.h"
#include "primitive/Sphere.h"
#include "primitive/Cylinder.h"

namespace a::gl {
namespace core {

// unit cube      
// A cube has 6 sides and each side has 4 vertices, therefore, the total number
// of vertices is 24 (6 sides * 4 verts), and 72 floats in the vertex array
// since each vertex has 3 components (x,y,z) (= 24 * 3)
//    v6----- v5  
//   /|      /|   
//  v1------v0|   
//  | |     | |   
//  | v7----|-v4  
//  |/      |/    
//  v2------v3    
static std::vector<glm::vec3> cube_positions =
{
    glm::vec3( .5f, .5f, .5f), glm::vec3(-.5f, .5f, .5f), glm::vec3(-.5f,-.5f, .5f), glm::vec3( .5f,-.5f, .5f), // v0,v1,v2,v3 (front)
    glm::vec3( .5f, .5f, .5f), glm::vec3( .5f,-.5f, .5f), glm::vec3( .5f,-.5f,-.5f), glm::vec3( .5f, .5f,-.5f), // v0,v3,v4,v5 (right)
    glm::vec3( .5f, .5f, .5f), glm::vec3( .5f, .5f,-.5f), glm::vec3(-.5f, .5f,-.5f), glm::vec3(-.5f, .5f, .5f), // v0,v5,v6,v1 (top)
    glm::vec3(-.5f, .5f, .5f), glm::vec3(-.5f, .5f,-.5f), glm::vec3(-.5f,-.5f,-.5f), glm::vec3(-.5f,-.5f, .5f), // v1,v6,v7,v2 (left)
    glm::vec3(-.5f,-.5f,-.5f), glm::vec3( .5f,-.5f,-.5f), glm::vec3( .5f,-.5f, .5f), glm::vec3(-.5f,-.5f, .5f), // v7,v4,v3,v2 (bottom)
    glm::vec3( .5f,-.5f,-.5f), glm::vec3(-.5f,-.5f,-.5f), glm::vec3(-.5f, .5f,-.5f), glm::vec3( .5f, .5f,-.5f)  // v4,v7,v6,v5 (back)
};

static std::vector<glm::vec3> cube_normals = 
{
    glm::vec3( 0, 0, 1),  glm::vec3( 0, 0, 1),  glm::vec3( 0, 0, 1),  glm::vec3( 0, 0, 1),  // v0,v1,v2,v3 (front)
    glm::vec3( 1, 0, 0),  glm::vec3( 1, 0, 0),  glm::vec3( 1, 0, 0),  glm::vec3( 1, 0, 0),  // v0,v3,v4,v5 (right)
    glm::vec3( 0, 1, 0),  glm::vec3( 0, 1, 0),  glm::vec3( 0, 1, 0),  glm::vec3( 0, 1, 0),  // v0,v5,v6,v1 (top)
    glm::vec3(-1, 0, 0),  glm::vec3(-1, 0, 0),  glm::vec3(-1, 0, 0),  glm::vec3(-1, 0, 0),  // v1,v6,v7,v2 (left)
    glm::vec3( 0,-1, 0),  glm::vec3( 0,-1, 0),  glm::vec3( 0,-1, 0),  glm::vec3( 0,-1, 0),  // v7,v4,v3,v2 (bottom)
    glm::vec3( 0, 0,-1),  glm::vec3( 0, 0,-1),  glm::vec3( 0, 0,-1),  glm::vec3( 0, 0,-1)   // v4,v7,v6,v5 (back)
};

static std::vector<glm::vec2> cube_uvs = 
{ 
    glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), // v0,v1,v2,v3 (front)
    glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), // v0,v3,v4,v5 (right)
    glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1), // v0,v5,v6,v1 (top)
    glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), // v1,v6,v7,v2 (left)
    glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0), // v7,v4,v3,v2 (bottom)
    glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0)  // v4,v7,v6,v5 (back) 
};

static std::vector<unsigned int> cube_indices = 
{
     0, 1, 2,   2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
     4, 5, 6,   6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
     8, 9,10,  10,11, 8,    // v0-v5-v6, v6-v1-v0 (top)
    12,13,14,  14,15,12,    // v1-v6-v7, v7-v2-v1 (left)
    16,17,18,  18,19,16,    // v7-v4-v3, v3-v2-v7 (bottom)
    20,21,22,  22,23,20     // v4-v7-v6, v6-v5-v4 (back)
};

VAO VAOPrimitive::build_cube()
{
    std::vector<glm::vec3> tangents, bitangents;    
    a::gl::core::compute_tangent_space(tangents, bitangents, cube_positions, cube_normals, cube_uvs, cube_indices);
    auto vertices = a::gl::core::to_vertex_array(cube_positions, cube_normals, cube_uvs, tangents, bitangents, cube_indices);
    VAO p = a::gl::core::bind_mesh(vertices, cube_indices);
    return p;
}

// plane
//    v2----- v1  
//   /       /   
//  v3------v0

static std::vector<glm::vec3> plane_positions =
{
    glm::vec3( .5f, .0f, .5f), glm::vec3( .5f, .0f,-.5f), glm::vec3(-.5f, .0f,-.5f), glm::vec3(-.5f, .0f, .5f)
};
static std::vector<glm::vec3> plane_normals = 
{
    glm::vec3( 0, 1, 0),  glm::vec3( 0, 1, 0),  glm::vec3( 0, 1, 0),  glm::vec3( 0, 1, 0)
};
static std::vector<glm::vec2> plane_uvs = 
{ 
    glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1)
};
static std::vector<unsigned int> plane_indices = 
{
     0, 1, 2,   2, 3, 0
};

VAO VAOPrimitive::build_plane()
{
    std::vector<glm::vec3> tangents, bitangents;
    a::gl::core::compute_tangent_space(tangents, bitangents, plane_positions, plane_normals, plane_uvs, plane_indices);
    auto vertices = a::gl::core::to_vertex_array(plane_positions, plane_normals, plane_uvs, tangents, bitangents, plane_indices);
    VAO p = a::gl::core::bind_mesh(vertices, plane_indices);
    return p;
}

// sphere

VAO VAOPrimitive::build_sphere()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> tangents, bitangents;

    {
        //Cubesphere sphere(0.5f, 3, true);
        //Icosphere sphere(0.5f, 3, true);
        Sphere sphere(0.5f);
        int Vnum = sphere.getVertexCount();
        const float* V = sphere.getVertices();
        const float* N = sphere.getNormals();
        const float* T = sphere.getTexCoords();

        int Nnum = sphere.getNormalCount();
        assert(Vnum == Nnum);
        int Tnum = sphere.getTexCoordCount();
        assert(Vnum == Tnum);

        for(int i = 0; i < Vnum; ++i)
        {
            int vi = i * 3;
            int ti = i * 2;
#if 0
            positions.push_back(glm::vec3(V[vi + 0],V[vi + 1],V[vi + 2]));
            normals.push_back(glm::vec3(N[vi + 0], N[vi + 1], N[vi + 2]));
#else
            glm::vec3 p(V[vi + 0],V[vi + 1],V[vi + 2]);
            glm::vec3 normal = glm::normalize(p);
            positions.push_back(p);
            normals.push_back(normal);
            
            glm::vec3 up(0, 1, 0);
            glm::vec3 tangent;
            if(glm::distance(up, normal) < 0.001f)
            {
                tangent = glm::vec3(0, 0, 1);
            }
            else
            {
                tangent = glm::cross(normal, up);
                tangent = glm::normalize(tangent);
            }
            
            glm::vec3 bitangent = glm::cross(normal, tangent);
            bitangent = glm::normalize(bitangent);
            
            tangents.push_back(tangent);
            bitangents.push_back(bitangent);
#endif
            uvs.push_back(glm::vec2(T[ti + 0], T[ti + 1]));
        }

        const unsigned int* I = sphere.getIndices();
        int Inum = sphere.getIndexCount();
        for(int i = 0; i < Inum; ++i)
        {
            indices.push_back(I[i]);
        }
    }

    auto vertices = a::gl::core::to_vertex_array(positions, normals, uvs, tangents, bitangents, indices);
    VAO p = a::gl::core::bind_mesh(vertices, indices);
    return p;    
}

VAO VAOPrimitive::build_cylinder()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> tangents, bitangents;

    {
        Cylinder primitive_obj(0.5f, 0.5f, 1.0f, 36, 1, true);
        int Vnum = primitive_obj.getVertexCount();
        const float* V = primitive_obj.getVertices();
        const float* N = primitive_obj.getNormals();
        const float* T = primitive_obj.getTexCoords();

        int Nnum = primitive_obj.getNormalCount();
        assert(Vnum == Nnum);
        int Tnum = primitive_obj.getTexCoordCount();
        assert(Vnum == Tnum);

        //glm::mat4 rmat = glm::rotate(M_PI * 0.5f, glm::vec3(0, 0, 1));
        //glm::angleAxis(M_PI * 0.5f, glm::vec3(0, 0, 1));
        glm::dvec3 z_axis(1, 0, 0);
        glm::quat rq = glm::angleAxis(-M_PI * 0.5f, z_axis);

        for(int i = 0; i < Vnum; ++i)
        {
            int vi = i * 3;
            int ti = i * 2;
            glm::vec3 p(V[vi + 0],V[vi + 1],V[vi + 2]);
            p = rq * p;

            //glm::vec3 normal = glm::normalize(p);
            glm::vec3 normal(N[vi+0], N[vi+1], N[vi+2]);
            normal = rq * normal;

            positions.push_back(p);
            normals.push_back(normal);
            uvs.push_back(glm::vec2(T[ti + 0], T[ti + 1]));
        }

        const unsigned int* I = primitive_obj.getIndices();
        int Inum = primitive_obj.getIndexCount();
        for(int i = 0; i < Inum; ++i)
        {
            indices.push_back(I[i]);
        }
    }

    a::gl::core::compute_tangent_space(tangents, bitangents, 
        positions, normals, uvs, indices);

    auto vertices = a::gl::core::to_vertex_array(positions, normals, uvs, tangents, bitangents, indices);
    VAO p = a::gl::core::bind_mesh(vertices, indices);
    return p;
}

VAO VAOPrimitive::build_cone()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> tangents, bitangents;

    {
        Cylinder primitive_obj(0.5f, 0.0f, 1.0f, 36, 1, true);
        int Vnum = primitive_obj.getVertexCount();
        const float* V = primitive_obj.getVertices();
        const float* N = primitive_obj.getNormals();
        const float* T = primitive_obj.getTexCoords();

        int Nnum = primitive_obj.getNormalCount();
        assert(Vnum == Nnum);
        int Tnum = primitive_obj.getTexCoordCount();
        assert(Vnum == Tnum);

        //glm::mat4 rmat = glm::rotate(M_PI * 0.5f, glm::vec3(0, 0, 1));
        //glm::angleAxis(M_PI * 0.5f, glm::vec3(0, 0, 1));
        glm::dvec3 z_axis(1, 0, 0);
        glm::quat rq = glm::angleAxis(-M_PI * 0.5f, z_axis);

        for(int i = 0; i < Vnum; ++i)
        {
            int vi = i * 3;
            int ti = i * 2;
            glm::vec3 p(V[vi + 0],V[vi + 1],V[vi + 2]);
            p = rq * p + glm::vec3(0, 0.5f, 0);

            //glm::vec3 normal = glm::normalize(p);
            glm::vec3 normal(N[vi+0], N[vi+1], N[vi+2]);
            normal = rq * normal;

            positions.push_back(p);
            normals.push_back(normal);
            uvs.push_back(glm::vec2(T[ti + 0], T[ti + 1]));
        }

        const unsigned int* I = primitive_obj.getIndices();
        int Inum = primitive_obj.getIndexCount();
        for(int i = 0; i < Inum; ++i)
        {
            indices.push_back(I[i]);
        }
    }

    a::gl::core::compute_tangent_space(tangents, bitangents, 
        positions, normals, uvs, indices);

    auto vertices = a::gl::core::to_vertex_array(positions, normals, uvs, tangents, bitangents, indices);
    VAO p = a::gl::core::bind_mesh(vertices, indices);
    return p;
}

VAO VAOPrimitive::build_pyramid()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> tangents, bitangents;

    {
        Cylinder primitive_obj(0.5f, 0.0f, 1.0f, 4, 1, true);
        int Vnum = primitive_obj.getVertexCount();
        const float* V = primitive_obj.getVertices();
        const float* N = primitive_obj.getNormals();
        const float* T = primitive_obj.getTexCoords();

        int Nnum = primitive_obj.getNormalCount();
        assert(Vnum == Nnum);
        int Tnum = primitive_obj.getTexCoordCount();
        assert(Vnum == Tnum);

        //glm::mat4 rmat = glm::rotate(M_PI * 0.5f, glm::vec3(0, 0, 1));
        //glm::angleAxis(M_PI * 0.5f, glm::vec3(0, 0, 1));
        glm::dvec3 z_axis(1, 0, 0);
        glm::quat rq = glm::angleAxis(-M_PI * 0.5f, z_axis);

        for(int i = 0; i < Vnum; ++i)
        {
            int vi = i * 3;
            int ti = i * 2;
            glm::vec3 p(V[vi + 0],V[vi + 1],V[vi + 2]);
            p = rq * p + glm::vec3(0, 0.5f, 0);

            //glm::vec3 normal = glm::normalize(p);
            glm::vec3 normal(N[vi+0], N[vi+1], N[vi+2]);
            normal = glm::normalize(rq * normal);

            positions.push_back(p);
            normals.push_back(normal);
            uvs.push_back(glm::vec2(T[ti + 0], T[ti + 1]));
        }

        const unsigned int* I = primitive_obj.getIndices();
        int Inum = primitive_obj.getIndexCount();
        for(int i = 0; i < Inum; ++i)
        {
            indices.push_back(I[i]);
        }
    }

    a::gl::core::compute_tangent_space(tangents, bitangents, 
        positions, normals, uvs, indices);

    auto vertices = a::gl::core::to_vertex_array(positions, normals, uvs, tangents, bitangents, indices);
    VAO p = a::gl::core::bind_mesh(vertices, indices);
    return p;
}

}}