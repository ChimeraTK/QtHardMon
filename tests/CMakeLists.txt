## Setting up Boost Unit Test Framework.
add_dependency(Boost 1.46 system unit_test_framework REQUIRED)

## If there are any common tests' functionalities, they are compiled into separate library and later linked against each executable.
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src testSources)
add_library( ${PROJECT_NAME}_TEST_LIBRARY ${testSources} )

include_directories(${CMAKE_SOURCE_DIR}/src)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

## Add executable targets for each test in `executables_src` directory, additionally marking it as a test for CTest.
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/executables_src testExecutables)
foreach( testExecutableSrcFile ${testExecutables})
  #NAME_WE means the base name without path and (longest) extension
  get_filename_component(executableName ${testExecutableSrcFile} NAME_WE)
  add_executable(${executableName} ${testExecutableSrcFile} $<TARGET_OBJECTS:${PROJECT_NAME}_OBJECTS>)
  # target_link_libraries(${executableName} ${PROJECT_NAME} ${PROJECT_NAME}_TEST_LIBRARY ${Boost_LIBRARIES}) 
  target_link_libraries(${executableName} ${Boost_LIBRARIES} ${QT_LIBRARIES} 
     ${mtca4u-deviceaccess_LIBRARIES} ${QWT_LIBRARY} ${PROJECT_NAME}_TEST_LIBRARY)
  add_test(${executableName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executableName})
endforeach(testExecutableSrcFile)

## Copy files from test_files into the tests binary directory.
foreach( DIRECTORY  test_files )
    file( COPY ${DIRECTORY} DESTINATION ${PROJECT_BINARY_DIR}/tests )
  endforeach( DIRECTORY )