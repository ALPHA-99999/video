param(
    [string]$QtRoot = "D:\Qt\6.8.2\msvc2022_64",
    [string]$BuildDir = "build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release",
    [string]$InstallRoot = "deploy"
)

$ErrorActionPreference = "Stop"

function Assert-PathExists {
    param([string]$PathToCheck, [string]$Description)
    if (-not (Test-Path -LiteralPath $PathToCheck)) {
        throw "$Description 不存在: $PathToCheck"
    }
}

$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectFile = Join-Path $ProjectRoot "untitled.pro"
$BuildPath = Join-Path $ProjectRoot $BuildDir
$InstallPath = Join-Path $BuildPath $InstallRoot
$DeployBinPath = Join-Path $InstallPath "bin"

$VsDevCmd = "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
$Qmake = Join-Path $QtRoot "bin\qmake.exe"
$WinDeployQt = Join-Path $QtRoot "bin\windeployqt.exe"

Assert-PathExists -PathToCheck $ProjectFile -Description "项目文件"
Assert-PathExists -PathToCheck $BuildPath -Description "构建目录"
Assert-PathExists -PathToCheck $VsDevCmd -Description "VS 开发环境脚本"
Assert-PathExists -PathToCheck $Qmake -Description "qmake"
Assert-PathExists -PathToCheck $WinDeployQt -Description "windeployqt"

Write-Host "==> 项目目录: $ProjectRoot"
Write-Host "==> 构建目录: $BuildPath"
Write-Host "==> Qt 目录: $QtRoot"

Push-Location $BuildPath
try {
    if (Test-Path -LiteralPath $InstallPath) {
        Remove-Item -LiteralPath $InstallPath -Recurse -Force
    }

    $cmd = "call `"$VsDevCmd`" -arch=x64 -host_arch=x64 && " +
           "`"$Qmake`" `"..\..\untitled.pro`" CONFIG+=release && " +
           "nmake /f Makefile.Release && " +
           "nmake /f Makefile.Release install INSTALL_ROOT=.\$InstallRoot"

    Write-Host "==> 执行 qmake + nmake install ..."
    cmd /c $cmd
    if ($LASTEXITCODE -ne 0) {
        throw "qmake/nmake install 失败，退出码: $LASTEXITCODE"
    }

    $ExePath = Join-Path $DeployBinPath "untitled.exe"
    Assert-PathExists -PathToCheck $ExePath -Description "部署后的可执行文件"

    Write-Host "==> 执行 windeployqt ..."
    & $WinDeployQt --release --dir $DeployBinPath $ExePath
    if ($LASTEXITCODE -ne 0) {
        throw "windeployqt 失败，退出码: $LASTEXITCODE"
    }

    Write-Host ""
    Write-Host "部署完成，输出目录: $DeployBinPath" -ForegroundColor Green
    Write-Host "可执行文件: $ExePath" -ForegroundColor Green
}
finally {
    Pop-Location
}
