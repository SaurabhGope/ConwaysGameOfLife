param(
    [string]$BuildDir = "build",
    [string]$Config = "Debug"
)

$ProjectRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$BuildPath = Join-Path $ProjectRoot $BuildDir
$cmakeArgs = @(
    "-S", $ProjectRoot,
    "-B", $BuildPath
)

if ($env:CMAKE_GENERATOR -and ($env:CMAKE_GENERATOR -notmatch "Visual Studio"))
{
    $cmakeArgs += "-DCMAKE_BUILD_TYPE=$Config"
}

cmake @cmakeArgs
if (!$?)
{
    exit $LASTEXITCODE
}
