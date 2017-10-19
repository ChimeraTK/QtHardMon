cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

PROJECT(QtHardMon)

set(${PROJECT_NAME}_MAJOR_VERSION 00)
set(${PROJECT_NAME}_MINOR_VERSION 19)
set(${PROJECT_NAME}_PATCH_VERSION 00)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc, rcc and uic automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

## Set additional version-specific variables
include(${CMAKE_SOURCE_DIR}/cmake/set_version_numbers.cmake)
## Load custom modules, i.e. FindDOOCS.cmake
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

## Set the build type to Release if none is specified
## Force it into Release if "None" is specified (needed to overrule dkpg_buildpackage)
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None")

set(CMAKE_CXX_STANDARD 14)

include(cmake/add_dependency.cmake)
add_dependency(mtca4u-deviceaccess 00.22 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

FIND_PACKAGE(Boost COMPONENTS REQUIRED system filesystem)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
#use -DCMAKE_BUILD_TYPE=Debug in your cmake command to turn on the coverage option
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 --coverage")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
include_directories(${CMAKE_SOURCE_DIR}/src)
aux_source_directory(${CMAKE_SOURCE_DIR}/src app_sources)
list(REMOVE_ITEM app_sources ${CMAKE_SOURCE_DIR}/src/main.cpp)
add_library(${PROJECT_NAME}_STATIC STATIC ${app_sources})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}_STATIC Qt5::Core Qt5::Widgets Qt5::Gui ${mtca4u-deviceaccess_LIBRARIES} ${Boost_LIBRARIES})

#configure a header file which contains the version information for use in C++
configure_file(cmake/QtHardMonVersion.h.in
  "${PROJECT_BINARY_DIR}/QtHardMonVersion.h" @ONLY)

SET( QWT_LIBRARY "")

# ADD_EXECUTABLE(${PROJECT_NAME} src/main.cpp )
# TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${${PROJECT_NAME}_STATIC} Qt5::Core Qt5::Widgets Qt5::Gui  ${mtca4u-deviceaccess_LIBRARIES} ${Boost_LIBRARIES} )
ADD_EXECUTABLE(${PROJECT_NAME} ${app_sources} src/main.cpp )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui ${Boost_LIBRARIES} ${mtca4u-deviceaccess_LIBRARIES})

  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # enable code coverate report
  include(cmake/enable_code_coverage_report.cmake)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
  # do not install the developer information
  #install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/ DESTINATION share/doc/${PROJECT_NAME})
else(DOXYGEN_FOUND)
  message("Doxygen not found, documentation will not be build.")
endif(DOXYGEN_FOUND)

#FIXME: the user guide should be build from source, but we need dependencies defined, 
# fix the date to the writing date (not compilation date) and put the version
# guide has been written for (it most probably is outdated)
INSTALL(FILES doc/user_guide/QtHardMon_user_guide.pdf DESTINATION share/doc/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
