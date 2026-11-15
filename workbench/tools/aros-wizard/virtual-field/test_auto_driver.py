import unittest
from auto_driver import diagnose

class AutoDriverTests(unittest.TestCase):
    def test_known_e1000(self):
        d = diagnose([{"vendor_id":"8086","device_id":"100e"}])["devices"][0]
        self.assertEqual(d["candidate_driver"], "e1000.device")
        self.assertFalse(d["development"]["automatic_install"])

    def test_unknown_hardware(self):
        report = diagnose([{"vendor_id":"1234","device_id":"5678","description":"prototype"}])
        d = report["devices"][0]
        self.assertEqual(d["status"], "UNKNOWN_HARDWARE")
        self.assertIsNone(d["candidate_driver"])
        self.assertFalse(report["driver_code_generated"])
        self.assertFalse(report["ai_model_connected"])

    def test_invalid_pci_id(self):
        for value in ("12345", "nothex", "0x1", None):
            with self.subTest(value=value):
                with self.assertRaises(ValueError):
                    diagnose([{"vendor_id":value,"device_id":"100e"}])

    def test_reject_excessive_inventory(self):
        with self.assertRaises(ValueError):
            diagnose([{"vendor_id":"1234","device_id":"5678"}] * 257)

if __name__ == "__main__":
    unittest.main()
