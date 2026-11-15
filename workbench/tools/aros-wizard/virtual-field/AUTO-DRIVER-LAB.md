# AROS Auto Driver Lab — prototype

The first module `auto_driver.py` accepts PCI inventory JSON and classifies devices by PCI vendor/device IDs. It recognizes QEMU E1000 (8086:100e) and PCnet (1022:2000) as **candidate** AROS drivers, not as verified working drivers. Unknown devices get a structured research and development plan.

Example inventory:
```json
[
  {"vendor_id":"8086","device_id":"100e","class_code":"0200"},
  {"vendor_id":"1234","device_id":"5678","description":"Unknown prototype"}
]
```

Usage:
```sh
python3 auto_driver.py inventory.json --output driver-plan.json
python3 -m unittest discover -s . -p 'test_*.py' -v
```

**Not implemented:** live PCI enumeration inside AROS, LLM connection, automatic driver source synthesis, build, deployment, hardware probing or kernel execution. These require dedicated adapters and a sandboxed build/test pipeline. Do not deploy generated driver patches without human review and reproducible guest/hardware tests.

Proposed pipeline: inventory -> driver database lookup -> datasheet/license retrieval -> AI-assisted code review and source patch -> cross-compile -> static analysis -> QEMU tests -> manual hardware validation -> approved release.
