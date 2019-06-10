cmake_minimum_required(VERSION 3.13)
project(proof)

set(CMAKE_CXX_STANDARD 11)

# Add source files
file(GLOB SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/source/CGTutorial/source/*.c
        ${CMAKE_SOURCE_DIR}/source/CGTutorial/source/*.cpp)

# Add header files
file(GLOB HEADER_FILES
        ${CMAKE_SOURCE_DIR}/source/CGTutorial/source/*.h
        ${CMAKE_SOURCE_DIR}/source/CGTutorial/source/*.hpp)

# Add shader files
file(GLOB SHADER_FILES
        ${CMAKE_SOURCE_DIR}/source/CGTutorial/resources/*.fragmentshader
        ${CMAKE_SOURCE_DIR}/source/CGTutorial/resources/*.vertexshader
        ${CMAKE_SOURCE_DIR}/source/CGTutorial/source/res/shaders/*.frag
        ${CMAKE_SOURCE_DIR}/source/CGTutorial/source/res/shaders/*.vs)

# Add .lib files
link_directories(
        ${CMAKE_SOURCE_DIR}/source/common/external/libs)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} source/CGTutorial/source/StreetObject.h)

# Find external dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Find the required packages

# OpenGL
find_package(OpenGL REQUIRED)

# GLUT
find_package(GLUT REQUIRED)

# GLM
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

# GLFW
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

# ASSIMP
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

# SOIL
#find_package(SOIL REQUIRED)
#message(STATUS "Found SOIL in ${SOIL_INCLUDE_DIR}")
set(SOIL SOIL)

# GLEW
find_package(GLEW REQUIRED)
message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")

# X11 on UNIX systems
find_package(X11 REQUIRED)

# Put all libraries into a variable
set(LIBS ${GLFW3_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLEW_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread ${ASSIMP_LIBRARY} ${SOIL})

target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIRS})

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS} ${GLUT_LIBRARY})

# Define the include directories
include_directories(
        ${CMAKE_SOURCE_DIR}/source/common/external/include)
