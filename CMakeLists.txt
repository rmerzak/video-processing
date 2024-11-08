cmake_minimum_required(VERSION 3.14)
project(video C CXX)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/glfw)
add_subdirectory(lib/FFmpeg)
add_definitions(-DGL_SILENCE_DEPRECATION)
if(APPLE)
    list(APPEND Extra_LIBS 
        "-framework OpenGL"
    )
elseif(WIN32)
    list(APPEND Extra_LIBS 
        "-lglu32 -lopengl32"
    )
    set(CMAKE_EXE_LINKER_FLAGS "-std=gnu99 -static -static-libgcc -static-libstdc++ -mwindows")
else()
    list(APPEND Extra_LIBS 
        "-lGL -lGLU -lX11 -lXxf86vm -lXrandr -lpthread -lXi"
    )
endif()
list (APPEND SOURCES
    src/main.cpp
    src/load_frame.cpp
)
add_executable(video ${SOURCES})
target_link_libraries(video FFmpeg glfw ${Extra_LIBS})