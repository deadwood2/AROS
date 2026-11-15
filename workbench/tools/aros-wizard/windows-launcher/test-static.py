from pathlib import Path
import re

root = Path(__file__).resolve().parent
script = (root / 'AROS.ps1').read_text(encoding='utf-8-sig')
bootstrap = (root / 'ArosSetup.cs').read_text(encoding='utf-8-sig')
build = (root / 'build.ps1').read_text(encoding='utf-8-sig')
forms = len(re.findall(r'\$form\s*=\s*New-Object Windows\.Forms\.Form', script))
styled = len(re.findall(r'Set-MacintoshWizardTheme\s+\$form', script))
assert forms == 3, f'Expected 3 dialogs, found {forms}'
assert styled == forms, f'{styled} themed dialogs of {forms}'
assert "[System.Drawing.Font]::new('Segoe UI', 9)" in script, 'Font constructor missing'
assert "New-Object System.Drawing.Font('Segoe UI', 9)" not in script, 'Invalid New-Object constructor regression'
for token in ['#E8E8E8', '#F28B57', '#FFFFFF', '#111111']:
    assert token in script, f'Missing color: {token}'
for token in ['-Smp', '-VirtualDrives', '-ChooseDistribution']:
    assert token in bootstrap, f'Missing option: {token}'
assert 'GetManifestResourceStream("AROS.ps1")' in bootstrap
assert '/resource:$script,AROS.ps1' in build
print(f'PASS: {forms} dialogs styled, palette, CLI switches and resource build checked')
