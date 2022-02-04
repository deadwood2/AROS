## Required packages

Please install these packages before moving to next step. Below is a reference list for Debian-based distributions. Reference build system is Ubuntu 18.04/20.04 amd64.

    subversion git-core gcc g++ make gawk bison flex bzip2 netpbm autoconf automake libx11-dev libxext-dev libc6-dev liblzo2-dev libxxf86vm-dev libpng-dev gcc-multilib libsdl1.2-dev byacc python-mako python3-mako libxcursor-dev cmake genisoimage libjpeg-dev

## Clone & build

    $ mkdir myrepo
    $ cd myrepo
    $ git clone https://github.com/deadw00d/AROS.git AROS
    $ cd AROS
    $ git checkout alt-runtime
    $ cd ..
    $ cp ./AROS/scripts/rebuild.sh .
    $ ./rebuild.sh

Proceed to build selection below

### Runtimelinux-x86_64

1. Select alt-runtimelinux-x86_64 (DEBUG)

Runtime binaries available in

    alt-runtimelinux-x86_64-d/bin/runtimelinux-x86_64/AROS

### Validating build

Build Wanderer

    $ cd alt-runtimelinux-x86_64-d
    $ make workbench-system-wanderer

Configure setup

    $ mkdir ~/.axrt/
    $ mkdir -p ~/.axrt/Prefs/Presets/Themes
    $ cp -r ../AROS/arch/all-runtimelinux/boot/deb/libaxrt/UserSYS/* ~/.axrt/
    $ cp -r ../AROS/images/Themes/pixblue ~/.axrt/Prefs/Presets/Themes
    $ export AXRT_ROOT=<myrepo-absolute-path>/alt-runtimelinux-x86_64-d/bin/runtimelinux-x86_64/AROS

Copy loader

    $ cd bin/runtimelinux-x86_64/AROS/System/Wanderer
    $ cp ../../Development/lib/libaxrt-4.0.so .

Start Wanderer

    $ ./Wanderer
