import unittest
from unittest.mock import patch
from driver_diagnostics import DRIVERS, preflight

class DiagnosticsTests(unittest.TestCase):
    @patch("driver_diagnostics.shutil.which", return_value=None)
    def test_missing_qemu_is_blocked(self, _which):
        report = preflight({})
        self.assertEqual(report["host_checks"]["qemu_binary"]["status"], "BLOCKED")
        self.assertEqual(report["host_checks"]["aros_iso"]["status"], "NOT_RUN")
        self.assertTrue(all(x["status"] == "NOT_RUN" for x in report["guest_driver_checks"].values()))

    @patch("driver_diagnostics.shutil.which", return_value=None)
    def test_network_profile_and_matrix(self, _which):
        report = preflight({"network_adapter": "e1000"})
        self.assertEqual(report["host_checks"]["network_adapter"]["detail"], "user,model=e1000")
        self.assertEqual(set(report["guest_driver_checks"]), set(DRIVERS))

if __name__ == "__main__":
    unittest.main()
