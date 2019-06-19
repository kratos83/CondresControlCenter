cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

PROJECT(CondresControlCenter)

find_package(ECM 1.4.0 REQUIRED NO_MODULE)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})


include(ECMInstallIcons)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
INCLUDE(AddFileDependencies)
INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(FindPkgConfig)

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "2")
SET(VERSION_PATCH "4")

#SET(VERSION_SUFFIX "")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET(VERSION "${VERSION}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)

INCLUDE(DetectMachine)
include(ECMInstallIcons)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

LIST(APPEND CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX}")
#set(KF5_MIN_VERSION "5.31")

set(QT5 TRUE)

message(STATUS "Checking for Qt5...")
find_package(Qt5 REQUIRED Widgets Gui Core Sql Network DBus UiPlugin PrintSupport LinguistTools)

message(STATUS "Checking for KF5...")
find_package(KF5 REQUIRED COMPONENTS
    CoreAddons
    ConfigWidgets
    ItemModels
    Notifications)
    
message(STATUS "Checking for ICU...")
find_package(ICU REQUIRED)

message(STATUS "Checking for GLIB...")
find_package(Glib REQUIRED)

message(STATUS "Checking for ALPM...")
find_package(Alpm REQUIRED)

message(STATUS "Checking form ALPM OCTOPI UTILS...")
find_package(AlpmOctopi REQUIRED)

# set compiler flags
SET(WERROR_FLAGS "-Wall -Werror=unused-function -Wno-sign-compare -Wno-strict-overflow")
OPTION(USE_WERROR "Add -werror to the build flags. Stops the build on warnings" OFF)
IF(${USE_WERROR})
   SET(WERROR_FLAGS "${WERROR_FLAGS} -Werror")
ENDIF()

# Due to a regression in gcc-4.8.X, we need to disable array-bounds check
IF (CMAKE_COMPILER_IS_GNUCXX AND ((CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.8.0") OR (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.8.0") OR FastAss_BUILD_WIN32))
   SET(WERROR_FLAGS "${WERROR_FLAGS} -Wno-array-bounds")
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
  		"MinSizeRel" "RelWithDebInfo")
ENDIF()

SET(CMAKE_C_FLAGS "${WERROR_FLAGS} ${CMAKE_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "-fno-exceptions ${WERROR_FLAGS} ${CMAKE_CXX_FLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCONDRESCONTROLCENTER_DEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCONDRESCONTROLCENTER_DEBUG")


# make sub-directories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(notifier/pacmanhelper)
ADD_SUBDIRECTORY(notifier/condres-notifier)

#
# add distclean-target
#
ADD_CUSTOM_TARGET(distclean
			COMMAND make clean
			COMMAND rm -rf `find -name cmake_install.cmake` `find -name Makefile` `find -type d -name CMakeFiles` CMakeCache.txt)

#
# display configuration information
#

MESSAGE("\n"
"Installation Summary\n"
"--------------------\n"
"* Install Directory           : ${CMAKE_INSTALL_PREFIX}\n"
"* Build type                  : ${CMAKE_BUILD_TYPE}\n"
"* Platform                    : ${CMAKE_SYSTEM_PROCESSOR} (${Machine})\n"
)

MESSAGE(
"\n"
"-----------------------------------------------------------------\n"
"IMPORTANT:\n"
"after installing missing packages, remove CMakeCache.txt before\n"
"running cmake again!\n"
"-----------------------------------------------------------------\n"
"\n\n")

# add_custom_target(uninstall
#     "${CMAKE_COMMAND}" -P "${CMAKE_MODULE_PATH}/uninstall.cmake"
# )
