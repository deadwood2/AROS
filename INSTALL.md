## Required packages

Reference packages for Debian

    subversion git-core gcc g++ make gawk bison flex bzip2 netpbm autoconf    automake libx11-dev libxext-dev libc6-dev liblzo2-dev libxxf86vm-dev libpng-dev gcc-multilib libsdl1.2-dev byacc python-mako libxcursor-dev

## Clone & build

    $ mkdir myrepo
    $ cd myrepo
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


### Amiga-m68k

1. Select toolchain-core-m68k
2. Select core-amiga-m68k

Kickstart images available in

    core-amiga-m68k/bin/amiga-m68k/AROS/boot/amiga
