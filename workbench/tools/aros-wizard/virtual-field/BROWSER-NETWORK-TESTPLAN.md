# AROS ABIv11 Odyssey + QEMU network validation

Status: **test plan only; guest boot and browser tests NOT executed**.

## Baseline
- AROS ABIv11 2026.09 x86-64 ISO
- Odyssey 2.1 x86-64 ABIv11 browser package
- QEMU user-mode networking with `-nic user,model=e1000`
- Verify the AROS `e1000.device` stability fix reported 2026-10-01 is included in the selected image. If not, update before stress testing.

## Host preflight
1. Confirm `qemu-system-x86_64 --version` succeeds.
2. Verify the ISO exists and is readable.
3. Generate a field JSON using `field_generator.py` with `network: true`.
4. Review QEMU argv; confirm `-nic user,model=e1000` and no `hostfwd` rules.
5. Boot AROS in QEMU (manual step; do not assume success).

## Guest network tests
1. Inspect whether AROS detects the emulated E1000 NIC and binds `e1000.device`.
2. Check interface configuration and DHCP lease. QEMU user-mode networking commonly uses guest gateway `10.0.2.2` and DNS `10.0.2.3`, but verify actual settings.
3. Test gateway reachability if ICMP is supported; lack of ICMP response alone is not a network failure.
4. Test DNS resolution independently from TCP connectivity.
5. Test HTTP then HTTPS (TLS trust store, certificate validation, time/date).
6. Reboot and repeat to detect initialization and driver stability issues.

## Browser smoke tests
1. Launch Odyssey 2.1; note crash or startup errors.
2. Open a local HTML file (offline rendering baseline).
3. Load an HTTP page; check redirects, images, basic CSS.
4. Load a valid HTTPS page; check TLS errors and CA bundle.
5. Test JavaScript basics and browser responsiveness; do not assume modern web compatibility.
6. Run a 15-minute browsing session, watch memory and network stability.

## Result recording
Record image version, driver version, QEMU version, Odyssey version, guest IP, DNS, each pass/fail, crash logs and screenshots. Report unexecuted checks as NOT RUN rather than PASS.

References:
- https://github.com/deadwood2/AROS/releases/tag/ABIv11_2026.09
- https://github.com/deadwood2/OdysseyWebBrowser
- https://www.arosworld.org/infusions/forum/viewthread.php?thread_id=2097
