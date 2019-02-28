cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

PROJECT(QtHardMon)

set(${PROJECT_NAME}_MAJOR_VERSION 01)
set(${PROJECT_NAME}_MINOR_VERSION 02)
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
add_dependency(ChimeraTK-DeviceAccess 01.01 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qwt 6)

FIND_PACKAGE(Boost COMPONENTS REQUIRED system filesystem)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
#use -DCMAKE_BUILD_TYPE=Debug in your cmake command to turn on the coverage option
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 --coverage")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
include_directories(${CMAKE_SOURCE_DIR}/src)
aux_source_directory(${CMAKE_SOURCE_DIR}/src app_sources)
list(REMOVE_ITEM app_sources ${CMAKE_SOURCE_DIR}/src/main.cpp)
add_library(${PROJECT_NAME}_STATIC STATIC ${app_sources})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}_STATIC Qt5::Core  Qt5::Network Qt5::Widgets Qt5::Gui ${ChimeraTK-DeviceAccess_LIBRARIES} ${Boost_LIBRARIES})

#CMAKE from version 3.7 puts auto-generated objects in a target-specific directory and adds the include path only to this target.
#To make the headers generated from the ui files of the library available to the executable and the test,
#we have to include it explicitly.
#Note: from cmake 3.9.6 this is configurable, but we want to support all version.
#CMake before 3.7 puts the generated files to the build folder, which wer also include at a different place.
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_STATIC_autogen/include)

#configure a header file which contains the version information for use in C++
configure_file(cmake/QtHardMonVersion.h.in
  "${PROJECT_BINARY_DIR}/QtHardMonVersion.h" @ONLY)
#configure the resource file which contains the path to the images
configure_file(cmake/QtHardMon.qrc.in
  "${PROJECT_BINARY_DIR}/QtHardMon.qrc" @ONLY)

#The use of the QWT plotting library is optional
if(QWT_FOUND)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_QWT=true")
  INCLUDE_DIRECTORIES( SYSTEM ${QWT_INCLUDE_DIR} )
else(QWT_FOUND)
  SET( QWT_LIBRARY "")
  message("QWT not found, plotting is disabled in this build.")
endif(QWT_FOUND)

ADD_EXECUTABLE(${PROJECT_NAME} src/main.cpp ${PROJECT_BINARY_DIR}/QtHardMon.qrc)
#The static library as well as its auto-generated headers are needed to compile main
if(NOT ${CMAKE_VERSION} VERSION_LESS "3.7")
  ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}_STATIC ${PROJECT_NAME}_STATIC_autogen)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_NAME}_STATIC Qt5::Core Qt5::Widgets Qt5::Gui ${Boost_LIBRARIES}  ${ChimeraTK-DeviceAccess_LIBRARIES} ${QWT_LIBRARY})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "${ChimeraTK-DeviceAccess_LINKER_FLAGS}")

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

enable_testing()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
