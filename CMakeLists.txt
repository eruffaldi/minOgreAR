project(minOgreAR)
find_package(cmakego REQUIRED COMPONENTS ogre eigen)
add_definitions(--std=c++11)
add_executable(minimal minimal.cpp)
target_link_libraries(minimal p::ogre p::eigen)
file(COPY plugins.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY rviz_cube.mesh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})