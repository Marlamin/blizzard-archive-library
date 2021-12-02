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

PROJECT("BlizzardArchiveLib")

SET(EXTERNAL_SOURCE_DIR external)
# Add the found include directories to our include list.
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include" )
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/${EXTERNAL_SOURCE_DIR}/casclib/src")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/${EXTERNAL_SOURCE_DIR}/stormlib/src")
#add stormLib deps
ADD_SUBDIRECTORY("${EXTERNAL_SOURCE_DIR}/stormlib")
collect_files(stormlib_headers "${EXTERNAL_SOURCE_DIR}/stormlib/src" FALSE "*.h;*.hpp" "")
#add cascLib deps
ADD_SUBDIRECTORY("${EXTERNAL_SOURCE_DIR}/casclib")
collect_files(casclib_headers "${EXTERNAL_SOURCE_DIR}/casclib/src" FALSE "*.h;*.hpp" "")

#library files
collect_files(BlizzardArchiveLib_source "${CMAKE_CURRENT_SOURCE_DIR}/src" true "*.cpp" "")
collect_files(BlizzardArchiveLib_headers "${CMAKE_CURRENT_SOURCE_DIR}/include" true "*.h;*.hpp;*.inl" "")

#testConsole files
collect_files(TestConsole_source "${CMAKE_CURRENT_SOURCE_DIR}/test" false "*.cpp" "")
collect_files(TestConsole_headers "${CMAKE_CURRENT_SOURCE_DIR}/test" false "*.h;*.hpp;*.inl" "")

ADD_LIBRARY(BlizzardArchiveLib  
    ${BlizzardArchiveLib_source} 
    ${BlizzardArchiveLib_headers} 
    ${stormlib_headers}
    ${casclib_headers}
)
TARGET_LINK_LIBRARIES(BlizzardArchiveLib)

OPTION(BLIZZARD_ARCHIVE_TEST_CONSOLE "Build Test Console" OFF)
IF(NOT BLIZZARD_ARCHIVE_TEST_CONSOLE)
  MESSAGE(STATUS "Skipping test console build")
ELSE()
    ADD_EXECUTABLE(TestConsole 
        ${TestConsole_source} 
        ${TestConsole_headers}
    )  
    TARGET_LINK_LIBRARIES(TestConsole BlizzardArchiveLib)
ENDIF(NOT BLIZZARD_ARCHIVE_TEST_CONSOLE)