## Required packages

Please install these packages before moving to next step. Below is a reference list for Debian-based distributions. Reference build system is Ubuntu 22.04/24.04 amd64.

    git gcc g++ make gawk bison flex bzip2 netpbm autoconf automake libx11-dev libxext-dev libc6-dev liblzo2-dev libxxf86vm-dev libpng-dev gcc-multilib libsdl1.2-dev byacc python3-mako libxcursor-dev cmake genisoimage dh-make yasm curl  libjpeg-dev libxinerama-dev

## Clone & build

    $ cd <myprojects>
    $ mkdir arosbuilds
    $ cd arosbuilds
    $ git clone https://github.com/deadwood2/AROS.git AROS
    $ cd AROS
    $ git checkout alt-runtime
    $ cd ..
    $ cp ./AROS/scripts/rebuild.sh .

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

    $ mkdir ~/SYS/
    $ ../AROS/arch/all-runtimelinux/boot/deb/libaxrt/buildusersys.sh ./bin/runtimelinux-x86_64/AROS ../AROS ~/SYS
    $ export AXRT_ROOT=<myrepo-absolute-path>/alt-runtimelinux-x86_64-d/bin/runtimelinux-x86_64/AROS

Copy loader

    $ cd bin/runtimelinux-x86_64/AROS/System/Wanderer
    $ cp ../../Development/lib/libaxrt-4.0.so .

Start Wanderer

    $ ./Wanderer

