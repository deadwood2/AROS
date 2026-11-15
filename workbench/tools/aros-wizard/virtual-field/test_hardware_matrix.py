import unittest
from hardware_matrix import PROFILES, validate_profiles

class HardwareMatrixTests(unittest.TestCase):
    def test_all_profiles_generate(self):
        self.assertEqual(set(validate_profiles()), set(PROFILES))

    def test_network_profiles(self):
        results = validate_profiles()
        self.assertIn("user,model=pcnet", results["network_pcnet"])
        self.assertIn("user,model=e1000", results["network_e1000"])
        self.assertEqual(results["network_offline"][-1], "none")

    def test_disk_and_cdrom(self):
        results = validate_profiles()
        self.assertIn("-drive", results["ide_disk"])
        self.assertIn("-cdrom", results["cdrom"])

if __name__ == "__main__":
    unittest.main()
