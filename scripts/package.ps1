param(
    [string]$BuildDir = "build",
    [string]$Config = "Debug"
)

$ProjectRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$BuildPath = Join-Path $ProjectRoot $BuildDir

cmake --build $BuildPath --config $Config --target package
if (!$?)
{
    exit $LASTEXITCODE
}
