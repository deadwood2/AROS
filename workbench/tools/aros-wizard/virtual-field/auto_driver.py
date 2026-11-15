#!/usr/bin/env python3
"""AROS unknown-hardware diagnostic and driver-development planner.

Consumes PCI inventory JSON. Generates reviewable driver plans and stub metadata,
never executable kernel code or an automatic driver installation.
"""
import argparse
import json
import re
from pathlib import Path

HEX_ID = re.compile(r"^(?:0x)?[0-9a-fA-F]{4}$")
KNOWN = {
    ("8086", "100e"): ("e1000.device", "network", "Verify fixed interrupt-safe TX buffer handling before use"),
    ("1022", "2000"): ("pcnet32.device", "network", "Preferred QEMU fallback"),
}

def normalize_id(value):
    if not isinstance(value, str) or not HEX_ID.fullmatch(value):
        raise ValueError("PCI identifiers must be exactly four hex digits")
    return value.lower().removeprefix("0x")

def diagnose(inventory):
    if not isinstance(inventory, list) or len(inventory) > 256:
        raise ValueError("Inventory must be a list with at most 256 devices")
    results = []
    for index, item in enumerate(inventory):
        if not isinstance(item, dict) or set(item) - {"vendor_id", "device_id", "class_code", "description"}:
            raise ValueError("Invalid inventory entry")
        vendor = normalize_id(item.get("vendor_id"))
        device = normalize_id(item.get("device_id"))
        class_code = item.get("class_code", "")
        description = item.get("description", "")
        if not isinstance(class_code, str) or len(class_code) > 16 or not isinstance(description, str) or len(description) > 160:
            raise ValueError("Invalid device metadata")
        match = KNOWN.get((vendor, device))
        results.append({
            "index": index,
            "pci_id": vendor + ":" + device,
            "class_code": class_code,
            "description": description,
            "status": "CANDIDATE_DRIVER" if match else "UNKNOWN_HARDWARE",
            "candidate_driver": match[0] if match else None,
            "category": match[1] if match else "unclassified",
            "advice": match[2] if match else "Inspect PCI BARs, IRQ mode, DMA capabilities, datasheet and licensing",
            "development": {
                "stage": "review_required",
                "tasks": [
                    "Obtain authoritative device datasheet and hardware register map",
                    "Identify compatible AROS driver model and ABI",
                    "Design IRQ and DMA lifetime management with bounds checks",
                    "Create a reviewed driver source patch in a separate branch",
                    "Compile and run static checks, then QEMU/device simulation",
                    "Test on real hardware only with explicit operator approval",
                ],
                "automatic_install": False,
                "automatic_kernel_code_execution": False,
            },
        })
    return {"schema": 1, "devices": results, "driver_code_generated": False, "ai_model_connected": False}

def main():
    p = argparse.ArgumentParser()
    p.add_argument("inventory", type=Path, help="PCI inventory JSON list")
    p.add_argument("--output", type=Path)
    ns = p.parse_args()
    report = json.dumps(diagnose(json.loads(ns.inventory.read_text(encoding="utf-8"))), indent=2, ensure_ascii=False) + "\n"
    if ns.output:
        ns.output.write_text(report, encoding="utf-8")
    else:
        print(report, end="")

if __name__ == "__main__":
    main()
