param(
    [string]$PngQuant = "pngquant"
)

$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

$projectRoot = Split-Path -Parent $PSScriptRoot
$sourcePath = Join-Path $projectRoot "ICON0.PNG"
$assetDirectory = Join-Path $projectRoot "assets\vita"
$temporaryPath = Join-Path $assetDirectory "icon0-rgb.png"
$outputPath = Join-Path $assetDirectory "icon0.png"

New-Item -ItemType Directory -Force -Path $assetDirectory | Out-Null

$source = [System.Drawing.Image]::FromFile($sourcePath)
$bitmap = New-Object System.Drawing.Bitmap 128, 128, ([System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
$graphics.Clear([System.Drawing.Color]::FromArgb(13, 19, 31))
$graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
$graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
$graphics.DrawImage($source, 0, 0, 128, 128)
$graphics.Dispose()
$source.Dispose()
$bitmap.Save($temporaryPath, [System.Drawing.Imaging.ImageFormat]::Png)
$bitmap.Dispose()

& $PngQuant --force --strip --speed 1 --output $outputPath 128 $temporaryPath
if ($LASTEXITCODE -ne 0) {
    throw "pngquant failed with exit code $LASTEXITCODE"
}

Remove-Item -LiteralPath $temporaryPath
Write-Output "Created $outputPath"
