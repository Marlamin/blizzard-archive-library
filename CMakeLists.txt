# ███████████               █████████   ███████████     █████████  █████   █████ █████ █████   █████ ██████████
#░░███░░░░░███             ███░░░░░███ ░░███░░░░░███   ███░░░░░███░░███   ░░███ ░░███ ░░███   ░░███ ░░███░░░░░█
# ░███    ░███            ░███    ░███  ░███    ░███  ███     ░░░  ░███    ░███  ░███  ░███    ░███  ░███  █ ░ 
# ░██████████  ██████████ ░███████████  ░██████████  ░███          ░███████████  ░███  ░███    ░███  ░██████   
# ░███░░░░░███░░░░░░░░░░  ░███░░░░░███  ░███░░░░░███ ░███          ░███░░░░░███  ░███  ░░███   ███   ░███░░█   
# ░███    ░███            ░███    ░███  ░███    ░███ ░░███     ███ ░███    ░███  ░███   ░░░█████░    ░███ ░   █
# ███████████             █████   █████ █████   █████ ░░█████████  █████   █████ █████    ░░███      ██████████
#░░░░░░░░░░░             ░░░░░   ░░░░░ ░░░░░   ░░░░░   ░░░░░░░░░  ░░░░░   ░░░░░ ░░░░░      ░░░      ░░░░░░░░░░ 
CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE(./cmake/cmake-function.cmake)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

PROJECT("BlizzardArchiveLib")

SET(EXTERNAL_SOURCE_DIR external)
# Add the found include directories to our include list.
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include" )

#testConsole files
collect_files(TestConsole_source "${CMAKE_CURRENT_SOURCE_DIR}/test" false "*.cpp" "")
collect_files(TestConsole_headers "${CMAKE_CURRENT_SOURCE_DIR}/test" false "*.h;*.hpp;*.inl" "")

collect_files(BlizzardArchiveLib_source "${CMAKE_CURRENT_SOURCE_DIR}/src" false "*.cpp" "")
collect_files(BlizzardArchiveLib_headers "${CMAKE_CURRENT_SOURCE_DIR}/include" false "*.h;*.hpp;*.inl" "")

assign_source_group(
  ${BlizzardArchiveLib_source} 
  ${BlizzardArchiveLib_headers} 
  ${TestConsole_source} 
  ${TestConsole_headers} 
)

FIND_PACKAGE(StormLib REQUIRED )
FIND_PACKAGE(CascLib REQUIRED )

OPTION(BLIZZARD_ARCHIVE_TEST_CONSOLE "Build Test Console" OFF)
IF(NOT BLIZZARD_ARCHIVE_TEST_CONSOLE)
  MESSAGE(STATUS "Skipping test console build")
ELSE()
    ADD_EXECUTABLE(TestConsole 
        ${BlizzardArchiveLib_headers} 
        ${BlizzardArchiveLib_source} 

        ${TestConsole_source} 
        ${TestConsole_headers}
    )  
    TARGET_LINK_LIBRARIES(TestConsole StormLib CascLib)
ENDIF(NOT BLIZZARD_ARCHIVE_TEST_CONSOLE)