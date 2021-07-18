# aOpenGL source files
add_library(aOpenGL SHARED 
    # src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/appmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/eigentype.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ik.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/joint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/light.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/render.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderoption.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp

    # core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/primitive.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/shader.cpp

    # primitives
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/primitive/Icosphere.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/primitive/Cubesphere.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/primitive/Cylinder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/primitive/Sphere.cpp
    
    # fbx parser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbx/fbx_animation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbx/fbx_material.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbx/fbx_mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbx/fbx_skeleton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbx/fbx_skin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbx/fbx_texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbx/fbxparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fbx/keyframe.cpp

    # glad
    ${EXT_DIR}/glad/src/glad.c
    )

# include directories
target_include_directories(aOpenGL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(aOpenGL PUBLIC ${EXT_DIR}/glad/include)
target_include_directories(aOpenGL PUBLIC ${EXT_DIR}/glm)
target_include_directories(aOpenGL PUBLIC ${EXT_DIR}/stb)

# eigen
target_include_directories(aOpenGL PUBLIC ${EXT_DIR}/eigen)

# glfw
find_library(GLFW NAMES libglfw3.a
            HINTS ${EXT_DIR}/glfw/build/src
            REQUIRED)
target_include_directories(aOpenGL PUBLIC ${EXT_DIR}/glfw/include)

# fbxsdk
find_library(FBXSDK NAMES libfbxsdk.so 
             HINTS ${EXT_DIR}/fbxsdk/lib/gcc/x64/release
             REQUIRED)
find_library(XML2 NAMES xml2 REQUIRED) # xml2 is required in fbxsdk
target_include_directories(aOpenGL PUBLIC ${EXT_DIR}/fbxsdk/include)

# link
target_link_libraries(aOpenGL PUBLIC ${GLFW})
target_link_libraries(aOpenGL PUBLIC ${FBXSDK} ${XML2})
target_link_libraries(aOpenGL PUBLIC ${CMAKE_DL_LIBS})