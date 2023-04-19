## Setting up Boost Unit Test Framework.
find_package(Boost 1.46 COMPONENTS system unit_test_framework REQUIRED)

## If there are any common tests' functionalities, they are compiled into separate library and later linked against each executable.
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src testSources)
add_library( ${PROJECT_NAME}_TEST_LIBRARY ${testSources} )
if(NOT ${CMAKE_VERSION} VERSION_LESS "3.7")
  ADD_DEPENDENCIES(${PROJECT_NAME}_TEST_LIBRARY ${PROJECT_NAME}_STATIC_autogen)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

## Add executable targets for each test in `executables_src` directory, additionally marking it as a test for CTest.
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/executables_src testExecutables)
foreach( testExecutableSrcFile ${testExecutables})
  #NAME_WE means the base name without path and (longest) extension
  get_filename_component(executableName ${testExecutableSrcFile} NAME_WE)
  add_executable(${executableName} ${testExecutableSrcFile})
  if(NOT ${CMAKE_VERSION} VERSION_LESS "3.7")
    ADD_DEPENDENCIES(${executableName} ${PROJECT_NAME}_STATIC ${PROJECT_NAME}_STATIC_autogen)
  endif()
  target_link_libraries(${executableName} ${Boost_LIBRARIES} ${PROJECT_NAME}_STATIC Qt5::Core Qt5::Widgets Qt5::Gui
                                          ${mtca4u-deviceaccess_LIBRARIES} ${PROJECT_NAME}_TEST_LIBRARY)
  set_target_properties(${executableName} PROPERTIES LINK_FLAGS "${ChimeraTK-DeviceAccess_LINKER_FLAGS}")
  add_test(${executableName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executableName})
endforeach(testExecutableSrcFile)

## Copy files from test_files into the tests binary directory.
foreach( DIRECTORY  test_files )
    file( COPY ${DIRECTORY} DESTINATION ${PROJECT_BINARY_DIR}/tests )
  endforeach( DIRECTORY )
