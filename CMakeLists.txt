cmake_minimum_required(VERSION 3.10)  # Tương thích mọi máy
project(ThePhysicalNightmare LANGUAGES C CXX)
# ✅ GRAPHICS API - HỖ TRỢ MÁY CŨ
# Uncomment 1 trong 3 options dưới đây:
# Option 1: OpenGL 3.3 (mặc định - cho máy mới, hiệu suất tốt nhất)
# add_compile_definitions(GRAPHICS_API_OPENGL_33)
# Option 2: OpenGL 2.1 (cho máy cũ hơn, vẫn hardware accelerated)
add_compile_definitions(GRAPHICS_API_OPENGL_21)
# Option 3: OpenGL 1.1 (cho máy CỰC CŨ, chạy được mọi nơi nhưng chậm)
# add_compile_definitions(GRAPHICS_API_OPENGL_11)

add_compile_definitions(PLATFORM_DESKTOP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Fix cho MinGW
if(MINGW OR WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    
    # ✅ TƯƠNG THÍCH WINDOWS 10
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    add_compile_definitions(WINVER=0x0A00)
    
    # ✅ TẮT WARNING RAYLIB (không ảnh hưởng game)
    add_compile_options(-O3 -Wall -Wextra -Wno-missing-field-initializers)
endif()

# ============================================
# RAYLIB
# ============================================
set(RAYLIB_PATH "E:/AppData/raylib/raylib/src")

find_library(RAYLIB_LIB 
    NAMES raylib libraylib
    PATHS ${RAYLIB_PATH}
    NO_DEFAULT_PATH
    REQUIRED
)

find_path(RAYLIB_INCLUDE 
    NAMES raylib.h
    PATHS ${RAYLIB_PATH}
    NO_DEFAULT_PATH
    REQUIRED
)

message(STATUS "Raylib library: ${RAYLIB_LIB}")
message(STATUS "Raylib include: ${RAYLIB_INCLUDE}")

# ============================================
# SOURCE FILES
# ============================================
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/**/*.cpp"
)

file(GLOB_RECURSE HEADERS 
    "src/*.hpp"
    "src/**/*.hpp"
    "src/*.h"
    "src/**/*.h"
)

# Loại bỏ file không mong muốn (nếu có)
list(FILTER SOURCES EXCLUDE REGEX ".*/test/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*/old/.*")

# ============================================
# EXECUTABLE
# ============================================
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# ============================================
# LINKING
# ============================================
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${RAYLIB_LIB}
    opengl32
    gdi32
    winmm
    kernel32
    user32
    shell32
)

# ============================================
# INCLUDE DIRECTORIES
# ============================================
target_include_directories(${PROJECT_NAME} PRIVATE
    ${RAYLIB_INCLUDE}
    ${RAYLIB_INCLUDE}/external
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/systems
)

# ✅ TẮT WARNING CHO RAYLIB HEADERS
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${RAYLIB_INCLUDE}
    ${RAYLIB_INCLUDE}/external
)

# ============================================
# COPY ASSETS
# ============================================
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Copying assets..."
)

# ============================================
# PRINT INFO
# ============================================
message(STATUS "===========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Raylib: ${RAYLIB_LIB}")
message(STATUS "===========================================")
message(STATUS "Build thanh cong! THE PHYSICAL NIGHTMARE san sang chay!!!")