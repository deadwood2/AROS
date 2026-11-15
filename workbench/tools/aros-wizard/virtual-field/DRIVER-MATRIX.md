# AROS virtual driver test matrix

**Current status: configuration simulation only.** The test harness does not emulate an AROS guest, exercise driver interrupts, or prove hardware compatibility.

| Device class | QEMU model/profile | Current check | Guest test required |
| --- | --- | --- | --- |
| Network | pcnet | argv generation | pcnet32.device DHCP, TCP, DNS, throughput |
| Network | e1000 | argv generation | e1000.device fixed revision, TX interrupt/free lifecycle |
| Storage | IDE qcow2 | argv generation | partition detection, read/write integrity |
| Optical | ISO CD-ROM | argv generation | boot, media read |
| Display | gtk / sdl / none | argv generation | guest graphics driver, modes, redraw |
| CPU | 1 / 4 vCPU | argv generation | SMP scheduling, interrupts |
| RAM | 512 MiB / 8 GiB | argv generation | boot, allocator stress |
| USB | not yet modeled | not tested | controller, keyboard/mouse/storage |
| Audio | not yet modeled | not tested | sound output and timing |
| Input | implicit default | not tested | keyboard/mouse event behavior |

Run offline configuration tests:
```sh
python3 -m unittest discover -s workbench/tools/aros-wizard/virtual-field -p 'test_*.py' -v
python3 workbench/tools/aros-wizard/virtual-field/hardware_matrix.py
```

For actual driver debugging, record AROS release, exact driver revision, QEMU version, host OS, guest logs, a reproducible failure and whether the driver runs in interrupt or task context. Never mark simulated configuration tests as guest driver passes.
