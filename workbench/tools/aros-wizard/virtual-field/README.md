# QEMU Virtual Field Generator

A first, offline-capable virtual environment generator for AROS Wizard.

The generator accepts a JSON specification (which can be authored by a human or a separate AI model), validates it, and emits a QEMU argument array. **It does not run commands, access a model, or modify disk images.** Review output before executing.

Example:
```json
{"name":"AROS Macintosh-WB13","memory_mb":2048,"cpus":2,"iso":"AROS.iso","network":false,"display":"gtk"}
```

Usage:
```sh
python3 workbench/tools/aros-wizard/virtual-field/field_generator.py spec.json
python3 -m unittest discover -s workbench/tools/aros-wizard/virtual-field -p 'test_*.py'
```

The AI integration is a future interface: an LLM can generate only the allowlisted JSON keys, which are validated before use. No API credentials or unreviewed model output are passed to a shell.

Current limitations: no graphical canvas, no model connector, no QEMU execution, no automated installation, no Windows GUI integration, and no AROS-native build.
