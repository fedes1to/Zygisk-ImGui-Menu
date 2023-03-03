cmake_minimum_required(VERSION 3.18.1)

if (NOT DEFINED MODULE_NAME)
    message(FATAL_ERROR "MODULE_NAME is not set")
else ()
    project(${MODULE_NAME})
endif ()

message("Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 20)

set(LINKER_FLAGS "-ffixed-x18 -Wl,--hash-style=both")
set(C_FLAGS "-Werror=format -fdata-sections -ffunction-sections -lz")
set(CXX_FLAGS "${CXX_FLAGS} -fno-exceptions -fno-rtti")

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(C_FLAGS "${C_FLAGS} -O2 -fvisibility=hidden -fvisibility-inlines-hidden")
    set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,-exclude-libs,ALL -Wl,--gc-sections -Wl,--strip-all")
else ()
    set(C_FLAGS "${C_FLAGS} -O0")
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS} ${CXX_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKER_FLAGS}")

include_directories(KittyMemory)
include_directories(Include)
include_directories(Includes)
include_directories(ImGui)
include_directories(ImGui/backends)

add_library(${MODULE_NAME}
        SHARED
        ImGui/imgui.cpp
        ImGui/imgui_draw.cpp
        ImGui/imgui_demo.cpp
        ImGui/imgui_widgets.cpp
        ImGui/imgui_tables.cpp
        ImGui/backends/imgui_impl_opengl3.cpp
        ImGui/backends/imgui_impl_android.cpp
        KittyMemory/KittyArm64.cpp
        KittyMemory/KittyMemory.cpp
        KittyMemory/KittyScanner.cpp
        KittyMemory/KittyUtils.cpp
        KittyMemory/MemoryBackup.cpp
        KittyMemory/MemoryPatch.cpp
        main.cpp
        hook.cpp
        )

set(DOBBY_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libdobby.a)
set(CURL_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libcurl.a)
find_library(log-lib log)
find_library(egl EGL)
find_library(android android)
find_library(GLESV2_LIB GLESv2)
find_library(LZ_LIB z)
if (${ANDROID_PLATFORM_LEVEL} LESS 12)
    message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 /
                      (currently using ${ANDROID_PLATFORM_LEVEL}).")
    return()
elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
    add_definitions("-DDYNAMIC_ES3")
    set(GL3STUB_SRC gl3stub.c)
    set(OPENGL_LIB GLESv2)
else ()
    set(OPENGL_LIB GLESv3)
endif (${ANDROID_PLATFORM_LEVEL} LESS 12)

target_link_libraries(
        ${MODULE_NAME} log
        ${DOBBY_LIB}
        ${CURL_LIB}
        ${OPENGL_LIB}
        ${GLESV2_LIB}
        ${android}
        ${egl}  )
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
            COMMAND ${CMAKE_STRIP} --strip-all --remove-section=.comment "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${MODULE_NAME}.so")
endif ()
