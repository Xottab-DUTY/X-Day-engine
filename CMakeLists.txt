cmake_minimum_required(VERSION 3.8)

project(X-Day-engine)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED YES)

if (MSVC)
    if(MSVC_VERSION EQUAL 1911) # MSVS 2017.3
        execute_process(COMMAND "vswhere" "-version" "15.3" "-property" "installationPath"
                        WORKING_DIRECTORY ${ENGINE_ROOT}/tools
                        OUTPUT_VARIABLE VS_TOOLS_PATH
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(VC_ENV_BAT ${VS_TOOLS_PATH}/VC/Auxiliary/Build/vcvarsall.bat)
        add_compile_options(/std:c++latest /await)
    else()
        message(FATAL_ERROR "Only MSVC 15.3 (MSVS 2017) and higher is supported.")
    endif()
else()
    message(FATAL_ERROR "Only MSVC compiler is supported right now.")
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ARCH_X86 TRUE)
else()
    set(ARCH_X86 FALSE)
endif()

if (ARCH_X86)
    set(ARCH_SUFFIX x86)
else()
    set(ARCH_SUFFIX x64)
endif()

set(ENGINE_ROOT ${CMAKE_HOME_DIRECTORY})
set(SOURCES_PATH ${ENGINE_ROOT}/src)
set(INCLUDES_PATH ${ENGINE_ROOT}/include)
set(RESOURCES_PATH ${ENGINE_ROOT}/resources)

set(BIN_OUT_PATH ${ENGINE_ROOT}/bin/${ARCH_SUFFIX})
set(PDB_OUT_PATH ${ENGINE_ROOT}/bin/pdb/${ARCH_SUFFIX})
set(LIB_OUT_PATH ${ENGINE_ROOT}/bin/pdb/${ARCH_SUFFIX})
set(INTERMEDIATE_OUT_PATH ${ENGINE_ROOT}/intermediate/${ARCH_SUFFIX})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_OUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_OUT_PATH})
set(CMAKE_PDB_OUTPUT_DIRECTORY ${PDB_OUT_PATH})

find_package(Vulkan REQUIRED)
find_package(GLFW PATHS ${INCLUDES_PATH}/glfw )

if (NOT GLFW_FOUND)
	message("glfw not found")
else()
	message("glfw found")
endif()

include_directories($ENV{VULKAN_SDK}/include)
include_directories(${INCLUDES_PATH})
include_directories(${INCLUDES_PATH}/glfw/include)
include_directories(${SOURCES_PATH})

add_subdirectory(src)
add_subdirectory(include)
