# Draft Pull Request: AROS Wizard Macintosh-WB13

**Upstream:** https://github.com/deadwood2/AROS  
**Source fork:** https://github.com/aurabin/AROS  
**Head:** `aurabin:feature/aros-wizard-macintosh-wb13`  
**Base:** `deadwood2:master`  
**Status:** Not opened. GitHub connector returned HTTP 403 when creating the PR.

## Purpose

Move the AROS-specific Windows/QEMU launcher into the AROS repository rather than maintaining it as part of BOINC. Provide an initial Macintosh-inspired Windows Forms visual style with an Amiga Workbench 1.3-inspired palette.

## Changes

- `workbench/tools/aros-wizard/windows-launcher/AROS.ps1`: imported launcher; styles three Windows Forms dialogs.
- `workbench/tools/aros-wizard/windows-launcher/ArosSetup.cs`: imported C# launcher bootstrap.
- `workbench/tools/aros-wizard/windows-launcher/AROS-starten.cmd`: imported Windows start script.
- `workbench/tools/aros-wizard/themes/Macintosh-WB13/design.json`: initial color and window design specification.
- `README.md` and `INTEGRATION-STATUS.md`: scope, limitations and progress.

## Visual design

- Desktop blue: `#315C9B`
- Title bar orange: `#F28B57`
- Window background: `#E8E8E8`
- Highlight white: `#FFFFFF`
- Text and border: `#111111`

The design is inspired by classic desktop interfaces. No proprietary Macintosh assets are included.

## Validation

**Not verified:** Windows executable build, PowerShell runtime, QEMU boot, AROS x86-64 build, native AROS theme installation, and regression testing.

The imported launcher may require additional files and dependency cleanup. The design JSON is a specification, not a functioning AROS Appearance theme.

## Reviewer checklist

- [ ] Confirm project placement and licensing
- [ ] Audit imported Windows launcher dependencies
- [ ] Validate PowerShell syntax and Windows GUI runtime
- [ ] Verify distribution selection, ISO selection, virtual drives and SMP
- [ ] Implement and validate native AROS Appearance theme if in scope
- [ ] Build/test on supported platforms
- [ ] Confirm upstream CI passes before merging

## Open a draft PR

https://github.com/deadwood2/AROS/compare/master...aurabin:AROS:feature/aros-wizard-macintosh-wb13?expand=1

Suggested title: **Draft: Add standalone AROS Wizard Windows launcher with Macintosh-WB13 styling**

Do not merge until review and validation are complete.
