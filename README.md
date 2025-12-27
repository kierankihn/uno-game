# UNO

这是一个使用 C++ 编写的现代化 GUI 的 UNO 游戏。

## 快速开始

### 预构建二进制

从 Release 中下载可执行文件，运行服务端后，使用客户端连接上即可游玩。

### 从源码构建

1. 确保你已经安装有 CMake + Visual Studio
2. 使用 vcpkg 或其他工具安装以下依赖：argparse, asio, nlohmann-json, spdlog, slint
3. 激活 Visual Studio 环境变量
4. 执行以下代码构建：

```shell
mkdir build
cd build
cmake ..
cmake --build .
```

## TODO

- [ ] 添加喊 UNO 相关功能
- [ ] 完善客户端和服务端对异常的处理，加强鲁棒性

## 许可证

[MIT](./LICENSE)