cmake_minimum_required (VERSION 3.20)

set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

project(app)

set(EXECUTABLE_NAME ${PROJECT_NAME})

add_executable(${EXECUTABLE_NAME})

target_include_directories(${EXECUTABLE_NAME} PRIVATE include /usr/include/freetype2)

# set(SDLTTF_VENDORED ON) # tell SDL_ttf to build its own dependencies
# add_subdirectory(external/SDL_ttf)

file(GLOB_RECURSE srcs 
    "src/*.cpp"
)

target_sources(${EXECUTABLE_NAME}
    PRIVATE
    ${srcs}
)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC 
    freetype
	# SDL3_ttf::SDL3_ttf
)

target_compile_definitions(${EXECUTABLE_NAME} PUBLIC SDL_MAIN_USE_CALLBACKS)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS})

find_package(SDL3 REQUIRED)
include_directories( ${SDL3_INCLUDE_DIRS})

target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${OPENGL_LIBRARIES} ${SDL3_LIBRARIES})

file(COPY resources/shaders DESTINATION ${CMAKE_BINARY_DIR})
