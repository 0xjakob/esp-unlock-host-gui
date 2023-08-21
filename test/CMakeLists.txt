cmake_minimum_required(VERSION 3.10)

# set the project name
project(espkey VERSION 0.1)

set(CMAKE_CXX_STANDARD_REQUIRED True)
# add the executable
add_executable(test test.cpp ../totp_command_exception.cpp ../totp_add_command.cpp ../totp_command.cpp)
set_property(TARGET test PROPERTY CXX_STANDARD 20)

target_include_directories(test PUBLIC
                           "${CMAKE_SOURCE_DIR}/.."
                            "/usr/include/catch2"
                           )
set(EXTRA_LINKER_ARGS "-llibbaseencode" "-L /usr/lib/x86_64-linux-gnu/")

target_link_libraries(test baseencode)
