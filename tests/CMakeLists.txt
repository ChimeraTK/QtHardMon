## Setting up Boost Unit Test Framework.
find_package(Boost REQUIRED COMPONENTS system unit_test_framework)

## If there are any common tests' functionalities, they are compiled into separate library and later linked against each executable.
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src testSources)
add_library( ${PROJECT_NAME}_TEST_LIBRARY ${testSources} )

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

## Add executable targets for each test in `executables_src` directory, additionally marking it as a test for CTest.
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/executables_src testExecutables)
foreach( testExecutableSrcFile ${testExecutables})
  #NAME_WE means the base name without path and (longest) extension
  get_filename_component(executableName ${testExecutableSrcFile} NAME_WE)
  add_executable(${executableName} ${testExecutableSrcFile})
  target_link_libraries(${executableName} Boost::system Boost::unit_test_framework ${PROJECT_NAME}_STATIC Qt5::Core Qt5::Widgets Qt5::Gui ChimeraTK::ChimeraTK-DeviceAccess ${PROJECT_NAME}_TEST_LIBRARY)
  add_test(${executableName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executableName})
endforeach(testExecutableSrcFile)

## Copy files from test_files into the tests binary directory.
foreach( DIRECTORY  test_files )
    file( COPY ${DIRECTORY} DESTINATION ${PROJECT_BINARY_DIR}/tests )
endforeach( DIRECTORY )
