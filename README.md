# video

## 部署

### Ubuntu / Linux

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

默认输出目录：

`build/ubuntu-release/deploy/opt/untitled/bin`

### 打包部署

如果你想把程序带到另一台 Linux 电脑上运行，直接生成一个可搬运包：

```bash
bash package_ubuntu.sh
```

输出：

1. 可执行包目录：`dist/untitled-linux-x86_64/`
2. 压缩包：`dist/untitled-linux-x86_64.tar.gz`

解压后运行：

```bash
./run.sh
```

包内会包含：

- 程序本体
- 运行依赖库
- Qt `xcb` 平台插件
- 启动 wrapper

## 本地依赖路径

可复制 `local_paths.pri.example` 为 `local_paths.pri`，按本机修改路径。  
当前仓库默认从 `/home/arty/Documents/video_ros2/install` 读取 `QMQTT` 的头文件和 `libqmqtt.so`，如果你把它放到了别的位置，只需要改 `local_paths.pri` 里的 `QMQTT_ROOT`。
