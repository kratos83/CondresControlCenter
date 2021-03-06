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
../../src
../../src/gui
)

include_directories(${NotifierControlCenter_includes} ${ALPM_INCLUDE_DIR} ${ALPMOCTOPI_INCLUDE_DIR} ${GLIB_INCLUDE_DIRS})

set(NotifierControlCenter_moc_hdrs 
	../pacmanhelper/pacmanhelperclient.h
	NotifierCondres.h
	NotifierApp.h
	../../src/about.h
	../../src/settingsmanager.h
	../../src/backend/backend.h
)

qt5_wrap_cpp(NotifierControlCenter_mocs ${NotifierControlCenter_moc_hdrs})

set(NotifierControlCenter_uis 
	../../src/gui/about.ui
)

qt5_wrap_ui(NotifierControlCenter_ui_hdrs ${NotifierControlCenter_uis})

set(KDSINGLEAPPLICATIONGUARD
    kdsingleapplicationguard/kdsingleapplicationguard.cpp
    kdsingleapplicationguard/kdsharedmemorylocker.cpp
    kdsingleapplicationguard/kdtoolsglobal.cpp
    kdsingleapplicationguard/kdlockedsharedmemorypointer.cpp)
    
set_source_files_properties(
    ${KDSINGLEAPPLICATIONGUARD}
    PROPERTIES
    COMPILE_FLAGS "-Wno-unused-function")
    
set(NotifierControlCenter_srcs 
    ${KDSINGLEAPPLICATIONGUARD}
	../pacmanhelper/pacmanhelperclient.cpp
	main.cpp
	NotifierApp.cpp
	NotifierCondres.cpp
	../../src/about.cpp
	../../src/settingsmanager.cpp
	../../src/backend/backend.cpp
)

set_source_files_properties(
    ${NotifierControlCenter_srcs}
    PROPERTIES
    COMPILE_FLAGS "-Wno-unused-function")
    
qt5_add_resources(RESOURCE_FILES ../../src/images.qrc) 

ADD_EXECUTABLE(NotifierControlCenter ${NotifierControlCenter_srcs} ${CondresControlCenter_ui_hdrs} ${NotifierControlCenter_mocs} ${RESOURCE_FILES} ${WINRC} ${SOURCE_FILES})

target_compile_definitions(NotifierControlCenter PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries( NotifierControlCenter
  Qt5::Widgets Qt5::Network Qt5::Gui Qt5::DBus Qt5::Core KF5::Notifications
  ${ALPM_LIBRARIES} ${ALPMOCTOPI_LIBRARIES} ${GLIB_LIBRARIES}
)

INSTALL( TARGETS NotifierControlCenter RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
INSTALL( FILES "../../src/images/db_green.png" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons)
INSTALL( FILES "NotifierControlCenter.desktop" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
INSTALL( FILES "NotifierControlCenter.desktop" DESTINATION ${KDE_INSTALL_AUTOSTARTDIR})
