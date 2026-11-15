# Integration status

- [x] AROS fork development branch created
- [x] Macintosh-WB13 palette specification added
- [x] Windows launcher PowerShell, C# bootstrapper and CMD script imported
- [x] Macintosh-WB13 styling hooked into three Windows Forms dialogs
- [x] Windows build script added (Framework64 C# compiler + embedded PowerShell resource)
- [x] Python static checks and GitHub Actions Windows validation workflow added
- [ ] Confirm GitHub Actions run passes (not yet observed)
- [ ] Run Windows GUI and QEMU boot smoke tests
- [ ] Audit imported launcher dependencies and asset paths
- [ ] Implement native AROS theme and Appearance integration
- [ ] Build and test on AROS x86-64
- [ ] Open upstream pull request after validation

This is Windows-hosted QEMU launcher source, not an AROS-native executable. Build and runtime success must not be inferred from adding CI files.
