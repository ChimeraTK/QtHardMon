#
# this module look for qwt (http://hdf.ncsa.uiuc.edu) support
# it will define the following values
#
# QWT_INCLUDE_DIR  = where qwt.h can be found
# QWT_LIBRARY      = the library to link against qwt
# QWT_FOUND        = set to true after finding the library
#

INCLUDE(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/FindQt.cmake)

IF(EXISTS ${PROJECT_CMAKE}/QwtConfig.cmake)
  INCLUDE(${PROJECT_CMAKE}/QwtConfig.cmake)
ENDIF(EXISTS ${PROJECT_CMAKE}/QwtConfig.cmake)

IF(Qwt_INCLUDE_DIRS)

  FIND_PATH(QWT_INCLUDE_DIR qwt.h ${Qwt_INCLUDE_DIRS})
  FIND_LIBRARY(QWT_LIBRARY qwt ${Qwt_LIBRARY_DIRS})

ELSE(Qwt_INCLUDE_DIRS)

  FIND_PATH(QWT_INCLUDE_DIR qwt.h 
    /usr/include/qwt
    /usr/include/qwt6
    /usr/local/include/qwt
    /usr/local/include/qwt6
    /sw/include/qwt
    /sw/include/qwt6
    )
  FIND_LIBRARY(QWT_LIBRARY qwt-qt5
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /sw/lib
    /sw/lib64
    )

ENDIF(Qwt_INCLUDE_DIRS)

#message("QWT_INCLUDE_DIR is \"${QWT_INCLUDE_DIR}\", QWT_LIBRARY is \"${QWT_LIBRARY}\"")

IF(QWT_INCLUDE_DIR AND QWT_LIBRARY) 
  SET(QWT_FOUND 1 CACHE BOOL "Found qwt library")
ELSE(QWT_INCLUDE_DIR AND QWT_LIBRARY) 
  SET(QWT_FOUND 0 CACHE BOOL "Qwt library not found")
ENDIF(QWT_INCLUDE_DIR AND QWT_LIBRARY) 

MARK_AS_ADVANCED(
  QWT_INCLUDE_DIR
  QWT_LIBRARY
  QWT_FOUND
  )

