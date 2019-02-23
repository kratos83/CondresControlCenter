project(NotifierControlCenter)

cmake_minimum_required(VERSION 2.6)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0043 NEW)
ENDIF(COMMAND CMAKE_POLICY)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )

SET( CMAKE_CXX_FLAGS  "-fexceptions -std=c++11 -fPIC" )

set(NotifierControlCenter "NotifierControlCenter")

set(NotifierControlCenter_includes
${CMAKE_BINARY_DIR}
../src
../src/gui
)

include_directories(${NotifierControlCenter_includes})

set(NotifierControlCenter_moc_hdrs 
	CchAdaptor.h
	CchClient.h
	CchHelper.h
	NotifierCondres.h
	../src/about.h
	../src/settingsmanager.h
)

qt5_wrap_cpp(NotifierControlCenter_mocs ${NotifierControlCenter_moc_hdrs})

set(NotifierControlCenter_uis 
	../src/gui/about.ui
)

qt5_wrap_ui(NotifierControlCenter_ui_hdrs ${NotifierControlCenter_uis})

set(NotifierControlCenter_srcs 
	CchAdaptor.cpp
	CchClient.cpp
	CchHelper.cpp
	main.cpp
	NotifierCondres.cpp
	../src/about.cpp
	../src/settingsmanager.cpp
)

qt5_add_resources(RESOURCE_FILES ../src/images.qrc) 

ADD_EXECUTABLE(NotifierControlCenter ${NotifierControlCenter_srcs} ${CondresControlCenter_ui_hdrs} ${NotifierControlCenter_mocs} ${RESOURCE_FILES} ${WINRC} ${SOURCE_FILES})

target_link_libraries( NotifierControlCenter
  Qt5::Widgets Qt5::Network Qt5::Gui Qt5::DBus Qt5::Core
)

INSTALL( TARGETS NotifierControlCenter RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
INSTALL( FILES "../src/images/db_green.png" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons)
INSTALL( FILES "NotifierControlCenter.desktop" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
INSTALL( FILES "NotifierControlCenter.desktop" DESTINATION /etc/xdg/autostart )
#Install polkit
INSTALL( FILES "polkit/org.condrescontrolcenter.pacman.policy" DESTINATION /usr/share/polkit-1/actions )
INSTALL( FILES "polkit/org.condrescontrolcenter.pacmanhelper.xml" DESTINATION /usr/share/dbus-1/interfaces )
INSTALL( FILES "polkit/org.condrescontrolcenter.pacmanhelper.service" DESTINATION /usr/share/dbus-1/system-services )
INSTALL( FILES "polkit/org.condrescontrolcenter.pacmanhelper.conf" DESTINATION /etc/dbus-1/system.d )
