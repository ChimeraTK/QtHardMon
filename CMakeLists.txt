PROJECT(QtHardMon)
cmake_minimum_required(VERSION 2.8)

set(${PROJECT_NAME}_MAJOR_VERSION 00)
set(${PROJECT_NAME}_MINOR_VERSION 03)
set(${PROJECT_NAME}_PATCH_VERSION 01)
set(${PROJECT_NAME}_VERSION
  ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})
set(${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION})

SET(CMAKE_BUILD_TYPE "Debug")
#only turn this on for testing. Qt causes WAY too many warning.
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -fPIC -Wextra -Wshadow -Weffc++ -ansi -pedantic -Wuninitialized")

#needed for findQwt
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake")

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(MtcaMappedDevice 00.02.01 REQUIRED)
FIND_PACKAGE(Qwt)

#turn on the rpath so libMtcaMappedDevice.so is found
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

INCLUDE_DIRECTORIES( ${MtcaMappedDevice_INCLUDE_DIRS} )

#let's see what we have
message("status: MtcaMappedDevice_DIR ${MtcaMappedDevice_DIR}")
message("status: MtcaMappedDevice_INCLUDE_DIRS ${MtcaMappedDevice_INCLUDE_DIRS}")
message("status: MtcaMappedDevice_LIBRARIES ${MtcaMappedDevice_LIBRARIES}")
message("status: MtcaMappedDevice_VERSION ${MtcaMappedDevice_VERSION}")

SET(${PROJECT_NAME}_SOURCES main.cpp ${PROJECT_NAME}.cpp ConfigFileReaderWriter.cpp)
SET(${PROJECT_NAME}_HEADERS ${PROJECT_NAME}.h)
SET(${PROJECT_NAME}_FORMS ${PROJECT_NAME}Form.ui PreferencesForm.ui)
SET(${PROJECT_NAME}_RESOURCES ${PROJECT_NAME}.qrc)

QT4_WRAP_CPP(${PROJECT_NAME}_HEADERS_MOC ${${PROJECT_NAME}_HEADERS})
QT4_WRAP_UI(${PROJECT_NAME}_FORMS_HEADERS ${${PROJECT_NAME}_FORMS})
QT4_ADD_RESOURCES(${PROJECT_NAME}_RESOURCES_RCC ${${PROJECT_NAME}_RESOURCES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

configure_file(cmake/QtHardMonVersion.h.in
  "${PROJECT_BINARY_DIR}/QtHardMonVersion.h" @ONLY)

ADD_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} 
  ${${PROJECT_NAME}_HEADERS_MOC}
  ${${PROJECT_NAME}_FORMS_HEADERS} 
  ${${PROJECT_NAME}_RESOURCES_RCC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${QT_LIBRARIES} ${MtcaMappedDevice_LIBRARIES})

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
endif(DOXYGEN_FOUND)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  )

