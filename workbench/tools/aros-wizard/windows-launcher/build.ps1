param([string]$OutputDirectory = (Join-Path $PSScriptRoot 'dist'))
$ErrorActionPreference = 'Stop'
$source = Join-Path $PSScriptRoot 'ArosSetup.cs'
$script = Join-Path $PSScriptRoot 'AROS.ps1'
if (!(Test-Path -LiteralPath $source -PathType Leaf)) { throw "Missing source: $source" }
if (!(Test-Path -LiteralPath $script -PathType Leaf)) { throw "Missing script: $script" }
$compiler = Join-Path $env:WINDIR 'Microsoft.NET\Framework64\v4.0.30319\csc.exe'
if (!(Test-Path -LiteralPath $compiler -PathType Leaf)) { throw "Missing .NET Framework C# compiler: $compiler" }
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$target = Join-Path $OutputDirectory 'AROS-Setup-x64.exe'
& $compiler /nologo /target:exe /platform:x64 /out:$target "/resource:$script,AROS.ps1" $source
if ($LASTEXITCODE -ne 0) { throw "C# build failed: $LASTEXITCODE" }
& $target --self-test
if ($LASTEXITCODE -ne 0) { throw "Embedded script self-test failed: $LASTEXITCODE" }
Write-Host "Build and self-test passed: $target"
