## Required packages

Please install these packages before moving to next step. Below is a reference list for Debian-based distributions. Reference build system is Ubuntu 20.04/22.04 amd64.

    git gcc g++ make gawk bison flex bzip2 netpbm autoconf automake libx11-dev libxext-dev libc6-dev liblzo2-dev libxxf86vm-dev libpng-dev gcc-multilib libsdl1.2-dev byacc python3-mako libxcursor-dev cmake genisoimage dh-make

For armhf build, please install additional packages:

    gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

## Clone & build

    $ cd <myprojects>
    $ mkdir arosbuilds
    $ cd arosbuilds
    $ git clone https://github.com/deadw00d/AROS.git AROS
    $ cp ./AROS/scripts/rebuild.sh .
    $ ./rebuild.sh

Proceed to build selection below

### Linux-x86_64

1. Select toolchain-core-x86_64
2. Select core-linux-x86_64 (DEBUG)

Start AROS by:

    $ cd core-linux-x86_64-d/bin/linux-x86_64/AROS
    $ ./boot/linux/AROSBootstrap

In order to use the cross-compiler built in step 1 for compiling your own projects (which don't use AROS builds system / mmakefile.src approach), you need to provide --sysroot parameter, for example:

    $ <myprojects>/arosbuilds/toolchain-core-x86_64/x86_64-aros-gcc --sysroot=<myprojects>/arosbuilds/core-linux-x86_64-d/bin/linux-x86_64/AROS/Development

### Amiga-m68k

1. Select toolchain-core-m68k
2. Select core-amiga-m68k

Kickstart images available in

    core-amiga-m68k/bin/amiga-m68k/AROS/boot/amiga
