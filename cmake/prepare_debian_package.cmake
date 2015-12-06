#Prepare the debian control files from the template.
#Basically this is setting the correct version number in most of the files

#Nothing to change, just copy
file(COPY ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/compat
           ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/qthardmon.install
           ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/qthardmon-empty.install
           ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/rules
     DESTINATION debian_from_template)

file(COPY ${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/source/format
     DESTINATION debian_from_template/source)

#Set the mtca4u-deviceaccess_MIN_VERSION
configure_file(${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/control.in
               debian_from_template/control @ONLY)

#Set the version number
configure_file(${CMAKE_SOURCE_DIR}/cmake/debian_package_templates/copyright.in
               debian_from_template/copyright @ONLY)

#Copy and configure the shell script which performs the actual 
#building of the package
configure_file(${CMAKE_SOURCE_DIR}/cmake/make_debian_package.sh.in
               make_debian_package.sh @ONLY)

#A custom target so you can just run make debian_package
#(You could instead run make_debian_package.sh yourself, hm...)
add_custom_target(debian_package ${CMAKE_BINARY_DIR}/make_debian_package.sh
                  COMMENT Building debian package for tag ${${PROJECT_NAME}_VERSION})

#For convenience: Also create an install script for DESY
set(PACKAGE_NAME "qthardmon")
set(PACKAGE_FILES_WILDCARDS "${PACKAGE_NAME}_*.deb ${PACKAGE_NAME}_*.changes")

configure_file(${CMAKE_SOURCE_DIR}/cmake/install_debian_package_at_DESY.sh.in
               install_debian_package_at_DESY.sh @ONLY)

