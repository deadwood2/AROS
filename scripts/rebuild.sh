#!/bin/bash

# Variables
# TOOLCHAIN_DIR     -> directory where toolchain will be installed or used from
# TOOLCHAIN_BUILD   -> if yes, we are building toolchain
# PORTS_DIR         -> directory where ports sources will be placed for re-use
# CONFIGURE_OPTS    -> additional options passed to "configure"
# CONFIGURE_TARGET  -> AROS build system style target
# BUILD_DIR         -> directory where build output will be placed
# MAKE_TARGET       -> "make" target to build
# MAKE_TARGET_2     -> second "make" target to build

# Note: each valid selection has to set TOOLCHAIN_DIR, BUILD_DIR and
# CONFIGURE_TARGET at minimum in process_selection()

TOOLCHAIN_BUILD=no
PORTS_DIR=$(pwd)/portssources
CONFIGURE_OPTS=""

check_location ()
{
    if [[ ! -d $(pwd)/AROS/scripts ]]; then
        printf "This script must be placed next to AROS source repository (same top level directory)\n"
        printf "Exiting...\n"
        exit 1
    fi
}

source $(pwd)/AROS/scripts/rebuild-conf

check_location

printf "rebuild.sh v1.2, select an option:\n"
printf "    0) exit\n"

show_selection

read input
if [[ $input = 0 ]]; then
    exit 0
fi

process_selection $input

if [ -z $TOOLCHAIN_DIR ]
then
    printf "Toolchain directory not set. Exiting\n"
    exit 0
fi

# Listing directories
printf "Toolchain : "
printf $TOOLCHAIN_DIR
printf "\nBuild : "
printf $BUILD_DIR
printf "\nPorts sources : "
printf $PORTS_DIR
printf "\n"
sleep 3

rm -rf $BUILD_DIR
mkdir $BUILD_DIR

# Delete toolchain directory only if toolchain is being build
if [ $TOOLCHAIN_BUILD = yes ]; then
    rm -rf $TOOLCHAIN_DIR
    mkdir $TOOLCHAIN_DIR
fi

# Main build
cd $BUILD_DIR
../AROS/configure --target=$CONFIGURE_TARGET --with-aros-toolchain-install=$TOOLCHAIN_DIR --with-portssources=$PORTS_DIR $CONFIGURE_OPTS
make $MAKE_TARGET -j 3
if [[ -n $MAKE_TARGET_2 ]]
then
    make $MAKE_TARGET_2 -j 3
fi
cd ..

# Delete build directory if toolchain is being build
if [ $TOOLCHAIN_BUILD = yes ]; then
    rm -rf $BUILD_DIR
fi
