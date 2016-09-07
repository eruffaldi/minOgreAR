
find_package(cmakego COMPONENTS ogre eigen)

add_executable(minimal minimal.cpp)
target_link_libraries(minimal p::ogre p::eigen)
