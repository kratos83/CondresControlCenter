project(pacmanhelper)

cmake_minimum_required(VERSION 2.6)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0043 NEW)
ENDIF(COMMAND CMAKE_POLICY)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )

SET( CMAKE_CXX_FLAGS  "-fexceptions -std=c++11 -fPIC" )

set(pacmanhelper "pacmanhelper")

set(pacmanhelper_includes
${CMAKE_BINARY_DIR}
)

include_directories(${pacmanhelper_includes})

set(pacmanhelper_moc_hdrs 
	CchAdaptor.h
	CchClient.h
	CchHelper.h
)

qt5_wrap_cpp(pacmanhelper_mocs ${pacmanhelper_moc_hdrs})


set(pacmanhelper_srcs 
	CchAdaptor.cpp
	CchClient.cpp
	CchHelper.cpp
	main.cpp
)


ADD_EXECUTABLE(pacmanhelper ${pacmanhelper_srcs} ${CondresControlCenter_ui_hdrs} ${pacmanhelper_mocs} ${RESOURCE_FILES} ${WINRC} ${SOURCE_FILES})

target_link_libraries( pacmanhelper
  Qt5::Widgets Qt5::Network Qt5::Gui Qt5::DBus Qt5::Core
)

INSTALL( TARGETS pacmanhelper RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/condrescontrolcenter/)

#Install polkit
INSTALL( FILES "polkit/org.condrescontrolcenter.pacman.policy" DESTINATION /usr/share/polkit-1/actions )
INSTALL( FILES "polkit/org.condrescontrolcenter.pacmanhelper.xml" DESTINATION /usr/share/dbus-1/interfaces )
INSTALL( FILES "polkit/org.condrescontrolcenter.pacmanhelper.service" DESTINATION /usr/share/dbus-1/system-services )
INSTALL( FILES "polkit/org.condrescontrolcenter.pacmanhelper.conf" DESTINATION /etc/dbus-1/system.d )
