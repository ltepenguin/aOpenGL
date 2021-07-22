include(FindPackageHandleStandardArgs)

set(aOpenGL_INCLUDE_DIRS 
    ${CMAKE_CURRENT_LIST_DIR}/code/include
    ${CMAKE_CURRENT_LIST_DIR}/ext/glad/include
    ${CMAKE_CURRENT_LIST_DIR}/ext/glm
    ${CMAKE_CURRENT_LIST_DIR}/ext/stb
    ${CMAKE_CURRENT_LIST_DIR}/ext/eigen
    ${CMAKE_CURRENT_LIST_DIR}/ext/glfw/include
    ${CMAKE_CURRENT_LIST_DIR}/ext/fbxsdk/include)

set(aOpenGL_LIBRARIES 
    ${CMAKE_CURRENT_LIST_DIR}/build/lib/libaOpenGL.so)

add_compile_definitions(AGL_PATH="${CMAKE_CURRENT_LIST_DIR}")