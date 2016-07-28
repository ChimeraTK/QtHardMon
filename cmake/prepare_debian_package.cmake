#Prepare the debian control files from the template.
#Basically this is setting the correct version number in most of the files

#Variables needed by the unified make_denian_package.sh
#We must not have hyphens in the environment variable name 
string(REPLACE "-" "_" PACKAGE_BUILDVERSION_ENVIRONMENT_VARIABLE_NAME ${PROJECT_NAME}_BUILDVERSION)
set(PACKAGE_BASE_NAME "qthardmon")
set(PACKAGE_FULL_LIBRARY_VERSION ${${PROJECT_NAME}_VERSION})
set(PACKAGE_GIT_URI "https://github.com/ChimeraTK/QtHardMon.git")
set(PACKAGE_TAG_VERSION ${${PROJECT_NAME}_VERSION})
set(PACKAGE_MESSAGE "Debian package for MTCA4U QtHardMon ${${PROJECT_NAME}_VERSION}")

#Nothing to change, just copy
file(COPY ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/compat
           ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/copyright
           ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/qthardmon.install
           ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/qthardmon-empty.install
           ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/rules
     DESTINATION debian_from_template)

file(COPY ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/source/format
     DESTINATION debian_from_template/source)

#Set the mtca4u-deviceaccess_MIN_VERSION
configure_file(${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/control.in
               debian_from_template/control @ONLY)

#Copy and configure the shell script which performs the actual 
#building of the package
configure_file(${CMAKE_SOURCE_DIR}/cmake/make_debian_package.sh.in
               make_debian_package.sh @ONLY)

#A custom target so you can just run make debian_package
#(You could instead run make_debian_package.sh yourself, hm...)
add_custom_target(debian_package ${CMAKE_BINARY_DIR}/make_debian_package.sh
                  COMMENT Building debian package for tag ${${PROJECT_NAME}_VERSION})

#For convenience: Also create an install script for DESY
#FIXME: PACKAGE_NAME and PACKAGE_BASE_NAME are redundant
set(PACKAGE_NAME ${PACKAGE_BASE_NAME})
set(PACKAGE_FILES_WILDCARDS "${PACKAGE_NAME}_*.deb ${PACKAGE_NAME}_*.changes")

configure_file(${CMAKE_SOURCE_DIR}/cmake/install_debian_package_at_DESY.sh.in
               install_debian_package_at_DESY.sh @ONLY)

