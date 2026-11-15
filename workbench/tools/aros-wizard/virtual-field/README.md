# QEMU Virtual Field Generator

A first, offline-capable virtual environment generator for AROS Wizard.

The generator accepts a JSON specification (which can be authored by a human or a separate AI model), validates it, and emits a QEMU argument array. **It does not run commands, access a model, or modify disk images.** Review output before executing.

Example:
```json
{"name":"AROS Macintosh-WB13","memory_mb":2048,"cpus":2,"iso":"AROS.iso","network":true,"display":"gtk"}
```

Usage:
```sh
python3 workbench/tools/aros-wizard/virtual-field/field_generator.py spec.json
python3 -m unittest discover -s workbench/tools/aros-wizard/virtual-field -p 'test_*.py'
```

The AI integration is a future interface: an LLM can generate only the allowlisted JSON keys, which are validated before use. No API credentials or unreviewed model output are passed to a shell.

Current limitations: no graphical canvas, no model connector, no QEMU execution, no automated installation, no Windows GUI integration, and no AROS-native build.

Networking defaults to QEMU user-mode NAT with an emulated e1000 adapter. It does not expose incoming ports by default. Set `"network": false` to disable networking. Guest network drivers and connectivity still need runtime verification.

## Driver safety (2026-11)
The default virtual NIC is **pcnet** (`-nic user,model=pcnet`) to avoid the known AROS e1000 interrupt-context memory-free crash on older guest images. E1000 remains opt-in with `"network_adapter": "e1000"` only after verifying the guest driver includes the upstream fix. This change does not patch or rebuild the AROS kernel driver.

Root-cause evidence: https://github.com/kaffeine1/telegram-amiga/blob/main/ROADMAP.md (e1000 interrupt FreeMem analysis).
