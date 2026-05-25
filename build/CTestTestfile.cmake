# CMake generated Testfile for 
# Source directory: /mnt/c/git-repo/sentinex
# Build directory: /mnt/c/git-repo/sentinex/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[sentinex.tests]=] "/mnt/c/git-repo/sentinex/build/sentinex-test")
set_tests_properties([=[sentinex.tests]=] PROPERTIES  _BACKTRACE_TRIPLES "/mnt/c/git-repo/sentinex/CMakeLists.txt;95;add_test;/mnt/c/git-repo/sentinex/CMakeLists.txt;0;")
subdirs("_deps/doctest-build")
subdirs("_deps/rapidcheck-build")
