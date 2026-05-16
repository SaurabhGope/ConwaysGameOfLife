param(
    [string]$BuildDir = "build",
    [string[]]$Config = @("Debug", "Release")
)

$ProjectRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$BuildPath = Join-Path $ProjectRoot $BuildDir

foreach ($CurrentConfig in $Config)
{
    cmake --build $BuildPath --config $CurrentConfig
    if (!$?)
    {
        exit $LASTEXITCODE
    }

    $PackageName = "Life3D-0.1.0-windows-x64-$CurrentConfig"
    cpack --config (Join-Path $BuildPath "CPackConfig.cmake") -C $CurrentConfig -G ZIP -B $BuildPath -D "CPACK_PACKAGE_FILE_NAME=$PackageName"
    if (!$?)
    {
        exit $LASTEXITCODE
    }
}
