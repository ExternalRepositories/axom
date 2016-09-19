#!/bin/bash
# 09-12-2016 chang28, build-and-test.sh "clang@3.5.0" "Debug"
# 09-16-2016 chang28, build-and-test.sh "clang@3.5.0" "Debug" ""
# 09-19-2016 chang28, the decider has decided to have a configuration file call a main_script file, this is the configuration file, all environment variables are set up here. chaos5_build_test_all_compilers.sh "Debug" ""


HC="host-configs/surface-chaos_5_x86_64_ib-${COMPILER}.cmake"
#BT="Debug"
BT=$1
BP="atk_build"
IP="atk_install"
COMP_OPT=""
BUILD_OPT=$2
OPTIONS="-ecc -hc $HC -bt $BT -bp $BP -ip $IP $COMP_OPT $BUILD_OPT"

BUILD=true
TEST=true
DOC=false
INSTALL_FILES=false
INSTALL_DOCS=false

COMPILER="clang@3.5.0"
echo "Running "$COMPILER"
./main_script.sh
if [ $? -ne 0 ]; then
    echo "Error: 'calling ' $COMPILER '  failed"
    exit 1
fi

exit

    echo "Running tests..."
    echo "-----------------------------------------------------------------------"
    make test ARGS="-T Test -j16"
    if [ $? -ne 0 ]; then
        echo "Error: 'make test' failed"
        exit 1
    fi
    echo "-----------------------------------------------------------------------"

    echo "Installing files..."
    echo "-----------------------------------------------------------------------"
    make VERBOSE=1 install
    if [ $? -ne 0 ]; then
        echo "Error: 'make install' failed"
        exit 1
    fi
    echo "-----------------------------------------------------------------------"

