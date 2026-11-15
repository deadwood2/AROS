#!/usr/bin/env python3
"""AROS/QEMU host preflight and driver test report; does not boot guest."""
import argparse
import json
import shutil
import subprocess
from datetime import datetime, timezone
from pathlib import Path

from field_generator import build_field

DRIVERS = {
    "network_pcnet": "pcnet32.device: link, DHCP, DNS, HTTP, HTTPS",
    "network_e1000": "e1000.device: fixed revision, TX/RX, interrupt safety",
    "storage_ide": "IDE: disk discovery, partitioning, read/write checksum",
    "optical": "CD-ROM: boot and media reads",
    "graphics": "Wanderer: modes, redraw, resize, window resize integration",
    "input": "keyboard and pointer input",
    "usb": "USB controller, hotplug and mass storage",
    "audio": "audio playback and timing",
    "smp": "multicore boot, interrupts, scheduler stress",
    "browser": "Odyssey: HTTP, TLS certificates, DNS and stability",
}

def preflight(spec, iso=None):
    field = build_field(spec)
    binary = shutil.which("qemu-system-x86_64")
    checks = {"qemu_binary": {"status": "PASS" if binary else "BLOCKED", "detail": binary or "not found in PATH"}}
    if binary:
        try:
            completed = subprocess.run([binary, "--version"], capture_output=True, text=True, timeout=10, check=False)
            checks["qemu_version"] = {"status": "PASS" if completed.returncode == 0 else "FAIL", "detail": (completed.stdout or completed.stderr).strip()[:300]}
        except (OSError, subprocess.TimeoutExpired) as exc:
            checks["qemu_version"] = {"status": "FAIL", "detail": str(exc)}
    if iso:
        path = Path(iso)
        checks["aros_iso"] = {"status": "PASS" if path.is_file() else "BLOCKED", "detail": str(path)}
    else:
        checks["aros_iso"] = {"status": "NOT_RUN", "detail": "No ISO provided"}
    checks["network_adapter"] = {"status": "PASS", "detail": field["qemu_argv"][field["qemu_argv"].index("-nic") + 1]}
    return {
        "schema": 1,
        "timestamp_utc": datetime.now(timezone.utc).isoformat(),
        "field": field,
        "host_checks": checks,
        "guest_driver_checks": {key: {"status": "NOT_RUN", "procedure": procedure} for key, procedure in DRIVERS.items()},
        "note": "PASS on host checks does not indicate working AROS guest drivers",
    }

def main():
    p = argparse.ArgumentParser()
    p.add_argument("--spec", type=Path, help="JSON virtual-field configuration")
    p.add_argument("--iso", type=Path)
    p.add_argument("--output", type=Path)
    ns = p.parse_args()
    spec = json.loads(ns.spec.read_text(encoding="utf-8")) if ns.spec else {}
    report = json.dumps(preflight(spec, ns.iso), indent=2, ensure_ascii=False) + "\n"
    if ns.output:
        ns.output.write_text(report, encoding="utf-8")
    else:
        print(report, end="")

if __name__ == "__main__":
    main()
