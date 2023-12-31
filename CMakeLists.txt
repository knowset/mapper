cmake_minimum_required(VERSION 3.9)

project(Mapper)


set(CMAKE_CXX_STANDARD 17)

add_definitions(-DNAPI_VERSION=6)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(V8_DEPRECATION_WARNINGS 1)

include_directories(${CMAKE_JS_INC})

file(GLOB SOURCE_FILES "./MapperSource/*.cpp")

message(WARNING ${SOURCE_FILES})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})

execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )


string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})
