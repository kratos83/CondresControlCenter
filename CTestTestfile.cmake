# CMake generated Testfile for 
# Source directory: /home/angelo/Progetti/progetti_completi/control_center/CondresControlCenter
# Build directory: /home/angelo/Progetti/progetti_completi/control_center/CondresControlCenter
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(appstreamtest "/usr/bin/cmake" "-DAPPSTREAMCLI=/bin/appstreamcli" "-DINSTALL_FILES=/home/angelo/Progetti/progetti_completi/control_center/CondresControlCenter/install_manifest.txt" "-P" "/usr/share/ECM/kde-modules/appstreamtest.cmake")
set_tests_properties(appstreamtest PROPERTIES  _BACKTRACE_TRIPLES "/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;183;add_test;/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;201;appstreamtest;/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;0;;/home/angelo/Progetti/progetti_completi/control_center/CondresControlCenter/CMakeLists.txt;12;include;/home/angelo/Progetti/progetti_completi/control_center/CondresControlCenter/CMakeLists.txt;0;")
subdirs("src")
subdirs("notifier/pacmanhelper")
subdirs("notifier/condres-notifier")
