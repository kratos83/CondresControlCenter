project(CondresControlCenter)

cmake_minimum_required(VERSION 2.6)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0043 NEW)
ENDIF(COMMAND CMAKE_POLICY)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )

SET( CMAKE_CXX_FLAGS  "-fexceptions -std=c++11 -fPIC" )

set(CondresControlCenter "CondresControlCenter")

set(CondresControlCenter_includes
${CMAKE_BINARY_DIR}
)

include_directories(${CondresControlCenter_includes} ${ALPM_INCLUDE_DIR} ${ALPMOCTOPI_INCLUDE_DIR} ${GLIB_INCLUDE_DIRS})

set(CondresControlCenter_moc_hdrs 
	backend/backend.h
	cachecleaner/cachecleaner.h
	cachecleaner/packagegroupmodel.h
	repoeditor/addrepo.h
	repoeditor/checkboxdelegate.h
	repoeditor/optionsdelegate.h
	repoeditor/repoconf.h
	repoeditor/repoeditor.h
	repoeditor/repoentry.h
	about.h
	constant.h
	esci.h
	InstallPackages.h
	logfile.h
	mainwindow.h
	tab_style.h
	tablewidgetheader.h
	upgrade.h
)


qt5_wrap_cpp(CondresControlCenter_mocs ${CondresControlCenter_moc_hdrs})


set(CondresControlCenter_uis 
	cachecleaner/ui/cachecleaner.ui
	gui/about.ui
	gui/Apply.ui
	gui/esci.ui
	gui/InstallPackages.ui
	gui/logfile.ui
	gui/mainwindow.ui
	gui/upgrade.ui
	repoeditor/addrepo.ui
	repoeditor/repoeditor.ui
)

qt5_wrap_ui(CondresControlCenter_ui_hdrs ${CondresControlCenter_uis})


set(CondresControlCenter_srcs 
	backend/backend.cpp
	cachecleaner/cachecleaner.cpp
	cachecleaner/packagegroupmodel.cpp
	repoeditor/addrepo.cpp
	repoeditor/checkboxdelegate.cpp
	repoeditor/optionsdelegate.cpp
	repoeditor/repoconf.cpp
	repoeditor/repoeditor.cpp
	repoeditor/repoentry.cpp
	about.cpp
	Apply.cpp
	esci.cpp
	InstallPackages.cpp
	logfile.cpp
	main.cpp
	mainwindow.cpp
	tab_style.cpp
	tablewidgetheader.cpp
	upgrade.cpp
)

qt5_add_resources(RESOURCE_FILES images.qrc) 



ADD_EXECUTABLE(CondresControlCenter ${CondresControlCenter_srcs} ${CondresControlCenter_mocs} ${CondresControlCenter_ui_hdrs} ${RESOURCE_FILES} ${WINRC} ${SOURCE_FILES})

target_link_libraries( CondresControlCenter
  Qt5::Widgets Qt5::Network Qt5::Gui
  Qt5::Core ${ALPM_LIBRARIES} ${ALPMOCTOPI_LIBRARIES} ${GLIB_LIBRARIES}
)

INSTALL( TARGETS CondresControlCenter RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
INSTALL( FILES "bash/CondresControlCenterBin" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE)
INSTALL( FILES "bash/CondresControlCenterInstallBin" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE)
INSTALL( FILES "bash/CondresControlCenterUpdateBin" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE)
INSTALL( FILES "images/preferences-system.png" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons)
INSTALL( FILES "desktop/CondresControlCenter.desktop" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
