cmake_minimum_required(VERSION 3.10)
project(raytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Ofast -Wall -Wextra -fopenmp -ffast-math")# -funroll-loops")
#set(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra -fopenmp -ffast-math")
#set(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra -ffast-math")

include_directories(include/sdl/include/SDL2)
include_directories(include/tinytiff)
include_directories(Source)

add_executable(raytracer
        include/tinytiff/tinytiffreader.cpp
        include/tinytiff/tinytiffreader.h
        Source/boundingbox.cpp
        Source/boundingbox.h
        Source/camera.h
        Source/enums.h
        Source/geometry.h
        Source/intersection.h
        Source/light.h
        Source/material.h
        Source/math_utils.h
        Source/math_vector_utils.h
        Source/main.cpp
        Source/octree.cpp
        Source/octree.h
        Source/object.h
        Source/pathtrace.h
        Source/ray.h
        Source/raycast.cpp
        Source/raycast.h
        Source/raytrace.cpp
        Source/raytrace.h
        Source/sampler.h
        Source/scatterevent.h
        Source/scene.h
        Source/SDLauxiliary.h
        Source/settings.h
        Source/texture.h
        Source/transferfunction.h
        Source/triangle.cpp
        Source/triangle.h
        Source/utils.cpp
        Source/utils.h
        Source/volume.cpp
        Source/volume.h
        Source/vector_type.h Source/enums.h)

find_library(SDL_LIB libsdl2 HINTS include/sdl/lib)
target_link_libraries(raytracer PUBLIC ${SDL_LIB})