import unittest
from field_generator import build_field

class FieldTests(unittest.TestCase):
    def test_default(self):
        result = build_field({})
        self.assertEqual(result["execution"], "manual-review-required")
        self.assertIn("-nic", result["qemu_argv"])
        self.assertEqual(result["qemu_argv"][-1], "user,model=pcnet")

    def test_network_disabled(self):
        result = build_field({"network": False})
        self.assertEqual(result["qemu_argv"][-1], "none")

    def test_custom(self):
        result = build_field({"name":"Workbench","memory_mb":4096,"cpus":4,"network":True,"iso":"AROS.iso"})
        self.assertIn("4096", result["qemu_argv"])
        self.assertIn("AROS.iso", result["qemu_argv"])

    def test_invalid(self):
        for spec in ({"cpus":0}, {"memory_mb":True}, {"network":"yes"}, {"command":"rm -rf /"}, {"display":"bad"}):
            with self.subTest(spec=spec):
                with self.assertRaises(ValueError):
                    build_field(spec)

if __name__ == "__main__":
    unittest.main()
