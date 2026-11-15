"""Offline QEMU network configuration self-test; no guest boot or Internet claim."""
import unittest
from field_generator import build_field

class NetworkTests(unittest.TestCase):
    def test_default_nat(self):
        args = build_field({})["qemu_argv"]
        self.assertEqual(args[args.index("-nic") + 1], "user,model=e1000")

    def test_explicit_nat(self):
        args = build_field({"network": True})["qemu_argv"]
        self.assertEqual(args[args.index("-nic") + 1], "user,model=e1000")

    def test_pcnet_fallback(self):
        args = build_field({"network_adapter": "pcnet"})["qemu_argv"]
        self.assertEqual(args[args.index("-nic") + 1], "user,model=pcnet")

    def test_invalid_adapter(self):
        with self.assertRaises(ValueError):
            build_field({"network_adapter": "virtio"})

    def test_network_disabled(self):
        args = build_field({"network": False})["qemu_argv"]
        self.assertEqual(args[args.index("-nic") + 1], "none")

    def test_no_inbound_host_forwarding(self):
        args = build_field({"network": True})["qemu_argv"]
        self.assertFalse(any("hostfwd" in arg for arg in args))

    def test_invalid_network_type(self):
        for value in ("true", 1, None, [], {}):
            with self.subTest(value=value):
                with self.assertRaises(ValueError):
                    build_field({"network": value})

if __name__ == "__main__":
    unittest.main()
