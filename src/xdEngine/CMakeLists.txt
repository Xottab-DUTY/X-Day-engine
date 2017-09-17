cmake_minimum_required(VERSION 3.8)

project(xdEngine)

set(CONSOLE Console.hpp
			Console.cpp)

set(CONSOLE_COMMAND ConsoleCommand.hpp
                    ConsoleCommand.cpp
                    ConsoleCommands.hpp
                    ConsoleCommands.cpp)

set(DEBUG_FOLDER Debug/Log.hpp
				 Debug/Log.cpp
				 Debug/QuantMS.hpp
				 Debug/QuantMS.cpp)

set(VKDEMORENDERER Renderer/VkDemoRenderer.hpp
                   Renderer/VkDemoRenderer.cpp
                   Renderer/Shader.hpp
                   Renderer/Shader.cpp)

set(VKDEMORENDERER_SHADERS ${RESOURCES_PATH}/shaders/shader.frag
                           ${RESOURCES_PATH}/shaders/shader.vert)

set(XMLFOLDER XMLDocument.hpp
              XMLDocument.cpp
              XMLResource.hpp
              XMLResource.cpp)

set(XDCORE xdCore.hpp
           xdCore.cpp)

set(XDENGINE xdEngine.hpp
             xdEngine.cpp)

source_group("Console" FILES ${CONSOLE})
source_group("Console\\Command" FILES ${CONSOLE_COMMAND})

source_group("Debug" FILES ${DEBUG_FOLDER})

source_group("VkDemoRender" FILES ${VKDEMORENDERER})
source_group("VkDemoRender\\Shaders" FILES ${VKDEMORENDERER_SHADERS})

source_group("XML" FILES ${XMLFOLDER})

source_group("Main" FILES main.cpp ${XDCORE} ${XDENGINE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${INCLUDES_PATH}/spdlog/include)
include_directories(${INCLUDES_PATH}/fmt)
include_directories(${INCLUDES_PATH}/glm)
include_directories(${INCLUDES_PATH}/gli)
include_directories(${INCLUDES_PATH}/glslang/glslang/Public)
include_directories(${INCLUDES_PATH}/glslang/glslang/Include)
include_directories(${INCLUDES_PATH}/tinyobjloader)

add_definitions(-DXDAY_EXPORTS -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS)

add_executable(xdEngine main.cpp
               ${CONSOLE}
               ${CONSOLE_COMMAND}
               ${DEBUG_FOLDER}
			   ${VKDEMORENDERER}
               ${VKDEMORENDERER_SHADERS}
               ${XMLFOLDER}
               ${XDCORE}
               ${XDENGINE})

target_link_libraries(xdEngine ${Vulkan_LIBRARY} ${GLFW_LIBRARY})
