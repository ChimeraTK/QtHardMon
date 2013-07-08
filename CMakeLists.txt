PROJECT(QtHardMon)
cmake_minimum_required(VERSION 2.8)

set(${PROJECT_NAME}_MAJOR_VERSION 00)
set(${PROJECT_NAME}_MINOR_VERSION 04)
set(${PROJECT_NAME}_PATCH_VERSION 04)
set(${PROJECT_NAME}_VERSION
  ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})
set(${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION})

SET(CMAKE_BUILD_TYPE "Debug")
#only turn this on for testing. Qt causes WAY too many warning.
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -fPIC -Wextra -Wshadow -Weffc++ -ansi -pedantic -Wuninitialized")

#Moderate version of the compiler flags which give no qt warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -Wextra -ansi -Wuninitialized")

#needed for findQwt
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake")

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(MtcaMappedDevice 00.02.01 REQUIRED)
FIND_PACKAGE(Qwt)

#turn on the rpath so libMtcaMappedDevice.so is found
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

INCLUDE_DIRECTORIES( ${MtcaMappedDevice_INCLUDE_DIRS} )

SET(${PROJECT_NAME}_SOURCES main.cpp ${PROJECT_NAME}.cpp ConfigFileReaderWriter.cpp PlotWindow.cpp)
# The headers which contain Q_OBJECTs (need to be processed with moc)
SET(${PROJECT_NAME}_QOBJECT_HEADERS ${PROJECT_NAME}.h PlotWindow.h)
# All headers, to have the dependencies correct
SET(${PROJECT_NAME}_HEADERS ${PROJECT_NAME}_QOBJECT_HEADERS PlotWindow.h)
SET(${PROJECT_NAME}_FORMS ${PROJECT_NAME}Form.ui PreferencesForm.ui PlotWindowForm.ui)
SET(${PROJECT_NAME}_RESOURCES ${PROJECT_NAME}.qrc)

QT4_WRAP_CPP(${PROJECT_NAME}_HEADERS_MOC ${${PROJECT_NAME}_QOBJECT_HEADERS})
QT4_WRAP_UI(${PROJECT_NAME}_FORMS_HEADERS ${${PROJECT_NAME}_FORMS})
QT4_ADD_RESOURCES(${PROJECT_NAME}_RESOURCES_RCC ${${PROJECT_NAME}_RESOURCES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

#configure a header file which contains the version information for use in C++
configure_file(cmake/QtHardMonVersion.h.in
  "${PROJECT_BINARY_DIR}/QtHardMonVersion.h" @ONLY)

#The use of the QWT plotting library is optional
if(QWT_FOUND)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_QWT=true")
  INCLUDE_DIRECTORIES( ${QWT_INCLUDE_DIR} )  
else(QWT_FOUND)
  SET( QWT_LIBRARY "")
  message("QWT not found, plotting is disabled in this build.")
endif(QWT_FOUND)

ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} 
  ${${PROJECT_NAME}_HEADERS_MOC}
  ${${PROJECT_NAME}_FORMS_HEADERS} 
  ${${PROJECT_NAME}_RESOURCES_RCC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${QT_LIBRARIES} ${MtcaMappedDevice_LIBRARIES} ${QWT_LIBRARY})

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc DESTINATION .)
else(DOXYGEN_FOUND)
  message("Doxygen not found, documentation will not be build.")
endif(DOXYGEN_FOUND)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  )

# The instructions to create a debian package are loaded from a separate file
include(${CMAKE_SOURCE_DIR}/cmake/prepare_debian_package.cmake)
