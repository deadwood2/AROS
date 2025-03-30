## Core

branch | target | description | how to build | download
-------|--------|-------------|------------- |---------
[master](https://github.com/deadwood2/AROS/tree/master) | linux-x86_64<br/>pc-x86_64 | Stable and always backwards compatible hosted version, code-named ABIv11 | [Core](https://github.com/deadwood2/AROS/blob/master/INSTALL.md) | [ABIv11](https://axrt.org/downloads-aros-64bit)
[master](https://github.com/deadwood2/AROS/tree/master) | amiga-m68k | Amiga replacement ROM and system software | [Core](https://github.com/deadwood2/AROS/blob/master/INSTALL.md) | [M68K](https://build.axrt.org/download/builds/AROS-M68K/)

### Backwards compatibility

From system developer's point of view backwards compatibility is defined on a set of components below.

component | kept stable
----------|----------------------
Application Binary Interface (ABI) | YES
OS 3.1 API (examples: exec.library, input.device) | YES
3rd party public libraries API (example: muimaster.library) | YES
Classes, gadgets, datatypes API (examples: png.dataype) | YES
AROS driver system (HIDD, oop.library) | NO
AROS kernel components (example: kernel.resource) | NO

From application developer's point of view backwards compatibility is defined as follow: As long as your application only uses components marked as YES, maintainter of Core guarantees that your application will always run while the system will continue evolving and changing its components. In case you notice that compatibility has been broken, please contact the maintainer and the situation will be amended.

## Alternatives

branch | target | description | how to build | download
-------|--------|-------------|--------------|---------
[alt-runtime](https://github.com/deadwood2/AROS/tree/alt-runtime) | runtimelinux-x86_64 | AxRuntime for Linux x86_64 | [AxRuntime](https://github.com/deadwood2/AROS/blob/alt-runtime/INSTALL.md) | [AxRuntime](https://axrt.org/downloads)
[alt-abiv0](https://github.com/deadwood2/AROS/tree/alt-abiv0) | pc-i386 | ABI_V0 version of native 32-bit AROS | [ABIv0](https://github.com/deadwood2/AROS/blob/alt-abiv0/INSTALL.md) | [ABIv0](https://axrt.org/downloads-aros)

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

