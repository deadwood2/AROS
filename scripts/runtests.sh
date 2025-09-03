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

if [[ $1 == 'clean' ]]
then
rm -rf $AROSBUILDDIR
fi

# Preparing run environment
mkdir -p $AROSRUNDIR
rm -rf $AROSLOGDIR
mkdir -p $AROSLOGDIR
rsync -r -t -l --info=progress2 -s --exclude=*.dbg ./core-linux-x86_64-d/bin/linux-x86_64/AROS/* $AROSRUNDIR
cp ./core-linux-x86_64-d/bin/linux-x86_64/AROS/.gdb_ignore_errors.py $AROSRUNDIR
cp ./core-linux-x86_64-d/bin/linux-x86_64/AROS/.gdbinit $AROSRUNDIR

cp $AROSSRCDIR/scripts/nightly/autotest/User-Startup.cunit $AROSRUNDIR/S/User-Startup
cp $AROSSRCDIR/scripts/nightly/autotest/Try $AROSRUNDIR/S/
cp $AROSSRCDIR/scripts/nightly/autotest/Test.cunit $AROSRUNDIR/S/Test
mv $AROSRUNDIR/Devs/Monitors/X1* $AROSRUNDIR/Storage/Monitors
sed -i "s|Startup = 0|Startup = 1|g" $AROSRUNDIR/Devs/DOSDrivers/DEBUG
sed -i "s|module Devs/Drivers/gfx.hidd|&\nmodule Storage/Drivers/headlessgfx.hidd|g" $AROSRUNDIR/boot/linux/AROSBootstrap.conf
sed -i "/module Devs\/Drivers\/x11gfx.hidd/d" $AROSRUNDIR/boot/linux/AROSBootstrap.conf
find $AROSRUNDIR/ -name "*-Results.xml" -exec rm {} \;

# Running tests
cd $AROSRUNDIR
./boot/linux/AROSBootstrap -m 640

# Generating results
cd ../../../../
find $AROSRUNDIR/ -name "*-Results.xml" -exec cp {} $AROSLOGDIR/ \;
$AROSSRCDIR/scripts/merge_junit_results.py $AROSLOGDIR > $AROSLOGDIR/all-unittests.xml
junit2html $AROSLOGDIR/all-unittests.xml $AROSLOGDIR/all-unittests.html

# Checking if all suites were executed
sed -En "s/.*;(.*Suite).*/\1/gp" $AROSRUNDIR/S/User-Startup > $AROSLOGDIR/all-test-suites-in
sort -o $AROSLOGDIR/all-test-suites-in  $AROSLOGDIR/all-test-suites-in
sort -o $AROSLOGDIR/all-test-suites-out $AROSLOGDIR/all-test-suites-out

# Print out results
sed -En "s/^<testsuites.*(fail.*).*><test.*/\1/gp" $AROSLOGDIR/all-unittests.xml
diff -ruN $AROSLOGDIR/all-test-suites-in $AROSLOGDIR/all-test-suites-out
