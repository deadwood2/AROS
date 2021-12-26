#!/bin/bash

check_location ()
{
    if [[ ! -d $(pwd)/AROS/scripts ]]; then
        printf "This script must be placed next to AROS source repository (same top level directory)\n"
        printf "Exiting...\n"
        exit 1
    fi
}

check_location

AROSBUILDDIR=$(pwd)/core-linux-x86_64-tests

AROSSRCDIR=$(pwd)/AROS
AROSRUNDIR=$AROSBUILDDIR/bin/linux-x86_64/AROS
AROSLOGDIR=$AROSBUILDDIR/logs

rm -rf $AROSBUILDDIR

mkdir -p $AROSRUNDIR
mkdir -p $AROSLOGDIR
cp -r ./core-linux-x86_64-d/bin/linux-x86_64/AROS/* $AROSRUNDIR

cp $AROSSRCDIR/scripts/nightly/autotest/User-Startup.cunit $AROSRUNDIR/S/User-Startup
cp $AROSSRCDIR/scripts/nightly/autotest/Try $AROSRUNDIR/S/
cp $AROSSRCDIR/scripts/nightly/autotest/Test.cunit $AROSRUNDIR/S/Test
mv $AROSRUNDIR/Devs/Monitors/X1* $AROSRUNDIR/Storage/Monitors
sed -i "s|Startup = 0|Startup = 1|g" $AROSRUNDIR/Devs/DOSDrivers/DEBUG
sed -i "s|module Devs/Drivers/gfx.hidd|&\nmodule Storage/Drivers/headlessgfx.hidd|g" $AROSRUNDIR/boot/linux/AROSBootstrap.conf
sed -i "/module Devs\/Drivers\/x11gfx.hidd/d" $AROSRUNDIR/boot/linux/AROSBootstrap.conf
cd $AROSRUNDIR
./boot/linux/AROSBootstrap
cd ../../../../
find $AROSRUNDIR/ -name "*-Results.xml" -exec cp {} $AROSLOGDIR/ \;
$AROSSRCDIR/scripts/merge_junit_results.py $AROSLOGDIR > $AROSLOGDIR/all-unittests.xml
junit2html $AROSLOGDIR/all-unittests.xml $AROSLOGDIR/all-unittests.html
