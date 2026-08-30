cmake_minimum_required(VERSION 3.14)
project(forrnsown LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUAJIT REQUIRED luajit)
add_compile_definitions(SOL_LUA_VERSION=501)

# get all .cpp files in src
file(GLOB SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
add_library(forrnsown SHARED ${SRC_FILES})
#add_executable(forrnsown ${SRC_FILES})

target_include_directories(forrnsown PRIVATE
	include
	external/clap/include
	external/clap-helpers/include
	external/sol2/include
	${LUAJIT_INCLUDE_DIRS}
)

target_link_directories(forrnsown PRIVATE
    ${LUAJIT_LIBRARY_DIRS} # Ensures linker finds libluajit
)

target_link_libraries(forrnsown PRIVATE
	Threads::Threads
	${LUAJIT_LIBRARIES}
)

target_link_options(forrnsown PRIVATE ${LUAJIT_LDFLAGS})

set_target_properties(forrnsown PROPERTIES
	PREFIX ""
	SUFFIX ".clap"
)

set(CLAP_INSTALL_PATH "$ENV{HOME}/.clap")
add_custom_command(TARGET forrnsown POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CLAP_INSTALL_PATH}"
	COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:forrnsown>" "${CLAP_INSTALL_PATH}/forrnsown.clap"
	COMMENT "Auto-installing to ${CLAP_INSTALL_PATH}"
)
