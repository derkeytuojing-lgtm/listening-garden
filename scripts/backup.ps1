# Save tracked, untracked and ignored files, including unfinished edits.
$ErrorActionPreference = 'Stop'
$project = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$folder = Join-Path $env:LOCALAPPDATA 'ListeningGarden/Backups'
$null = New-Item -ItemType Directory -Force $folder
$stamp = [DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss-fffffff', [Globalization.CultureInfo]::InvariantCulture)
$revision = git -C $project rev-parse --short HEAD
if ($LASTEXITCODE) { throw 'Cannot read project revision.' }
$archive = Join-Path $folder "garden-$stamp-$revision.zip"
tar.exe -a -cf $archive --exclude=.git -C $project .
if ($LASTEXITCODE) { throw "Backup failed: $archive" }
$entries = tar.exe -tf $archive
if ($LASTEXITCODE -or $entries -notcontains './share/ListeningGarden.c') { throw "Invalid backup: $archive" }
Write-Output $archive
