## Required packages

Please install these packages before moving to next step. Below is a reference list for Debian-based distributions. Reference build system is Ubuntu 22.04/24.04 amd64.

    git gcc g++ make gawk bison flex bzip2 netpbm autoconf automake libx11-dev libxext-dev libc6-dev liblzo2-dev libxxf86vm-dev libpng-dev gcc-multilib libsdl1.2-dev byacc python3-mako libxcursor-dev cmake genisoimage dh-make yasm curl

## Clone & build

    $ cd <myprojects>
    $ mkdir arosbuilds
    $ cd arosbuilds
    $ git clone https://github.com/deadwood2/AROS.git AROS
    $ cp ./AROS/scripts/rebuild.sh .

Proceed to build selection below

### Linux-x86_64 (AROS that is a "program" running from Linux)

First, build the cross-compiler by running

    $ ./rebuild.sh

and selecting option ```1)  toolchain-core-x86_64```. Once the cross-compiler build is complete run ```$ ./rebuild.sh``` again and select option ```2)  core-linux-x86_64 (DEBUG)``` to build AROS.

Start AROS by:

    $ cd core-linux-x86_64-d/bin/linux-x86_64/AROS
    $ ./boot/linux/AROSBootstrap

In order to use the cross-compiler for compiling your own projects (which don't use AROS builds system / mmakefile.src approach), you need to provide --sysroot parameter, for example:

    $ <myprojects>/arosbuilds/toolchain-core-x86_64/x86_64-aros-gcc --sysroot=<myprojects>/arosbuilds/core-linux-x86_64-d/bin/linux-x86_64/AROS/Development

### Amiga-m68k (AROS that can be used in Amiga emulator, like WinUAE)

First, build the cross-compiler by running

    $ ./rebuild.sh

and selecting option ```11) toolchain-core-m68k```.
Once the cross-compiler build is complete run ```$ ./rebuild.sh``` again and type the number for the Amiga build you want.
There are two variants: ```core-amiga-m68k``` and ```core-amiga-m68k-classic``` with or without extra debug logging.
The first includes the USB stack and has the same Workbench theming as the PC versions.
It works best on a JIT-enabled emulator setup with RTG graphics, and should not be the first choice for a weaker system.
The second lacks the USB stack and looks more like a classic 3.1 system.
It does not need RTG graphics, but still benefits from a fast CPU.

Kickstart images available in

    core-amiga-m68k/bin/amiga-m68k/AROS/boot/amiga

or

    core-amiga-m68k-classic/bin/amiga-m68k-classic/AROS/boot/amiga-classic

### Additional information

* For armhf build, please install additional packages:

        gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf

* To control the number of parallel make jobs, the environment variable MAKE_JOBS can be given.
For example, ```MAKE_JOBS=4 ./rebuild.sh``` means four parallel make jobs can be used.
By default, three parallel make jobs are allowed.

* Additional options can be passed to the configure script with the environment variable EXTRA_CONFIGURE_OPTS.
