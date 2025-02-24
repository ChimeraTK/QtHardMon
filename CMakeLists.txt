cmake_minimum_required(VERSION 3.16)

PROJECT(QtHardMon)

set(${PROJECT_NAME}_MAJOR_VERSION 01)
set(${PROJECT_NAME}_MINOR_VERSION 08)
set(${PROJECT_NAME}_PATCH_VERSION 00)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc, rcc and uic automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# # Load custom modules, i.e. FindDOOCS.cmake
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

find_package(ChimeraTK-DeviceAccess 03.10 REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Widgets Gui Charts)
find_package(Qt${QT_VERSION_MAJOR} 5.10 REQUIRED COMPONENTS Core Network Widgets Gui Charts)

find_package(Boost REQUIRED COMPONENTS system filesystem)

include(${CMAKE_SOURCE_DIR}/cmake/set_default_build_to_release.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/set_default_flags.cmake)

include_directories(${CMAKE_SOURCE_DIR}/src)
aux_source_directory(${CMAKE_SOURCE_DIR}/src app_sources)
file(GLOB app_headers ${CMAKE_SOURCE_DIR}/src/*.h)
list(REMOVE_ITEM app_sources ${CMAKE_SOURCE_DIR}/src/main.cpp)
add_library(${PROJECT_NAME}_STATIC STATIC ${app_sources} ${app_headers})


target_link_libraries(${PROJECT_NAME}_STATIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Charts ChimeraTK::ChimeraTK-DeviceAccess Boost::system Boost::filesystem)

# CMAKE from version 3.7 puts auto-generated objects in a target-specific directory and adds the include path only to this target.
# To make the headers generated from the ui files of the library available to the executable and the test,
# we have to include it explicitly.
# Note: from cmake 3.9.6 this is configurable, but we want to support all version.
# CMake before 3.7 puts the generated files to the build folder, which wer also include at a different place.
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_STATIC_autogen/include)

# configure a header file which contains the version information for use in C++
configure_file(cmake/QtHardMonVersion.h.in
  "${PROJECT_BINARY_DIR}/QtHardMonVersion.h" @ONLY)

# configure the resource file which contains the path to the images
configure_file(cmake/QtHardMon.qrc.in
  "${PROJECT_BINARY_DIR}/QtHardMon.qrc" @ONLY)

ADD_EXECUTABLE(${PROJECT_NAME} src/main.cpp ${PROJECT_BINARY_DIR}/QtHardMon.qrc)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_NAME}_STATIC Qt${QT_VERSION_MAJOR}Core Qt${QT_VERSION_MAJOR}Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Charts Boost::system Boost::filesystem ChimeraTK::ChimeraTK-DeviceAccess)

# The use of the QWT plotting library is optional
include(cmake/enable_code_coverage_report.cmake)
include(cmake/enable_doxygen_documentation.cmake)

# FIXME: the user guide should be build from source, but we need dependencies defined,
# fix the date to the writing date (not compilation date) and put the version
# guide has been written for (it most probably is outdated)
INSTALL(FILES doc/user_guide/QtHardMon_user_guide.pdf DESTINATION share/doc/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
)

enable_testing()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
