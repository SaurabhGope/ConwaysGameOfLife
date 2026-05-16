param(
    [string]$BuildDir = "build",
    [string]$Config = "Debug"
)

$ProjectRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$BuildPath = Join-Path $ProjectRoot $BuildDir

ctest --test-dir $BuildPath -C $Config --output-on-failure
if (!$?)
{
    exit $LASTEXITCODE
}
