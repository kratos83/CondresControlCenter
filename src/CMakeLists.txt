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
	settingsmanager.h
	tab_style.h
	tablewidgetheader.h
	upgrade.h
	PageWidget.h
	Samba.h
	nfsshare.h
	keyboard/KeyboardCommon.h
	keyboard/KeyboardItem.h
	keyboard/KeyboardModel.h
	keyboard/KeyboardPreview.h
	keyboard/KeyboardPage.h
	keyboard/SetKeyboardLayoutJob.h
	mhwd/libmhwd/vita/string.hpp
	mhwd/libmhwd/const.h
	mhwd/libmhwd/mhwd.h
	mhwd/libmhwd/mhwd_p.h
	mhwd/MhwdCommon.h
	mhwd/MhwdPage.h
	users/AccountTypeDialog.h
	users/AddUserDialog.h
	users/ChangePasswordDialog.h
	users/FlatButton.h
	users/PreviewFileDialog.h
	users/UsersCommon.h
	users/UsersPage.h
	locale/EnabledLocalesModel.h
	locale/LanguageCommon.h
	locale/LanguageListViewDelegate.h
	locale/LocaleCommon.h
	locale/LocalePage.h
	locale/SelectLocalesDialog.h
	locale/SupportedLocalesItem.h
	locale/SupportedLocalesModel.h
	timedate/LocaleGlobal.h
	timedate/TimeDateCommon.h
	timedate/TimeDatePage.h
	timedate/TimeDateService.h
	timedate/TimeZoneDialog.h
	timedate/TimeZoneWidget.h
	ListWidget.h
	CheckBox.h
	RadioButton.h
	itdelegato.h
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
	gui/nfsshare.ui
	gui/NfsConfig.ui
	gui/Samba.ui
	gui/samba_conf_dir.ui
	gui/SambaPrinter.ui
	gui/sambauser.ui
	gui/upgrade.ui
	repoeditor/addrepo.ui
	repoeditor/repoeditor.ui
	keyboard/ui/PageKeyboard.ui
	mhwd/ui/PageMhwd.ui
	users/ui/AccountTypeDialog.ui
	users/ui/AddUserDialog.ui
	users/ui/ChangePasswordDialog.ui
	users/ui/PageUsers.ui
	locale/ui/LocaleModule.ui
	locale/ui/SelectLocalesDialog.ui
	timedate/ui/PageTimeDate.ui
	timedate/ui/TimeZoneDialog.ui
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
	settingsmanager.cpp
	tab_style.cpp
	tablewidgetheader.cpp
	upgrade.cpp
	PageWidget.cpp
	Samba.cpp
	nfsshare.cpp
	keyboard/KeyboardCommon.cpp
	keyboard/KeyboardItem.cpp
	keyboard/KeyboardModel.cpp
	keyboard/KeyboardPreview.cpp
	keyboard/KeyboardPage.cpp
	keyboard/SetKeyboardLayoutJob.cpp
	mhwd/libmhwd/vita/string.cpp
	mhwd/libmhwd/mhwd.cpp
	mhwd/MhwdCommon.cpp
	mhwd/MhwdPage.cpp
	users/AccountTypeDialog.cpp
	users/AddUserDialog.cpp 
	users/ChangePasswordDialog.cpp
	users/FlatButton.cpp
	users/PreviewFileDialog.cpp
	users/UsersCommon.cpp
	users/UsersPage.cpp
	locale/EnabledLocalesModel.cpp
	locale/LanguageCommon.cpp
	locale/LanguageListViewDelegate.cpp 
	locale/LocaleCommon.cpp
	locale/LocalePage.cpp
	locale/SelectLocalesDialog.cpp
	locale/SupportedLocalesItem.cpp
	locale/SupportedLocalesModel.cpp
	timedate/LocaleGlobal.cpp
	timedate/TimeDateCommon.cpp
	timedate/TimeDatePage.cpp
	timedate/TimeDateService.cpp
	timedate/TimeZoneDialog.cpp
	timedate/TimeZoneWidget.cpp
	ListWidget.cpp
	CheckBox.cpp
	RadioButton.cpp
	itdelegato.cpp
)

qt5_add_resources(RESOURCE_FILES images.qrc keyboard/keyboard.qrc mhwd/mhwd.qrc users/users.qrc timedate/timedate.qrc) 

ADD_EXECUTABLE(CondresControlCenter ${CondresControlCenter_srcs} ${CondresControlCenter_mocs} ${CondresControlCenter_ui_hdrs} ${RESOURCE_FILES})

target_link_libraries( CondresControlCenter
  Qt5::Widgets Qt5::Network Qt5::Gui Qt5::DBus Qt5::Sql Qt5::PrintSupport
  Qt5::Core ${ALPM_LIBRARIES} ${ALPMOCTOPI_LIBRARIES} ${GLIB_LIBRARIES}
  KF5::ItemModels KF5::ConfigWidgets KF5::CoreAddons hd icudata
    icuuc
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
INSTALL( FILES "keyboard/kbd-model-map" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/systemd)
INSTALL( FILES "database/condrescontrolcenter.db" DESTINATION /usr/lib/condrescontrolcenter)
