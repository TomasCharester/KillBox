cmake_minimum_required(VERSION 3.16)
project(KillBox LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/TomasCharester/KillBox.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(KillBox src/main.cpp)
target_link_libraries(KillBox PRIVATE sfml-graphics)
target_compile_features(KillBox PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET KillBox
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:KillBox>
        VERBATIM)
endif()

install(TARGETS KillBox)
