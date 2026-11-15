"""Offline hardware matrix for AROS QEMU driver smoke-test planning.

This validates QEMU argument profiles, not native guest drivers.
"""
from field_generator import build_field

PROFILES = {
    "network_pcnet": {"network_adapter": "pcnet"},
    "network_e1000": {"network_adapter": "e1000"},
    "network_offline": {"network": False},
    "display_gtk": {"display": "gtk"},
    "display_sdl": {"display": "sdl"},
    "headless": {"display": "none"},
    "single_cpu": {"cpus": 1},
    "smp_4": {"cpus": 4},
    "low_memory": {"memory_mb": 512},
    "large_memory": {"memory_mb": 8192},
    "ide_disk": {"disk": "aros.qcow2"},
    "cdrom": {"iso": "aros.iso"},
}

def validate_profiles():
    result = {}
    for name, spec in PROFILES.items():
        field = build_field(spec)
        argv = field["qemu_argv"]
        assert argv[0] == "qemu-system-x86_64"
        assert argv.count("-nic") == 1
        assert argv.count("-display") == 1
        assert field["execution"] == "manual-review-required"
        result[name] = argv
    return result

if __name__ == "__main__":
    for name, argv in validate_profiles().items():
        print("CONFIG PASS", name, repr(argv))
    print("NOTE: no QEMU boot, guest driver, USB, audio, or graphics rendering tests were executed")
