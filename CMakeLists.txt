PROJECT(QtHardMon)
cmake_minimum_required(VERSION 2.8)

set(${PROJECT_NAME}_MAJOR_VERSION 00)
set(${PROJECT_NAME}_MINOR_VERSION 18)
set(${PROJECT_NAME}_PATCH_VERSION 02)
set(${PROJECT_NAME}_VERSION
  ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})
set(${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION})


# Set the build type to Release if none is specified
# Force it into Release if "None" is specified (needed to overrule dkpg_buildpackage)
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "None")

# Quite verbose compiler warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -fPIC -Wextra -Wshadow -Weffc++ -pedantic -Wuninitialized")

#needed for findQwt
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake")

FIND_PACKAGE(Qt4 REQUIRED)
set(mtca4u-deviceaccess_MIN_VERSION 00.22)
FIND_PACKAGE(mtca4u-deviceaccess ${mtca4u-deviceaccess_MIN_VERSION} REQUIRED)
FIND_PACKAGE(Qwt 6)


#boost as such is required for shared pointers e.g.

FIND_PACKAGE(Boost COMPONENTS REQUIRED system filesystem)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

#turn on the rpath so libmtca4u-deviceaccess.so is found
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

INCLUDE_DIRECTORIES( SYSTEM ${mtca4u-deviceaccess_INCLUDE_DIRS} )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${mtca4u-deviceaccess_CXX_FLAGS}")

SET(${PROJECT_NAME}_SOURCES src/main.cpp src/${PROJECT_NAME}.cpp src/ConfigFileReaderWriter.cpp 
                            src/PlotWindow.cpp src/CustomDelegates.cpp src/HexSpinBox.cpp
                            src/CustomQTreeItem.cpp)
# The headers which contain Q_OBJECTs (need to be processed with moc)
SET(${PROJECT_NAME}_QOBJECT_HEADERS src/${PROJECT_NAME}.h src/PlotWindow.h src/CustomDelegates.h src/HexSpinBox.h)
# All headers, to have the dependencies correct
SET(${PROJECT_NAME}_HEADERS ${PROJECT_NAME}_QOBJECT_HEADERS src/PlotWindow.h)
SET(${PROJECT_NAME}_FORMS src/${PROJECT_NAME}Form.ui src/PreferencesForm.ui src/PlotWindowForm.ui)
SET(${PROJECT_NAME}_RESOURCES src/${PROJECT_NAME}.qrc)

QT4_WRAP_CPP(${PROJECT_NAME}_HEADERS_MOC ${${PROJECT_NAME}_QOBJECT_HEADERS})
QT4_WRAP_UI(${PROJECT_NAME}_FORMS_HEADERS ${${PROJECT_NAME}_FORMS})
QT4_ADD_RESOURCES(${PROJECT_NAME}_RESOURCES_RCC ${${PROJECT_NAME}_RESOURCES})

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(SYSTEM ${QT_INCLUDES})

#configure a header file which contains the version information for use in C++
configure_file(cmake/QtHardMonVersion.h.in
  "${PROJECT_BINARY_DIR}/QtHardMonVersion.h" @ONLY)

#The use of the QWT plotting library is optional
if(QWT_FOUND)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_QWT=true")
  INCLUDE_DIRECTORIES( SYSTEM ${QWT_INCLUDE_DIR} )  
else(QWT_FOUND)
  SET( QWT_LIBRARY "")
  message("QWT not found, plotting is disabled in this build.")
endif(QWT_FOUND)

ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} 
  ${${PROJECT_NAME}_HEADERS_MOC}
  ${${PROJECT_NAME}_FORMS_HEADERS} 
  ${${PROJECT_NAME}_RESOURCES_RCC})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${QT_LIBRARIES} 
     ${mtca4u-deviceaccess_LIBRARIES} ${QWT_LIBRARY} ${Boost_LIBRARIES})

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

# asking compiler to treat the generate ui_ files as system files (these have
# warnings in them and we cannot do much because these headers are automatically
# generated?)
INCLUDE_DIRECTORIES(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  )

# The instructions to create a debian package are loaded from a separate file
include(${CMAKE_SOURCE_DIR}/cmake/prepare_debian_package.cmake)
