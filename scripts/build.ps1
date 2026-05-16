param(
    [string]$BuildDir = "build",
    [string]$Config = "Debug"
)

$ProjectRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$BuildPath = Join-Path $ProjectRoot $BuildDir

cmake --build $BuildPath --config $Config
if (!$?)
{
    exit $LASTEXITCODE
}
