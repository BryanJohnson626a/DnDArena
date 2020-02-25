cmake_minimum_required(VERSION 3.15)
project(DnDMonteCarloArena)

include(FetchContent)
FetchContent_Declare(
        nlohmann_json_dl
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.7.3
)
FetchContent_MakeAvailable(nlohmann_json_dl)

add_executable(DnDMonteCarloArena main.cpp Source/Actor.h Source/Action.h Source/Group.h Source/Arena.cpp Source/Arena.h Source/Types.h Source/Group.cpp Source/Dice.cpp Source/Dice.h Source/Actor.cpp Source/Action.cpp Source/Output.cpp Source/Output.h Source/ImportJson.cpp Source/ImportJson.h)

target_compile_features(DnDMonteCarloArena PRIVATE
        cxx_std_20
        cxx_auto_type
        cxx_lambdas
        cxx_nullptr
        cxx_range_for
        cxx_variadic_templates
        )

target_link_libraries(DnDMonteCarloArena PUBLIC nlohmann_json)

add_custom_command(TARGET DnDMonteCarloArena POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/Resources DEPENDS ${CMAKE_BINARY_DIR}/Resources
        COMMENT "Creating Resource folder symlink ${CMAKE_SOURCE_DIR}/Resources -> ${CMAKE_BINARY_DIR}/Resources")
