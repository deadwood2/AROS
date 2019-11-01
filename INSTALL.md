## Required packages

Please install these packages before moving to next step. Below is a reference list for Debian-based distributions.

    subversion git-core gcc g++ make gawk bison flex bzip2 netpbm autoconf    automake libx11-dev libxext-dev libc6-dev liblzo2-dev libxxf86vm-dev libpng-dev gcc-multilib libsdl1.2-dev byacc python-mako libxcursor-dev

## Clone & build

    $ mkdir myrepo
    $ cd myrepo
    $ git clone https://github.com/deadw00d/AROS.git AROS
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


### Pc-i386

1. Select toolchain-alt-abiv0-i386 (if not built yet)
2. Select alt-abiv0-pc-i386

ISO image available in

    alt-abiv0-pc-i386/distfiles
