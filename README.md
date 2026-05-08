# video

## 部署

### Windows

项目内置了 `deploy.ps1`：

```powershell
.\deploy.ps1
```

可选参数：

```powershell
.\deploy.ps1 -QtRoot "D:\Qt\6.8.2\msvc2022_64" -BuildDir "build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release" -InstallRoot "deploy"
```

输出目录默认在：

`build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release\deploy\bin`

### Ubuntu

新增 `deploy_ubuntu.sh`：

先安装常用构建依赖：

```bash
sudo apt update
sudo apt install -y build-essential qt6-base-dev qt6-base-dev-tools qmake6 pkg-config protobuf-compiler
```

```bash
chmod +x deploy_ubuntu.sh
./deploy_ubuntu.sh
```

可选参数：

```bash
./deploy_ubuntu.sh build/ubuntu-release deploy
```

脚本会执行：

1. `qmake6` 生成构建文件
2. `make -j$(nproc)` 编译
3. `make install INSTALL_ROOT=./deploy` 安装到部署目录
4. 如果系统安装了 `linuxdeployqt`，会自动尝试打包

默认输出目录：

`build/ubuntu-release/deploy/opt/untitled/bin`

## 本地依赖路径

可复制 `local_paths.pri.example` 为 `local_paths.pri`，按本机修改路径。  
`untitled.pro` 会自动包含 `local_paths.pri`（如果文件存在）。
