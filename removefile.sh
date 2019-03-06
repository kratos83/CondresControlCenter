#!/bin/sh

echo "Remove file cpp_parameters"
find . -type f -name '*.cpp_parameters' -delete
echo "Remove file cbp"
find . -type f -name '*.cbp' -delete
echo "Remove file depends"
find . -type f -name '*.depends' -delete
echo "Remove file CMakeCache"
find . -type f -name 'CMakeCache.txt' -delete
echo "Remove file ui_*"
find . -type f -name 'ui_*' -delete
echo "Remove file Makefile"
find . -type f -name 'Makefile' -delete
echo "Remove file CMakeFiles"
find . -type d -name 'CMakeFiles' -exec rm -rf {} \;
echo "Remove file CPackConfig.cmake"
find . -type f -name 'CPackConfig.cmake' -delete
echo "Remove file CPackSourceConfig.cmake"
find . -type f -name 'CPackSourceConfig.cmake' -delete
echo "Remove file compile_commands.json"
find . -type f -name 'compile_commands.json' -delete
echo "Remove file parameter"
find . -type f -name '*.cxx_parameters' -delete
echo "Remove file moc"
find . -type f -name 'moc_*.cpp' -delete
echo "Remove file install_cmake"
find . -type f -name 'cmake_install.cmake' -delete
echo "Remove file install_manifest"
find . -type f -name 'install_manifest.txt' -delete
echo "Remove file CTestTestfile.cmake"
find . -type f -name 'CTestTestfile.cmake' -delete
echo "Remove file ecm_uninstall.cmake"
find . -type f -name 'ecm_uninstall.cmake' -delete
echo "Remove file prefix.sh"
find . -type f -name 'prefix.sh' -delete
