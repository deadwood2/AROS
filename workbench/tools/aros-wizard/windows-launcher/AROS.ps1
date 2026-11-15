param(
    [switch]$SetupOnly,
    [switch]$Update,
    [switch]$AutoUpdate,
    [switch]$UpdateSettings,
    [switch]$CleanupOnly,
    [switch]$ShareWindowsFiles,
    [switch]$WritableShare,
    [switch]$WindowsDesktop,
    [ValidateRange(1, 65535)][int]$DesktopPort = 5900,
    [switch]$OfflineIso,
    [switch]$ChooseDistribution,
    [ValidateSet('AROSOne32','AROSOne64','Icaros','AspireOS')][string]$Distribution,
    [string]$IsoPath,
    [switch]$ChatGPT,
    [switch]$WandererFullscreen,
    [switch]$VirtualDrives,
    [switch]$Smp,
    [ValidateRange(1, 255)][int]$SmpCpus = 1,
    [ValidateRange(1, 255)][int]$SmpSockets = 1,
    [ValidateRange(1, 255)][int]$SmpCores = 1,
    [ValidateRange(1, 255)][int]$SmpThreads = 1,
    [ValidatePattern('^[A-Za-z0-9_.-]+$')][string]$SmpCpuModel,
    [switch]$DriverDebug
)

$ErrorActionPreference = 'Stop'
# Macintosh-WB13 theme: Windows host dialogs, independent of BOINC.
function Set-MacintoshWizardTheme([System.Windows.Forms.Form]$Form) {
    $Form.BackColor = [System.Drawing.ColorTranslator]::FromHtml('#E8E8E8')
    $Form.ForeColor = [System.Drawing.ColorTranslator]::FromHtml('#111111')
    $Form.Font = [System.Drawing.Font]::new('Segoe UI', 9)
    foreach ($control in $Form.Controls) {
        if ($control -is [System.Windows.Forms.Button]) {
            $control.FlatStyle = [System.Windows.Forms.FlatStyle]::Flat
            $control.FlatAppearance.BorderColor = [System.Drawing.ColorTranslator]::FromHtml('#111111')
            $control.FlatAppearance.BorderSize = 1
            if ($control.DialogResult -eq [System.Windows.Forms.DialogResult]::OK) {
                $control.BackColor = [System.Drawing.ColorTranslator]::FromHtml('#F28B57')
            } else {
                $control.BackColor = [System.Drawing.ColorTranslator]::FromHtml('#FFFFFF')
            }
        } elseif ($control -is [System.Windows.Forms.Label]) {
            $control.ForeColor = [System.Drawing.ColorTranslator]::FromHtml('#111111')
        } elseif ($control -is [System.Windows.Forms.ListBox] -or $control -is [System.Windows.Forms.TextBox]) {
            $control.BackColor = [System.Drawing.ColorTranslator]::FromHtml('#FFFFFF')
        }
    }
}

$root = $PSScriptRoot
if ($env:AROS_SETUP_DIRECTORY) { $root = $env:AROS_SETUP_DIRECTORY }
$data = Join-Path $root 'AROS-Daten'
$baseUrl = 'https://www.axrt.org/download/aros/v11/'
$baseline = '202609'
$releasePattern = '[0-9]{6}(?:[0-9]{2})?(?:-[0-9]+)?'

function Get-ReleaseOrder([string[]]$Releases) {
    $Releases | Sort-Object -Unique -Descending -Property @(
        { ($_.Split('-')[0]).PadRight(8, '0') },
        { if ($_.Contains('-')) { [long]$_.Split('-')[1] } else { 0 } }
    )
}

function Find-Qemu {
    $command = Get-Command qemu-system-x86_64.exe -ErrorAction SilentlyContinue
    if ($command) { return $command.Source }
    foreach ($folder in @($env:ProgramFiles, $env:LOCALAPPDATA, $root)) {
        if ($folder) {
            $candidate = Join-Path $folder 'qemu\qemu-system-x86_64.exe'
            if (Test-Path -LiteralPath $candidate -PathType Leaf) { return $candidate }
        }
    }
}

function Get-OrInstallQemu {
    $qemu = Find-Qemu
    if (!$qemu) {
        if ($useOfflineIso) { throw 'Fuer den Offline-Start muss QEMU bereits installiert sein.' }
        Write-Host 'QEMU fehlt. Windows Paket-Manager startet die Installation.'
        if (!(Get-Command winget.exe -ErrorAction SilentlyContinue)) {
            throw 'WinGet fehlt. Installiere QEMU von https://www.qemu.org/download/ und starte erneut.'
        }
        & winget.exe install --exact --id SoftwareFreedomConservancy.QEMU --source winget | Out-Host
        $qemu = Find-Qemu
        if (!$qemu) { throw 'QEMU wurde nicht gefunden. Schliesse dieses Fenster und starte erneut.' }
    }
    return $qemu
}

function Assert-IsoImage([string]$Path) {
    $null = Get-IsoVolumeLabel $Path
}

function Get-IsoVolumeLabel([string]$Path) {
    $file = [IO.File]::OpenRead($Path)
    try {
        $descriptor = New-Object byte[] 2048
        for ($sector = 16; $sector -lt 48; $sector++) {
            $file.Position = $sector * 2048
            if ($file.Read($descriptor, 0, $descriptor.Length) -ne $descriptor.Length) { break }
            if ([Text.Encoding]::ASCII.GetString($descriptor, 1, 5) -ne 'CD001') { break }
            if ($descriptor[6] -ne 1) { break }
            if ($descriptor[0] -eq 255) { break }
            if ($descriptor[0] -eq 1) {
                return [Text.Encoding]::ASCII.GetString($descriptor, 40, 32).Trim([char]0, [char]32)
            }
        }
    } finally { $file.Dispose() }
    throw "Kein gueltiger primaerer ISO-9660-Volumendeskriptor: $Path"
}

function Get-IsoBootProfile([string]$Path) {
    $label = Get-IsoVolumeLabel $Path
    $identity = "$label $([IO.Path]::GetFileNameWithoutExtension($Path))"
    $x64 = $identity -match '(?<![a-z0-9])(?:x86[-_ ]?64|amd64|64[-_ ]?bit|abiv11)(?![a-z0-9])'
    $x86 = $identity -match '(?<![a-z0-9])(?:i[3-6]86|32[-_ ]?bit|abiv0)(?![a-z0-9])'
    if ($x86 -and $x64) { return }
    if (!$x64 -and $identity -match 'icaros') { return 'Icaros' }
    if (!$x64 -and $identity -match 'aspire[-_ ]?os') { return 'AspireOS' }
    if ($identity -match 'aros[-_ ]?one') {
        if ($x64) { return 'AROSOne64' }
        if ($x86) { return 'AROSOne32' }
    }
    if ($x86) { return 'x86' }
}

function Get-AutoUpdatePreference([string]$Path, [switch]$AskAgain) {
    $saved = if (Test-Path -LiteralPath $Path -PathType Leaf) {
        ([string](Get-Content -LiteralPath $Path -Raw)).Trim()
    }
    if (!$AskAgain -and $saved -in 'enabled', 'disabled') { return $saved -eq 'enabled' }
    if (![Environment]::UserInteractive) { return $saved -eq 'enabled' }

    Add-Type -AssemblyName System.Windows.Forms
    $choice = [Windows.Forms.MessageBox]::Show(
        "Automatische AROS-Systemupdates fuer das Startabbild einschalten?`n`nBeim Start sucht der Wizard nach einer neuen offiziellen ABIv11-Boot-ISO. Installierte DH0-Systeme werden dabei nicht aktualisiert.",
        'AROS Wizard - Automatische Updates',
        [Windows.Forms.MessageBoxButtons]::YesNoCancel,
        [Windows.Forms.MessageBoxIcon]::Question,
        [Windows.Forms.MessageBoxDefaultButton]::Button2)
    if ($choice -eq [Windows.Forms.DialogResult]::Cancel) { return $saved -eq 'enabled' }
    $enabled = $choice -eq [Windows.Forms.DialogResult]::Yes
    $value = if ($enabled) { 'enabled' } else { 'disabled' }
    [IO.File]::WriteAllText($Path, $value)
    return $enabled
}

function Get-DriveSizeMiB([string]$ImageTool, [string]$Path) {
    if (!(Test-Path -LiteralPath $Path -PathType Leaf)) { return 0 }
    $info = & $ImageTool info --output=json -f qcow2 $Path
    if ($LASTEXITCODE -ne 0) { throw "Image konnte nicht gelesen werden: $Path" }
    $bytes = (($info -join "`n") | ConvertFrom-Json).'virtual-size'
    if ($bytes -lt 1MB -or ($bytes % 1MB) -ne 0) {
        throw "Image hat keine unterstuetzte Groesse in ganzen MiB: $Path"
    }
    return [long]($bytes / 1MB)
}

function Set-DriveImage([string]$ImageTool, [string]$Path, [long]$SizeMiB) {
    $current = Get-DriveSizeMiB $ImageTool $Path
    if ($current -gt $SizeMiB) { throw "Ein vorhandenes Image kann nicht verkleinert werden: $Path" }
    if ($current -eq $SizeMiB) { return }
    $size = "${SizeMiB}M"
    if ($current) {
        & $ImageTool resize -f qcow2 $Path $size | Out-Host
        if ($LASTEXITCODE -ne 0) { throw "Image konnte nicht vergroessert werden: $Path" }
        return
    }
    $temporary = $Path + '.' + [guid]::NewGuid().ToString('N') + '.tmp'
    try {
        & $ImageTool create -f qcow2 $temporary $size | Out-Host
        if ($LASTEXITCODE -ne 0) { throw "Image konnte nicht erstellt werden: $Path" }
        [IO.File]::Move($temporary, $Path)
    } finally {
        Remove-Item -LiteralPath $temporary -ErrorAction SilentlyContinue
    }
}

function Select-VirtualDrives([string]$ImageTool, [string[]]$Paths) {
    Add-Type -AssemblyName System.Windows.Forms
    Add-Type -AssemblyName System.Drawing
    $readSize = ${function:Get-DriveSizeMiB}
    $sizes = @(foreach ($path in $Paths) {
        try { Get-DriveSizeMiB $ImageTool $path } catch { 0 }
    })
    $form = New-Object Windows.Forms.Form
    try {
        $form.Text = 'AROS Wizard - Virtuelle Laufwerke'
        $form.ClientSize = [Drawing.Size]::new(665, 258)
        $form.StartPosition = 'CenterScreen'
        $form.FormBorderStyle = 'FixedDialog'
        $form.MaximizeBox = $false
        $form.MinimizeBox = $false
        $form.AutoScaleMode = 'Font'
        $fields = @()
        $pathFields = @()
        $browseButtons = @()
        for ($i = 0; $i -lt 2; $i++) {
            $y = 16 + ($i * 67)
            $label = New-Object Windows.Forms.Label
            $label.Text = "DH${i} - Image-Groesse (MiB)"
            $label.AutoSize = $true
            $label.Location = [Drawing.Point]::new(20, $y + 3)
            $size = New-Object Windows.Forms.NumericUpDown
            $size.Location = [Drawing.Point]::new(220, $y)
            $size.Width = 105
            $size.Minimum = [Math]::Max(1, $sizes[$i])
            $size.Maximum = [Math]::Max(2097152, $sizes[$i])
            $default = if ($i -eq 0) { 4096 } else { 2048 }
            $size.Value = if ($sizes[$i]) { $sizes[$i] } else { $default }
            $pathField = New-Object Windows.Forms.TextBox
            $pathField.Location = [Drawing.Point]::new(20, $y + 28)
            $pathField.Width = 525
            $pathField.ReadOnly = $true
            $pathField.Text = $Paths[$i]
            $browse = New-Object Windows.Forms.Button
            $browse.Text = 'Image waehlen'
            $browse.Location = [Drawing.Point]::new(553, $y + 27)
            $browse.Width = 92
            $browse.Add_Click({
                $picker = New-Object Windows.Forms.SaveFileDialog
                try {
                    $picker.Title = 'Vorhandenes oder neues QCOW2-Image waehlen'
                    $picker.Filter = 'QCOW2-Images (*.qcow2)|*.qcow2'
                    $picker.DefaultExt = 'qcow2'
                    $picker.OverwritePrompt = $false
                    $picker.InitialDirectory = [IO.Path]::GetDirectoryName($pathField.Text)
                    $picker.FileName = [IO.Path]::GetFileName($pathField.Text)
                    if ($picker.ShowDialog($form) -ne [Windows.Forms.DialogResult]::OK) { return }
                    $selected = $picker.FileName
                    if ([IO.Path]::GetExtension($selected) -ine '.qcow2') {
                        throw 'Bitte ein Image mit der Dateiendung .qcow2 waehlen.'
                    }
                    $existing = & $readSize $ImageTool $selected
                    $size.Minimum = [Math]::Max(1, $existing)
                    $size.Maximum = [Math]::Max(2097152, $existing)
                    $size.Value = if ($existing) { $existing } else { $default }
                    $pathField.Text = $selected
                } catch {
                    [void][Windows.Forms.MessageBox]::Show($form, $_.Exception.Message, 'AROS Wizard')
                } finally {
                    $picker.Dispose()
                }
            }.GetNewClosure())
            $fields += $size
            $pathFields += $pathField
            $browseButtons += $browse
            $form.Controls.AddRange(@($label, $size, $pathField, $browse))
        }
        $disable = New-Object Windows.Forms.CheckBox
        $disable.Text = 'DH1 deaktivieren'
        $disable.AutoSize = $true
        $disable.Location = [Drawing.Point]::new(345, 85)
        $disable.Add_CheckedChanged({
            $fields[1].Enabled = !$disable.Checked
            $pathFields[1].Enabled = !$disable.Checked
            $browseButtons[1].Enabled = !$disable.Checked
        }.GetNewClosure())
        $boot = New-Object Windows.Forms.CheckBox
        $boot.Text = 'Von DH0 starten statt von der ISO'
        $boot.AutoSize = $true
        $boot.Location = [Drawing.Point]::new(20, 152)
        $hint = New-Object Windows.Forms.Label
        $hint.Text = 'Neue Images zuerst in AROS partitionieren und formatieren.'
        $hint.AutoSize = $true
        $hint.Location = [Drawing.Point]::new(20, 187)
        $start = New-Object Windows.Forms.Button
        $start.Text = 'Starten'
        $start.Location = [Drawing.Point]::new(475, 219)
        $start.DialogResult = 'OK'
        $cancel = New-Object Windows.Forms.Button
        $cancel.Text = 'Abbrechen'
        $cancel.Location = [Drawing.Point]::new(570, 219)
        $cancel.DialogResult = 'Cancel'
        $form.Controls.AddRange(@($disable, $boot, $hint, $start, $cancel))
        $form.AcceptButton = $start
        $form.CancelButton = $cancel
        Set-MacintoshWizardTheme $form
        if ($form.ShowDialog() -eq [Windows.Forms.DialogResult]::OK) {
            return @{
                Sizes = @([long]$fields[0].Value, [long]$fields[1].Value)
                Paths = @($pathFields[0].Text, $pathFields[1].Text)
                DisableDH1 = $disable.Checked
                BootDH0 = $boot.Checked
            }
        }
    } finally {
        $form.Dispose()
    }
}

function Select-OfflineIso {
    Add-Type -AssemblyName System.Windows.Forms
    $dialog = New-Object Windows.Forms.OpenFileDialog
    try {
        $dialog.Title = if ($Distribution) { "AROS Wizard - ISO fuer $Distribution auswaehlen" } else {
            'AROS Wizard powered by ChatGPT - Offline-ISO auswaehlen'
        }
        $dialog.Filter = 'ISO-Dateien (*.iso)|*.iso'
        $dialog.CheckFileExists = $true
        if ($dialog.ShowDialog() -eq [Windows.Forms.DialogResult]::OK) { $dialog.FileName }
    } finally {
        $dialog.Dispose()
    }
}

function Select-ArosDistribution {
    Add-Type -AssemblyName System.Windows.Forms
    Add-Type -AssemblyName System.Drawing
    $names = @('AROS One 32 Bit', 'AROS One 64 Bit', 'Icaros Desktop', 'AspireOS')
    $profiles = @('AROSOne32', 'AROSOne64', 'Icaros', 'AspireOS')
    $form = New-Object Windows.Forms.Form
    try {
        $form.Text = 'AROS Wizard - Distribution auswaehlen'
        $form.ClientSize = [Drawing.Size]::new(355, 157)
        $form.StartPosition = 'CenterScreen'
        $form.FormBorderStyle = 'FixedDialog'
        $form.MaximizeBox = $false
        $list = New-Object Windows.Forms.ListBox
        $list.SetBounds(12, 12, 331, 91)
        $list.Items.AddRange($names)
        $list.SelectedIndex = 0
        $next = New-Object Windows.Forms.Button
        $next.SetBounds(152, 116, 92, 29)
        $next.Text = 'ISO waehlen'
        $next.DialogResult = 'OK'
        $cancel = New-Object Windows.Forms.Button
        $cancel.SetBounds(251, 116, 92, 29)
        $cancel.Text = 'Abbrechen'
        $cancel.DialogResult = 'Cancel'
        $form.Controls.AddRange(@($list, $next, $cancel))
        $form.AcceptButton = $next
        $form.CancelButton = $cancel
        Set-MacintoshWizardTheme $form
        if ($form.ShowDialog() -eq [Windows.Forms.DialogResult]::OK) {
            return $profiles[$list.SelectedIndex]
        }
    } finally { $form.Dispose() }
}

function Select-SmpCpuCount {
    Add-Type -AssemblyName System.Windows.Forms
    Add-Type -AssemblyName System.Drawing
    $form = New-Object Windows.Forms.Form
    try {
        $form.Text = 'AROS Wizard - SMP'
        $form.ClientSize = [Drawing.Size]::new(377, 126)
        $form.StartPosition = 'CenterScreen'
        $form.FormBorderStyle = 'FixedDialog'
        $form.MaximizeBox = $false
        $form.MinimizeBox = $false
        $form.AutoScaleMode = 'Font'
        $label = New-Object Windows.Forms.Label
        $label.Text = 'Virtuelle CPUs fuer die AROS-SMP-ISO'
        $label.AutoSize = $true
        $label.Location = [Drawing.Point]::new(16, 17)
        $count = New-Object Windows.Forms.NumericUpDown
        $count.Location = [Drawing.Point]::new(16, 44)
        $count.Minimum = 1
        $count.Maximum = 255
        $count.Value = [Math]::Min(255, [Environment]::ProcessorCount)
        $ok = New-Object Windows.Forms.Button
        $ok.Text = 'Weiter'
        $ok.Location = [Drawing.Point]::new(200, 88)
        $ok.DialogResult = 'OK'
        $cancel = New-Object Windows.Forms.Button
        $cancel.Text = 'Abbrechen'
        $cancel.Location = [Drawing.Point]::new(286, 88)
        $cancel.DialogResult = 'Cancel'
        $form.Controls.AddRange(@($label, $count, $ok, $cancel))
        $form.AcceptButton = $ok
        $form.CancelButton = $cancel
        Set-MacintoshWizardTheme $form
        if ($form.ShowDialog() -eq [Windows.Forms.DialogResult]::OK) { [int]$count.Value }
    } finally {
        $form.Dispose()
    }
}

function Save-ArosImage([string]$Release, [string]$Destination) {
    $stage = Join-Path $data ('iso-staging-' + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $stage | Out-Null
    $archivePath = Join-Path $stage 'aros.zip'
    $imagePath = Join-Path $stage 'aros.iso'
    try {
        Write-Host "Lade AROS $Release x86-64 herunter ..."
        $url = $baseUrl + "AROS-$Release-pc-x86_64-boot-iso.zip"
        Invoke-WebRequest -UseBasicParsing -Uri $url -OutFile $archivePath -TimeoutSec 600
        Add-Type -AssemblyName System.IO.Compression.FileSystem
        $archive = [IO.Compression.ZipFile]::OpenRead($archivePath)
        try {
            $images = @($archive.Entries | Where-Object { $_.Name -like '*.iso' })
            if ($images.Count -ne 1) { throw 'Das Archiv muss genau ein ISO enthalten.' }
            [IO.Compression.ZipFileExtensions]::ExtractToFile($images[0], $imagePath)
        } finally {
            $archive.Dispose()
        }

        Assert-IsoImage $imagePath
        if (Test-Path -LiteralPath $Destination -PathType Leaf) {
            $backup = "$Destination.invalid-$([guid]::NewGuid().ToString('N'))"
            [IO.File]::Replace($imagePath, $Destination, $backup)
        } else {
            [IO.File]::Move($imagePath, $Destination)
        }
    } finally {
        Remove-Item -LiteralPath $archivePath, $imagePath, $stage -ErrorAction SilentlyContinue
    }
}

function Clear-LauncherTemporaryFiles([string]$Directory) {
    $cutoff = [DateTime]::UtcNow.AddDays(-1)
    $removed = 0
    if ((Get-Item -LiteralPath $Directory -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) {
        return $removed
    }
    foreach ($entry in Get-ChildItem -LiteralPath $Directory -Force) {
        if ($entry.LastWriteTimeUtc -gt $cutoff -or
            ($entry.Attributes -band [IO.FileAttributes]::ReparsePoint)) { continue }
        if (!$entry.PSIsContainer) {
            if ($entry.Name -match '^[0-9a-f]{32}\.(chat|fullscreen)-port$') {
                try {
                    Remove-Item -LiteralPath $entry.FullName -Force
                    $removed++
                } catch { Write-Verbose $_.Exception.Message }
            }
            continue
        }
        if ($entry.Name -notmatch '^iso-staging-[0-9a-f]{32}$') { continue }
        $children = @(Get-ChildItem -LiteralPath $entry.FullName -Force)
        $unexpected = @($children | Where-Object {
            $_.PSIsContainer -or ($_.Attributes -band [IO.FileAttributes]::ReparsePoint) -or
                $_.Name -notin 'aros.zip', 'aros.iso'
        })
        if ($unexpected.Count) { continue }
        foreach ($child in $children) {
            Remove-Item -LiteralPath $child.FullName -Force -ErrorAction SilentlyContinue
        }
        if (!(Get-ChildItem -LiteralPath $entry.FullName -Force)) {
            try {
                [IO.Directory]::Delete($entry.FullName)
                $removed++
            } catch { Write-Verbose $_.Exception.Message }
        }
    }
    return $removed
}

function Assert-ShareHasNoLinks([IO.DirectoryInfo]$Folder) {
    if ($Folder.Attributes -band [IO.FileAttributes]::ReparsePoint) {
        throw "Der Austauschordner darf keine Verknuepfung sein: $Folder"
    }
    foreach ($item in Get-ChildItem -LiteralPath $Folder.FullName -Force) {
        if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint) {
            throw "Verknuepfungen sind im Austauschordner nicht erlaubt: $($item.FullName)"
        }
        if ($item.PSIsContainer) { Assert-ShareHasNoLinks $item }
    }
}

$chatBridge = {
    param([string]$ReadyPath)
    $ErrorActionPreference = 'Stop'
    $encoding = [Text.Encoding]::GetEncoding(28591)
    $model = $env:OPENAI_MODEL
    if (!$model) { $model = 'gpt-4.1-mini' }
    $headers = @{ Authorization = 'Bearer ' + $env:OPENAI_API_KEY }
    $listener = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback, 0)
    $client = $null
    try {
        $listener.Start()
        [IO.File]::WriteAllText($ReadyPath, [string]$listener.LocalEndpoint.Port)
        $client = $listener.AcceptTcpClient()
        $listener.Stop()
        $stream = $client.GetStream()
        $reader = [IO.StreamReader]::new($stream, $encoding, $false)
        $history = @()
        while ($true) {
            $line = [Text.StringBuilder]::new()
            while (($value = $reader.Read()) -ne -1 -and $value -ne 10) {
                if ($line.Length -ge 4000) { return }
                [void]$line.Append([char]$value)
            }
            if ($value -eq -1) { return }
            $prompt = $line.ToString().Trim()
            if (!$prompt) { continue }
            if ($prompt -eq '/reset') {
                $history = @()
                $answer = 'Neue Unterhaltung.'
            } else {
                $inputMessages = @($history) + @{ role = 'user'; content = $prompt }
                $body = @{
                    model = $model
                    input = $inputMessages
                    max_output_tokens = 700
                    store = $false
                } | ConvertTo-Json -Depth 6 -Compress
                try {
                    $response = Invoke-RestMethod -Method Post -Uri 'https://api.openai.com/v1/responses' `
                        -Headers $headers -ContentType 'application/json; charset=utf-8' `
                        -Body ([Text.Encoding]::UTF8.GetBytes($body)) -TimeoutSec 60
                    $answer = @(
                        foreach ($item in $response.output) {
                            if ($item.type -ne 'message') { continue }
                            foreach ($part in $item.content) {
                                if ($part.type -eq 'output_text') { $part.text }
                                if ($part.type -eq 'refusal') { $part.refusal }
                            }
                        }
                    ) -join "`n"
                    if (!$answer) { $answer = 'OpenAI hat keine Textantwort geliefert.' }
                    $history = @($inputMessages + @{ role = 'assistant'; content = $answer } |
                        Select-Object -Last 8)
                } catch {
                    $status = $_.Exception.Response
                    $answer = 'OpenAI-Anfrage fehlgeschlagen. Pruefe die Windows-Internetverbindung.'
                    if ($status) {
                        $answer = 'OpenAI-Anfrage fehlgeschlagen (HTTP ' + [int]$status.StatusCode + ').'
                    }
                }
            }
            $answer = $answer -replace '[\x00-\x08\x0B\x0C\x0E-\x1F\x7F-\x9F]', ''
            $bytes = $encoding.GetBytes($answer)
            $size = [Text.Encoding]::ASCII.GetBytes([string]$bytes.Length + "`n")
            $stream.Write($size, 0, $size.Length)
            $stream.Write($bytes, 0, $bytes.Length)
        }
    } finally {
        if ($client) { $client.Dispose() }
        $listener.Stop()
    }
}

if ($VirtualDrives -and ($Update -or $SetupOnly)) {
    throw '-VirtualDrives benoetigt einen normalen VM-Start.'
}
if ($DriverDebug -and ($Update -or $SetupOnly)) {
    throw '-DriverDebug benoetigt einen normalen VM-Start.'
}
if ($WandererFullscreen -and ($Update -or $SetupOnly)) {
    throw '-WandererFullscreen benoetigt einen normalen VM-Start.'
}

$fullscreenBridge = {
    param([string]$ReadyPath, [string]$WindowName)
    $ErrorActionPreference = 'Stop'
    Add-Type -AssemblyName System.Windows.Forms
    Add-Type @'
using System;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;

public static class WizardWindow {
    [DllImport("user32.dll")] private static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] private static extern uint GetWindowThreadProcessId(IntPtr window, out uint pid);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    private static extern int GetWindowText(IntPtr window, StringBuilder text, int capacity);
    public static bool IsForegroundQemu(string marker) {
        IntPtr window = GetForegroundWindow();
        if (window == IntPtr.Zero) return false;
        StringBuilder title = new StringBuilder(256);
        GetWindowText(window, title, title.Capacity);
        if (title.ToString().IndexOf(marker, StringComparison.Ordinal) < 0) return false;
        uint pid;
        GetWindowThreadProcessId(window, out pid);
        if (pid == 0) return false;
        try {
            using (Process process = Process.GetProcessById((int)pid))
                return process.ProcessName.Equals("qemu-system-x86_64", StringComparison.OrdinalIgnoreCase);
        } catch (ArgumentException) { return false; }
    }
}
'@

    $listener = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback, 0)
    $client = $null
    try {
        $listener.Start()
        [IO.File]::WriteAllText($ReadyPath, [string]$listener.LocalEndpoint.Port)
        $client = $listener.AcceptTcpClient()
        $listener.Stop()
        $stream = $client.GetStream()
        $line = [Text.StringBuilder]::new()
        while (($byte = $stream.ReadByte()) -ge 0) {
            if ($byte -eq 10) {
                if ($line.ToString().TrimEnd([char]13) -ceq 'AROS-WIZARD-FULLSCREEN-1' -and
                    [WizardWindow]::IsForegroundQemu($WindowName)) {
                    [Windows.Forms.SendKeys]::SendWait('^%f')
                }
                [void]$line.Clear()
            } elseif ($line.Length -lt 80) {
                [void]$line.Append([char]$byte)
            }
        }
    } finally {
        if ($client) { $client.Dispose() }
        $listener.Stop()
    }
}

if ($ChatGPT -and ($Update -or $SetupOnly)) {
    throw '-ChatGPT benoetigt einen normalen VM-Start.'
}
if ($ChatGPT -and [string]::IsNullOrWhiteSpace($env:OPENAI_API_KEY)) {
    throw 'Setze OPENAI_API_KEY unter Windows, bevor du den Chat startest.'
}

if ($WritableShare -and !$ShareWindowsFiles) {
    throw '-WritableShare benoetigt -ShareWindowsFiles.'
}

$iso = $null
$hasSmpTopology = $PSBoundParameters.ContainsKey('SmpSockets') -or
    $PSBoundParameters.ContainsKey('SmpCores') -or $PSBoundParameters.ContainsKey('SmpThreads')
$useSmp = $Smp -or $PSBoundParameters.ContainsKey('SmpCpus') -or $hasSmpTopology -or $SmpCpuModel
if ($useSmp -and ($SetupOnly -or $Update)) {
    throw 'SMP benoetigt einen normalen VM-Start.'
}
$smpArgument = $null
if ($hasSmpTopology) {
    $topologyCount = $SmpSockets * $SmpCores * $SmpThreads
    if ($topologyCount -gt 255) { throw 'Die CPU-Topologie darf hoechstens 255 virtuelle CPUs ergeben.' }
    if ($PSBoundParameters.ContainsKey('SmpCpus') -and $SmpCpus -ne $topologyCount) {
        throw '-SmpCpus muss Sockel mal Kerne mal Threads entsprechen.'
    }
    $smpArgument = "cpus=$topologyCount,sockets=$SmpSockets,cores=$SmpCores,threads=$SmpThreads"
} elseif ($useSmp) {
    if (!$PSBoundParameters.ContainsKey('SmpCpus')) {
        $SmpCpus = Select-SmpCpuCount
        if (!$SmpCpus) { return }
    }
    $smpArgument = [string]$SmpCpus
}
if (($ChooseDistribution -or $Distribution) -and ($Update -or $UpdateSettings -or $CleanupOnly -or $SetupOnly)) {
    throw 'Ein Distributionsstart kann nicht mit einem Wartungsmodus kombiniert werden.'
}
if ($ChooseDistribution -and !$Distribution) {
    $Distribution = Select-ArosDistribution
    if (!$Distribution) { return }
}
$isLegacyDistribution = $Distribution -in 'AROSOne32', 'Icaros', 'AspireOS'
if ($isLegacyDistribution -and ($useSmp -or $ChatGPT -or $WandererFullscreen)) {
    throw 'Dieses 32-Bit-Profil unterstuetzt die ABIv11-Gast-Erweiterungen des Wizards nicht.'
}
$useOfflineIso = $OfflineIso -or $PSBoundParameters.ContainsKey('IsoPath') -or $useSmp -or $Distribution
if ($useOfflineIso -and $Update) {
    throw 'Eine lokale ISO kann nicht gleichzeitig mit -Update gestartet werden.'
}
if ($useOfflineIso) {
    if (!$PSBoundParameters.ContainsKey('IsoPath')) {
        $IsoPath = Select-OfflineIso
        if (!$IsoPath) { return }
    }
    if ([string]::IsNullOrWhiteSpace($IsoPath)) { throw 'Bitte einen ISO-Pfad angeben.' }
    $file = Get-Item -LiteralPath $IsoPath
    if ($file -isnot [IO.FileInfo] -or $file.Extension -ne '.iso') {
        throw "Bitte eine ISO-Datei auswaehlen: $IsoPath"
    }
    $iso = $file.FullName
    Assert-IsoImage $iso
    if (!$Distribution -and !$useSmp) {
        $detectedProfile = Get-IsoBootProfile $iso
        if ($detectedProfile) {
            Write-Host "ISO-Startprofil erkannt: $detectedProfile"
            if ($detectedProfile -ne 'x86') { $Distribution = $detectedProfile }
            $isLegacyDistribution = $detectedProfile -ne 'AROSOne64'
            if ($isLegacyDistribution -and ($ChatGPT -or $WandererFullscreen)) {
                throw 'Die erkannte 32-Bit-ISO kann nicht mit ABIv11-Gast-Erweiterungen gestartet werden.'
            }
        }
    }
}

if ($WindowsDesktop -and ($Update -or $SetupOnly)) {
    throw '-WindowsDesktop benoetigt einen normalen VM-Start.'
}

if ($WindowsDesktop) {
    try {
        $listeners = @([Net.NetworkInformation.IPGlobalProperties]::GetIPGlobalProperties().
            GetActiveTcpListeners() | Where-Object { $_.Port -eq $DesktopPort })
    } catch [Management.Automation.MethodInvocationException] {
        $listeners = $null
        Write-Warning "VNC-Listener nicht pruefbar. Der Server muss nur auf 127.0.0.1:$DesktopPort lauschen."
    }
    if ($null -ne $listeners) {
        $loopback = @($listeners | Where-Object { $_.Address.Equals([Net.IPAddress]::Loopback) })
        $external = @($listeners | Where-Object { ![Net.IPAddress]::IsLoopback($_.Address) })
        if (!$loopback.Count -or $external.Count) {
            throw "Starte den Windows-VNC-Server nur auf 127.0.0.1:$DesktopPort."
        }
    }
}

New-Item -ItemType Directory -Path $data -Force | Out-Null
$lockPath = Join-Path $data 'launcher.lock'
try {
    $lock = [IO.File]::Open($lockPath, 'OpenOrCreate', 'ReadWrite', 'None')
} catch [IO.IOException] {
    throw "AROS wird bereits ausgefuehrt oder der Ordner ist gesperrt: $data"
}

try {
    $removed = Clear-LauncherTemporaryFiles $data
    $updatePreference = Join-Path $data 'auto-update.txt'
    if ($CleanupOnly) {
        Write-Host "Bereinigung abgeschlossen: $removed temporaere Eintraege entfernt."
        return
    }
    $qemu = $null
    if ($UpdateSettings) {
        $null = Get-AutoUpdatePreference $updatePreference -AskAgain
        return
    }
    $configuration = $null
    $bootOrder = 'd'
    $virtualDriveArgs = @()
    if ($VirtualDrives) {
        $qemu = Get-OrInstallQemu
        $imageTool = Join-Path (Split-Path -Parent $qemu) 'qemu-img.exe'
        if (!(Test-Path -LiteralPath $imageTool -PathType Leaf)) { throw 'qemu-img.exe fehlt.' }
        $driveDirectory = $data
        if ($Distribution) {
            $driveDirectory = Join-Path $data $Distribution
            New-Item -ItemType Directory -Path $driveDirectory -Force | Out-Null
        }
        $images = @((Join-Path $driveDirectory 'DH0.qcow2'), (Join-Path $driveDirectory 'DH1.qcow2'))
        $driveSettings = Join-Path $driveDirectory 'virtual-drive-paths.json'
        if (Test-Path -LiteralPath $driveSettings -PathType Leaf) {
            try {
                $savedPaths = @(Get-Content -LiteralPath $driveSettings -Raw | ConvertFrom-Json)
                if ($savedPaths.Count -eq 2 -and $savedPaths[0] -is [string] -and $savedPaths[1] -is [string]) {
                    $images = $savedPaths
                }
            } catch {
                Write-Warning 'Gespeicherte Laufwerkspfade konnten nicht gelesen werden.'
            }
        }
        $configuration = Select-VirtualDrives $imageTool $images
        if (!$configuration) { return }
        $images = $configuration.Paths
        if (!$configuration.DisableDH1 -and $images[0] -ieq $images[1]) {
            throw 'DH0 und DH1 brauchen unterschiedliche Image-Dateien.'
        }
        if ($configuration.BootDH0) {
            if (!(Test-Path -LiteralPath $images[0] -PathType Leaf)) {
                throw 'DH0 ist noch nicht vorhanden. Starte zuerst von der ISO und installiere AROS.'
            }
            $bootOrder = 'c'
        }
    }

    if (!$useOfflineIso -and $bootOrder -eq 'd') {
        if (!$Update -and !$SetupOnly -and !$PSBoundParameters.ContainsKey('AutoUpdate')) {
            $AutoUpdate = Get-AutoUpdatePreference $updatePreference
        }
        $cachedPaths = @{}
        $null = @(
            Get-ChildItem -LiteralPath $data -File -Filter 'AROS-*.iso' |
                ForEach-Object {
                    if ($_.Name -match "^AROS-($releasePattern)\.iso$") {
                        $release = $Matches[1]
                        try {
                            Assert-IsoImage $_.FullName
                            $cachedPaths[$release] = $_.FullName
                        } catch {
                            Write-Warning "ISO im Cache nicht verwendbar: $($_.Exception.Message)"
                        }
                    }
                }
        )
        $legacy = Join-Path $data 'AROS-x86-64.iso'
        if (!$cachedPaths.ContainsKey($baseline) -and (Test-Path -LiteralPath $legacy -PathType Leaf)) {
            try {
                Assert-IsoImage $legacy
                $cachedPaths[$baseline] = $legacy
            } catch {
                Write-Warning "ISO im Cache nicht verwendbar: $($_.Exception.Message)"
            }
        }
        $cached = @($cachedPaths.Keys)
        $installed = @((Get-ReleaseOrder $cached))[0]
        $target = $installed
        if ($Update -or $AutoUpdate -or !$installed) {
            try {
                $index = Invoke-WebRequest -UseBasicParsing -Uri $baseUrl -TimeoutSec 30
                $available = @([regex]::Matches(
                    $index.Content, 'href="AROS-(' + $releasePattern + ')-pc-x86_64-boot-iso\.zip"'
                ) | ForEach-Object { $_.Groups[1].Value })
                if (!$available.Count) { throw 'Keine passenden AROS-ISO-Archive gefunden.' }
                $target = @(Get-ReleaseOrder ($available + @($cached) + $baseline))[0]
            } catch {
                if ($Update) { throw }
                if ($installed) {
                    Write-Warning 'Versionsliste nicht erreichbar. Starte das vorhandene ISO.'
                    $target = $installed
                } else {
                    Write-Warning 'Versionsliste nicht erreichbar. Verwende die mitgelieferte Download-Adresse.'
                    $target = $baseline
                }
            }
        }

        if ($cached -notcontains $target) {
            try {
                $destination = Join-Path $data "AROS-$target.iso"
                Save-ArosImage $target $destination
                $cachedPaths[$target] = $destination
            } catch {
                if (!$AutoUpdate -or $Update -or !$installed) { throw }
                Write-Warning "Update auf AROS $target fehlgeschlagen. Starte AROS $installed."
                $target = $installed
            }
        }
        $iso = $cachedPaths[$target]
        if ($Update) {
            Write-Host "AROS $target ist lokal verfuegbar."
            return
        }
    }

    if (!$qemu) { $qemu = Get-OrInstallQemu }
    if ($SetupOnly) { return }
    if ($configuration) {
        $count = if ($configuration.DisableDH1) { 1 } else { 2 }
        for ($i = 0; $i -lt $count; $i++) {
            Set-DriveImage $imageTool $images[$i] $configuration.Sizes[$i]
            $escapedImage = $images[$i].Replace(',', ',,')
            $virtualDriveArgs += @('-drive', "file=$escapedImage,format=qcow2,if=ide,index=$i")
        }
        ConvertTo-Json -InputObject $images | Set-Content -LiteralPath $driveSettings -Encoding UTF8
    }

    $forwarding = @()
    if ($WindowsDesktop) {
        $forwarding += "guestfwd=tcp:10.0.2.100:5900-tcp:127.0.0.1:$DesktopPort"
        Write-Host 'Windows-Desktop: VNC-Client in AROS mit 10.0.2.100:5900 verbinden.'
    }
    if ($ChatGPT) {
        $chatReady = Join-Path $data ([guid]::NewGuid().ToString('N') + '.chat-port')
        $chatJob = Start-Job -ScriptBlock $chatBridge -ArgumentList $chatReady
        $deadline = [DateTime]::UtcNow.AddSeconds(15)
        do {
            Start-Sleep -Milliseconds 150
            $chatPort = Get-Content -LiteralPath $chatReady -ErrorAction SilentlyContinue
        } until ($chatPort -match '^[0-9]+$' -or $chatJob.State -in 'Completed', 'Failed', 'Stopped' -or
            [DateTime]::UtcNow -ge $deadline)
        if ($chatPort -notmatch '^[0-9]+$') { throw 'ChatGPT-Verbindung konnte nicht gestartet werden.' }
        $forwarding += "guestfwd=tcp:10.0.2.100:16811-tcp:127.0.0.1:$chatPort"
        Write-Host 'ChatGPT for AROS: Netzwerk in AROS starten und ChatGPT in AROS ausfuehren.'
    }
    $network = 'none'
    if ($forwarding.Count) {
        $network = 'user,model=rtl8139,restrict=on,' + ($forwarding -join ',')
    }
    $qemuArgs = @('-m', '1024', '-boot', $bootOrder, '-nic', $network) + $virtualDriveArgs
    if ($smpArgument) { $qemuArgs += @('-smp', $smpArgument) }
    if ($SmpCpuModel) { $qemuArgs += @('-cpu', $SmpCpuModel) }
    elseif ($isLegacyDistribution) { $qemuArgs += @('-cpu', 'qemu32') }
    if ($iso) { $qemuArgs += @('-cdrom', $iso) }
    if ($DriverDebug) {
        $logRoot = Join-Path $data 'Treiberlogs'
        $logDirectory = Join-Path $logRoot ([DateTime]::Now.ToString('yyyyMMdd-HHmmss') + '-' + [guid]::NewGuid().ToString('N'))
        New-Item -ItemType Directory -Path $logDirectory -Force | Out-Null
        $qemuLog = Join-Path $logDirectory 'qemu.log'
        $serialLog = Join-Path $logDirectory 'aros-seriell.log'
        $qemuArgs += @('-d', 'guest_errors,unimp', '-D', $qemuLog)
        Write-Host "Treiberlogs: $logDirectory"
        Write-Host 'Fuer AROS-Kernelmeldungen debug=serial zur Kernel-Zeile im GRUB-Menue hinzufuegen.'
    }
    if ($WandererFullscreen) {
        $sessionId = [guid]::NewGuid().ToString('N')
        $windowName = "AROS-Wizard-$sessionId"
        $fullscreenReady = Join-Path $data "$sessionId.fullscreen-port"
        $fullscreenJob = Start-Job -ScriptBlock $fullscreenBridge -ArgumentList $fullscreenReady, $windowName
        $deadline = [DateTime]::UtcNow.AddSeconds(15)
        do {
            Start-Sleep -Milliseconds 150
            $fullscreenPort = Get-Content -LiteralPath $fullscreenReady -ErrorAction SilentlyContinue
        } until ($fullscreenPort -match '^[0-9]+$' -or
            $fullscreenJob.State -in 'Completed', 'Failed', 'Stopped' -or [DateTime]::UtcNow -ge $deadline)
        if ($fullscreenPort -notmatch '^[0-9]+$') {
            throw 'Der Windows-Vollbildkanal konnte nicht gestartet werden.'
        }
        $qemuArgs += @('-display', 'gtk', '-name', $windowName)
        if ($DriverDebug) {
            $logPath = $serialLog.Replace(',', ',,')
            $qemuArgs += @('-chardev', "socket,id=wizardserial,host=127.0.0.1,port=$fullscreenPort,logfile=$logPath",
                '-serial', 'chardev:wizardserial')
        } else {
            $qemuArgs += @('-serial', "tcp:127.0.0.1:$fullscreenPort")
        }
    } elseif ($DriverDebug) {
        $qemuArgs += @('-serial', "file:$serialLog")
    }
    if ($ShareWindowsFiles) {
        $share = Join-Path $root 'AROS-Austausch'
        New-Item -ItemType Directory -Force -Path $share | Out-Null
        Assert-ShareHasNoLinks (Get-Item -LiteralPath $share -Force)
        $sharePath = $share.Replace(',', ',,')
        $mode = 'off'
        if ($WritableShare) { $mode = 'on' }
        $drive = "if=none,id=winshare,format=raw,file.driver=vvfat,file.dir=$sharePath,file.label=WINDOWS,file.rw=$mode"
        if (!$WritableShare) { $drive += ',readonly=on' }
        $qemuArgs += @('-usb', '-drive', $drive, '-device', 'usb-storage,drive=winshare')
        Write-Host "Windows-Austauschordner: $share"
    }
    $savedApiKey = $env:OPENAI_API_KEY
    try {
        Remove-Item Env:OPENAI_API_KEY -ErrorAction SilentlyContinue
        & $qemu @qemuArgs
    } finally {
        if ($null -ne $savedApiKey) { $env:OPENAI_API_KEY = $savedApiKey }
    }
    if ($LASTEXITCODE -ne 0) { throw "QEMU endete mit Fehlercode $LASTEXITCODE." }
} finally {
    if ($chatJob) { Remove-Job -Job $chatJob -Force }
    if ($chatReady) { Remove-Item -LiteralPath $chatReady -ErrorAction SilentlyContinue }
    if ($fullscreenJob) { Remove-Job -Job $fullscreenJob -Force }
    if ($fullscreenReady) { Remove-Item -LiteralPath $fullscreenReady -ErrorAction SilentlyContinue }
    $lock.Dispose()
}

