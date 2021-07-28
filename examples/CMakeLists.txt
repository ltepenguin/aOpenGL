# example 01: basic rendering
add_executable(draw ${CMAKE_CURRENT_SOURCE_DIR}/01_draw.cpp)
target_link_libraries(draw PUBLIC aOpenGL)

# example 02: basic rendering
add_executable(texture ${CMAKE_CURRENT_SOURCE_DIR}/02_texture.cpp)
target_link_libraries(texture PUBLIC aOpenGL)

# example 03: model loading
add_executable(model ${CMAKE_CURRENT_SOURCE_DIR}/03_model.cpp)
target_link_libraries(model PUBLIC aOpenGL)

# example 04: motion loading
add_executable(motion ${CMAKE_CURRENT_SOURCE_DIR}/04_motion.cpp)
target_link_libraries(motion PUBLIC aOpenGL)

# example 05: sphere
add_executable(sphere ${CMAKE_CURRENT_SOURCE_DIR}/05_sphere.cpp)
target_link_libraries(sphere PUBLIC aOpenGL)

# example 06: interpolate
add_executable(interpolate ${CMAKE_CURRENT_SOURCE_DIR}/06_interpolate.cpp)
target_link_libraries(interpolate PUBLIC aOpenGL)

# example 07: stitching
add_executable(stitching ${CMAKE_CURRENT_SOURCE_DIR}/07_stitching.cpp)
target_link_libraries(stitching PUBLIC aOpenGL)

# example 08: kindata
add_executable(kindata ${CMAKE_CURRENT_SOURCE_DIR}/08_kindata.cpp)
target_link_libraries(kindata PUBLIC aOpenGL)

add_executable(test ${CMAKE_CURRENT_SOURCE_DIR}/08_test.cpp)
target_link_libraries(test PUBLIC aOpenGL)

add_executable(root_projection ${CMAKE_CURRENT_SOURCE_DIR}/09_root_projection.cpp)
target_link_libraries(root_projection PUBLIC aOpenGL)