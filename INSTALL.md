## Required packages

Please install these packages before moving to next step. Below is a reference list for Debian-based distributions. Reference build system is Ubuntu 20.04/22.04 amd64.

    git gcc g++ make gawk bison flex bzip2 netpbm autoconf automake libx11-dev libxext-dev libc6-dev liblzo2-dev libxxf86vm-dev libpng-dev gcc-multilib libsdl1.2-dev byacc python3-mako libxcursor-dev cmake genisoimage dh-make yasm curl zsh mingw64

For armhf build, please install additional packages:

    gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

## Clone & build

    $ cd <myprojects>
    $ mkdir arosbuilds
    $ cd arosbuilds
    $ git clone https://github.com/deadwood2/AROS.git AROS
    $ cd AROS
    $ git checkout alt-abiv0
    $ cd ..
    $ cp ./AROS/scripts/rebuild.sh .
    $ ./rebuild.sh

Proceed to build selection below

### Linux-i386

1. Select toolchain-alt-abiv0-i386
2. Select alt-abiv0-linux-i386 (DEBUG)

Start AROS by:

    $ cd alt-abiv0-linux-i386/bin/linux-i386/AROS
    $ ./Arch/linux/AROSBootstrap

In order to use the cross-compiler built in step 1 for compiling your own projects (which don't use AROS builds system / mmakefile.src approach), you need to provide --sysroot parameter, for example:

    $ <myprojects>/arosbuilds/toolchain-core-x86_64/x86_64-aros-gcc --sysroot=<myprojects>/arosbuilds/core-linux-x86_64-d/bin/linux-x86_64/AROS/Development

### Pc-i386

1. Select toolchain-alt-abiv0-i386 (if not built yet)
2. Select alt-abiv0-pc-i386

ISO image available in

    alt-abiv0-pc-i386/distfiles
