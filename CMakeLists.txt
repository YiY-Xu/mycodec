cmake_minimum_required(VERSION 3.14)
project(mycodec C CXX)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/glfw)
add_subdirectory(lib/ffmpeg)

if(APPLE)
    list(APPEND EXTRA_LIBS
        "-framework OpenGL"
        )
elseif(WIN32)
    list(APPEND EXTRA_LIBS
        "-lglu32 -lopengl32"
        )
    set(CMAKE_EXE_LINKER_FLAGS "-std=gun99 -static -static-libgcc -static-libstdc++ -mvwindows")
else()
    list(APPEND EXTRA_LIBS
        "-lGl -lGLU -lX11"
        )
endif()

list(APPEND SOURCES
    src/main.cpp
    src/load_frame.cpp
)

add_executable(mycodec ${SOURCES})
target_link_libraries(mycodec ffmpeg glfw ${EXTRA_LIBS})