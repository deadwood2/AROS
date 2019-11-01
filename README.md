## Core

branch | target | description | how to build | download
-------|--------|-------------|------------- |---------
master | amiga-m68k | Amiga replacement ROM and system software | [Core](https://github.com/deadw00d/AROS/blob/master/INSTALL.md) | [Core](https://vps691225.ovh.net/download/builds/AROS/)
master | linux-x86_64 | Stable and always backwards compatible hosted version | [Core](https://github.com/deadw00d/AROS/blob/master/INSTALL.md) | [Core](https://vps691225.ovh.net/download/builds/AROS/)

### Backwards compatibility

component | kept stable
----------|----------------------
Application Binary Interface (ABI) | YES
OS 3.1 API (examples: exec.library, input.device) | YES
3rd party public libraries API (example: muimaster.library) | YES
Classes, gadgets, datatypes API (examples: png.dataype) | YES
AROS driver system (HIDD, oop.library) | NO
AROS kernel components (example: kernel.resource) | NO

## Alternatives

branch | target | description | how to build | download
-------|--------|-------------|--------------|---------
alt-runtime | runtimelinux-x86_64 | AxRuntime for Linux x86_64 | [AxRuntime](https://github.com/deadw00d/AROS/blob/alt-runtime/INSTALL.md) | [AxRuntime](https://vps691225.ovh.net/download/builds/AxRuntime/)
alt-abiv0 | pc-i386 | ABI_V0 version of native 32-bit AROS | [ABIv0](https://github.com/deadw00d/AROS/blob/alt-abiv0/INSTALL.md) | [ABIv0](https://vps691225.ovh.net/download/builds/AROS-ABIV0/)

## Relation between Core and Alternatives

The separation between Core and Alternatives has been introduced to allow different, sometimes diverging views and usages of AROS to co-exist and contribute to common base.

Core is defined as the base for all projects. Core defines a few targets which are preserved at each commit.

Alternatives can use two mechanisms to implement their changes:
* **arch mechanism** of AROS build system which allows overwriting implementation on file basis and keeping the overwrites in master branch
* **periodically rebased git branches mechanism** for changes that modify the base files and would break the Core targets

Every Alternative needs to have at minimum a branch starting with alt- even if all changes are done via arch mechanism. On that branch README.md file should be modified to describe the Alternative. Checking out this branch should allow anyone to build a working version of the Alternative.

### Responsibilities

Every commit made to Core targets is required to preserve backwards compatibility. Maintainer of the Core targets reserves the right to revert a commit or ask for it to be moved to an Alternative branch.

Maintainers of Alternatives are responsible for adjusting their projects to changes happening in Core.

Maintainer of Core is responsible to keeping the number of wide-spread changes controlled, possibly batching them when needed and communicating to maintainers of Alternatives in advanced.

