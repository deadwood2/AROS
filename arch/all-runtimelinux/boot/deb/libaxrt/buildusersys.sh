#!/bin/sh


AROSDIR=$1
SRCDIR=$2
DSTDIR=$3

#echo $AROSDIR
#echo $SRCDIR
#echo $DSTDIR

copyfromsrc ()
{
    srcpath="$SRCDIR/arch/all-runtimelinux/boot/deb/libaxrt/UserSYS/$1"
    dstpath="$DSTDIR/$1"
#    echo "copy from $srcpath to $dstpath"
    cp $srcpath $dstpath
}

copyallfromsrc ()
{
    srcpath="$SRCDIR/arch/all-runtimelinux/boot/deb/libaxrt/UserSYS/$1"
    dstpath="$DSTDIR/$1"
#    echo "copy all from $srcpath to $dstpath"
    cp -r $srcpath $dstpath
}

copyfromaros ()
{
    srcpath="$AROSDIR/$1"
    dstpath="$DSTDIR/$1"
#    echo "copy from $srcpath to $dstpath"
    cp $srcpath $dstpath
}

copyallfromaros ()
{
    srcpath="$AROSDIR/$1"
    dstpath="$DSTDIR/$1"
#    echo "copy all from $srcpath to $dstpath"
    cp -r $srcpath $dstpath
}

makedirdst ()
{
    dstpath="$DSTDIR/$1"
    mkdir -p $dstpath
}

# Empty system directories
makedirdst "Classes"
makedirdst "Devs"
makedirdst "Libs"
makedirdst "C"
makedirdst "L"

# Startup
makedirdst "S"
copyfromsrc "S/User-Startup"

# Prefs
makedirdst "Prefs"
makedirdst "Prefs/Presets/Themes"
copyallfromsrc "Prefs/Env-Archive"
cp -r $SRCDIR/images/Themes/pixblue $DSTDIR/Prefs/Presets/Themes
copyallfromaros "Prefs/Env-Archive/SYS/Certificates"

# DataTypes
copyallfromaros "Devs/DataTypes"

# Fonts
makedirdst "Fonts"
makedirdst "Fonts/TrueType"

copyfromsrc "Fonts/ttcourier.font"
copyallfromaros "Fonts/ttcourier"

copyfromsrc "Fonts/bitstreamverasansroman.font"
copyfromsrc "Fonts/bitstreamverasansroman.otag"
copyfromaros "Fonts/TrueType/VeraMono.ttf"
copyfromaros "Fonts/TrueType/VeraMonoBold.ttf"
copyfromaros "Fonts/TrueType/VeraMonoBoldItalic.ttf"
copyfromaros "Fonts/TrueType/VeraMonoItalic.ttf"
copyfromaros "Fonts/TrueType/VeraSans.ttf"
copyfromaros "Fonts/TrueType/VeraSansBold.ttf"
copyfromaros "Fonts/TrueType/VeraSansBoldItalic.ttf"
copyfromaros "Fonts/TrueType/VeraSansItalic.ttf"
copyfromaros "Fonts/TrueType/VeraSerif.ttf"
copyfromaros "Fonts/TrueType/VeraSerifBold.ttf"
