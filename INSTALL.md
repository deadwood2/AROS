## Required packages

Please install these packages before moving to next step. Below is a reference list for Debian-based distributions. Reference build system is Ubuntu 22.04/24.04 amd64.

    git gcc g++ make gawk bison flex bzip2 netpbm autoconf automake libx11-dev libxext-dev libc6-dev liblzo2-dev libxxf86vm-dev libpng-dev gcc-multilib libsdl1.2-dev byacc python3-mako libxcursor-dev cmake genisoimage dh-make yasm curl zsh mingw64

## Clone & build

    $ cd <myprojects>
    $ mkdir arosbuilds
    $ cd arosbuilds
    $ git clone https://github.com/deadwood2/AROS.git AROS
    $ cd AROS
    $ git checkout alt-abiv0
    $ cd ..
    $ cp ./AROS/scripts/rebuild.sh .

Proceed to build selection below

### Linux-i386 (AROS that is a "program" running from Linux)

First, build the cross-compiler by running

    $ ./rebuild.sh

and selecting option ```1)  toolchain-alt-abiv0-i386```. Once the cross-compiler build is complete run ```$ ./rebuild.sh``` again and select option ```3)  alt-abiv0-linux-i386 (DEBUG)``` to build AROS.

Start AROS by:

    $ cd alt-abiv0-linux-i386/bin/linux-i386/AROS
    $ ./Arch/linux/AROSBootstrap

In order to use the cross-compiler for compiling your own projects (which don't use AROS builds system / mmakefile.src approach), you need to provide --sysroot parameter, for example:

    $ <myprojects>/arosbuilds/toolchain-alt-abiv0-i386/i386-aros-gcc --sysroot=<myprojects>/arosbuilds/alt-abiv0-linux-i386-d/bin/linux-i386/AROS/Development

### Pc-i386

1. Select toolchain-alt-abiv0-i386 (if not built yet)
2. Select alt-abiv0-pc-i386

ISO image available in
    alt-abiv0-pc-i386/distfiles

### Additional information

* To control the number of parallel make jobs, the environment variable MAKE_JOBS can be given.
For example, ```MAKE_JOBS=4 ./rebuild.sh``` means four parallel make jobs can be used.
By default, three parallel make jobs are allowed.

* Additional options can be passed to the configure script with the environment variable EXTRA_CONFIGURE_OPTS.
